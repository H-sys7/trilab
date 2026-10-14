#!/usr/bin/env bash
# TriLab — tests de l'outil tri du projet final (contrat : final/CONSIGNES.md).
# NE PAS MODIFIER CE FICHIER.
# Usage : test_cli.sh --bin <tri> --out <dossier> --mode test|check|memcheck
# Ajoute son score à <dossier>/result_<mode>, puis donne le verdict du projet final.

set -u
export LC_ALL=C.UTF-8

BIN=
OUT=build
MODE=test
while [ $# -gt 0 ]; do
    case "$1" in
        --bin) BIN=$2; shift 2 ;;
        --out) OUT=$2; shift 2 ;;
        --mode) MODE=$2; shift 2 ;;
        *) echo "test_cli.sh : option inconnue « $1 »" >&2; exit 2 ;;
    esac
done

if [ -t 1 ] && [ -z "${NO_COLOR:-}" ]; then
    RED=$'\e[31m'; GREEN=$'\e[32m'; YELLOW=$'\e[33m'; BOLD=$'\e[1m'; DIM=$'\e[2m'; RESET=$'\e[0m'
else
    RED=; GREEN=; YELLOW=; BOLD=; DIM=; RESET=
fi

TIMEOUT=10
RUNNER=()
if [ "$MODE" = memcheck ]; then
    TIMEOUT=200
    read -r -a RUNNER <<< "${TL_VALGRIND:-valgrind -q --leak-check=full \
--errors-for-leak-kinds=definite,indirect --track-origins=yes --error-exitcode=42}"
fi
export ASAN_OPTIONS="exitcode=23:detect_leaks=1"
export UBSAN_OPTIONS="exitcode=24:halt_on_error=1"

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
BIN_ABS=$(cd "$(dirname "$BIN")" && pwd)/$(basename "$BIN")

pass=0
total=0
nfail=0
detail=""

show() { # rend visibles les fins de ligne d'un texte court
    local s=${1//$'\n'/\\n}
    s=${s//$'\t'/\\t}
    [ ${#s} -gt 60 ] && s="${s:0:60}…"
    printf '"%s"' "$s"
}

# t <description> <entrée standard> <sortie attendue> <code attendu> <motif stderr> <question>
#   [--] [arguments de tri...]
t() {
    local desc=$1 input=$2 expected=$3 code_exp=$4 pattern=$5 question=$6
    shift 6
    [ "${1:-}" = -- ] && shift
    total=$((total + 1))
    printf '%s' "$input" > "$TMP/stdin.txt"
    (cd "$TMP" && timeout -k 2 "$TIMEOUT" "${RUNNER[@]}" "$BIN_ABS" "$@" \
        < stdin.txt > stdout.txt 2> stderr.txt)
    local code=$?
    local out err
    out=$(cat "$TMP/stdout.txt"; printf x)
    out=${out%x}
    err=$(cat "$TMP/stderr.txt")
    local ok=1 why=""
    if grep -q 'ERROR: AddressSanitizer\|ERROR: LeakSanitizer\|runtime error:' "$TMP/stderr.txt" \
       || [ $code -eq 42 ]; then
        ok=0
        why="erreur mémoire ou comportement indéfini détecté :"$'\n'
        why+=$(grep -m1 'ERROR:\|runtime error:\|Invalid\|uninitialised\|definitely lost' \
                   "$TMP/stderr.txt" | sed 's/^==[0-9]*== *//')
        why+=$'\n'$(grep -o '[a-z_]*\.c:[0-9]*' "$TMP/stderr.txt" | grep 'tri\.c' | head -n3 \
                    | sed 's/^/      à /')
    elif [ $code -eq 124 ] || [ $code -eq 137 ]; then
        ok=0
        why="le programme ne s'est pas terminé (arrêt après $TIMEOUT s)."
    elif [ $code -ge 128 ]; then
        ok=0
        why="le programme a planté (code $code)."
    elif [ $code -ne "$code_exp" ]; then
        ok=0
        why="code de sortie $code au lieu de $code_exp."
    elif [[ "$expected" == ^* ]]; then
        if ! printf '%s\n' "$out" | head -n1 | grep -qE "$expected"; then
            ok=0
            why="la sortie standard devrait commencer par « ${expected#^} »."
        fi
    elif [ "$code_exp" -eq 0 ] && [ "$out" != "$expected" ]; then
        ok=0
        why="la sortie standard n'est pas celle attendue."
    fi
    if [ $ok = 1 ] && [ -n "$pattern" ] && ! printf '%s\n' "$err" | grep -qE "$pattern"; then
        ok=0
        why="le message d'erreur ne correspond pas (attendu : une ligne qui contient"
        why+=" « ${pattern#^} »)."
    fi
    if [ $ok = 1 ] && [ "$code_exp" -ne 0 ] && [ -n "$out" ]; then
        ok=0
        why="en cas d'erreur, rien ne doit être écrit sur la sortie standard."
    fi
    if [ $ok = 1 ]; then
        pass=$((pass + 1))
        return
    fi
    nfail=$((nfail + 1))
    [ -n "$detail" ] && return
    detail="  ${RED}[ÉCHEC]${RESET} $desc"$'\n'
    detail+="    commande : tri $*  (entrée : $(show "$input"))"$'\n'
    detail+="    attendu  : code $code_exp"
    if [[ "$expected" == ^* ]]; then
        detail+=", sortie qui commence par « ${expected#^} »"
    elif [ "$code_exp" -eq 0 ]; then
        detail+=", sortie $(show "$expected")"
    fi
    detail+=$'\n'"    obtenu   : code $code, sortie $(show "$out")"$'\n'
    [ -n "$err" ] && detail+="    erreur   : $(show "$(printf '%s' "$err" | head -n1)")"$'\n'
    detail+="    Symptôme : $(printf '%s' "$why" | fold -s -w 58 | sed '2,$s/^/               /')"
    detail+=$'\n'"    Question : $(printf '%s' "$question" | fold -s -w 58 \
                                  | sed '2,$s/^/               /')"$'\n'
}

Q_SORT="Si tu n'as pas commencé l'outil tri, c'est normal. Sinon : lis-tu chaque ligne, \
puis écris-tu les nombres triés, un par ligne ?"
Q_LINE="Comment vérifies-tu qu'une ligne contient un entier, et seulement un entier ? \
Comptes-tu les lignes à partir de 1 ?"
Q_USAGE="Ton analyse de argv traite-t-elle ce cas ? Relis le tableau des codes de sortie de \
final/CONSIGNES.md."

printf '10\n-4\n7\n' > "$TMP/nombres.txt"
awk 'BEGIN { srand(42); for (i = 0; i < 50000; i++) print int(rand() * 2000000) - 1000000 }' \
    > "$TMP/grand.txt"
big_sorted=$(sort -n "$TMP/grand.txt"; printf x)
big_sorted=${big_sorted%x}
big_input=$(cat "$TMP/grand.txt"; printf x)
big_input=${big_input%x}
list_expected="bulle stable
selection instable
insertion stable
cocktail stable
peigne instable
shell instable
fusion stable
rapide instable
abr stable
avl stable
tas instable
comptage stable
radix stable
introsort instable
timsort stable
"

t "trier l'entrée standard" $'3\n1\n2\n' $'1\n2\n3\n' 0 "" "$Q_SORT"
t "trier un fichier" "" $'-4\n7\n10\n' 0 "" \
  "Ouvres-tu le fichier donné en argument à la place de l'entrée standard ?" -- nombres.txt
t "dernière ligne sans retour à la ligne" $'3\n1\n2' $'1\n2\n3\n' 0 "" \
  "La dernière ligne compte-t-elle, même sans « \\n » à la fin ?"
t "entrée vide" "" "" 0 "" "Une entrée vide est-elle une erreur ?"
t "espaces autour des nombres" $' 12 \n\t-3\t\n' $'-3\n12\n' 0 "" \
  "Acceptes-tu des espaces et des tabulations avant et après le nombre ?"
t "-r : ordre décroissant" $'3\n1\n2\n' $'3\n2\n1\n' 0 "" \
  "Que fais-tu de l'option -r ?" -- -r
t "-a fusion" $'5\n5\n-1\n' $'-1\n5\n5\n' 0 "" \
  "Retrouves-tu l'algorithme à partir de son nom avec tri_algo_name ?" -- -a fusion
t "-a comptage" $'9\n0\n4\n4\n' $'0\n4\n4\n9\n' 0 "" \
  "Le comptage passe-t-il par tri_sort_int ?" -- -a comptage
t "-a radix et valeurs extrêmes" $'2147483647\n-2147483648\n0\n' \
  $'-2147483648\n0\n2147483647\n' 0 "" "Lis-tu INT_MIN et INT_MAX sans débordement ?" \
  -- -a radix
t "options mélangées" "" $'10\n7\n-4\n' 0 "" \
  "Les options peuvent-elles venir avant ou après le fichier, dans n'importe quel ordre ?" \
  -- -a tas nombres.txt -r
t "-s avec un algorithme stable" $'2\n1\n' $'1\n2\n' 0 "" \
  "Avec -s, un algorithme stable est-il accepté ?" -- -s -a fusion
t "-l : liste des algorithmes" "" "$list_expected" 0 "" \
  "Le format est-il exactement « nom stable » ou « nom instable », un par ligne ?" -- -l
t "-h : aide" "" "^Usage" 0 "" "Que fais-tu de l'option -h ?" -- -h
t "50 000 nombres" "$big_input" "$big_sorted" 0 "" \
  "Ton tableau grandit-il avec realloc quand il est plein ?"
t "ligne non entière" $'1\nabc\n3\n' "" 2 "^tri: ligne 2" "$Q_LINE"
t "ligne vide au milieu" $'1\n\n3\n' "" 2 "^tri: ligne 2" "$Q_LINE"
t "nombre décimal" $'1.5\n' "" 2 "^tri: ligne 1" "$Q_LINE"
t "nombre trop grand pour un int" $'4\n2147483648\n' "" 2 "^tri: ligne 2" \
  "Comment détectes-tu qu'un nombre dépasse INT_MAX ? Que fait strtol dans ce cas ?"
t "fichier introuvable" "" "" 1 "^tri: " \
  "Que renvoie fopen quand le fichier n'existe pas ?" -- absent.txt
t "option inconnue" "" "" 3 "^tri: " "$Q_USAGE" -- -x
t "-a sans nom d'algorithme" "" "" 3 "^tri: " "$Q_USAGE" -- -a
t "algorithme inconnu" $'1\n' "" 3 "^tri: " "$Q_USAGE" -- -a magique
t "-s avec un algorithme instable" $'1\n' "" 3 "^tri: " \
  "Avec -s, interroges-tu tri_algo_is_stable ?" -- -s -a rapide
t "deux fichiers" "" "" 3 "^tri: " "$Q_USAGE" -- nombres.txt nombres.txt
t "comptage refusé (plage trop grande)" $'2147483647\n-2147483648\n' "" 4 "^tri: " \
  "Que fais-tu quand libtri renvoie autre chose que TRI_OK ?" -- -a comptage

title="Outil tri (cli/tri.c)"
ndots=$(( 38 - ${#title} ))
dots=$(printf '%*s' "$ndots" '' | tr ' ' '.')
echo
if [ $pass -eq $total ]; then
    echo "$title $dots $pass/$total ${GREEN}✓${RESET}"
else
    echo "$title $dots $pass/$total"
    printf '%s' "$detail"
    [ $nfail -gt 1 ] && echo "  ${DIM}(+ $((nfail - 1)) autre(s) test(s) en échec)${RESET}"
fi

# Score total : bibliothèque + outil.
lib_p=0
lib_t=0
if [ -f "$OUT/result_$MODE" ]; then
    read -r lib_p lib_t < "$OUT/result_$MODE"
fi
all_p=$((lib_p + pass))
all_t=$((lib_t + total))
mkdir -p "$OUT"
echo "$all_p $all_t" > "$OUT/result_$MODE"

echo
if [ $all_p -ne $all_t ]; then
    echo "${BOLD}Score total : $all_p/$all_t${RESET}"
    exit 1
fi
case "$MODE" in
    check)
        echo "${BOLD}Score total : $all_p/$all_t · make check ${GREEN}✓${RESET}${BOLD}" \
             "· Termine la validation avec make memcheck.${RESET}"
        ;;
    memcheck)
        if [ -f "$OUT/result_check" ] && read -r cp ct < "$OUT/result_check" \
           && [ "$cp" = "$ct" ]; then
            echo "${BOLD}Score total : $all_p/$all_t · ${GREEN}Projet final validé ✓${RESET}"
            echo "Il reste : make bench (tri-bench) et RAPPORT.md."
        else
            echo "${BOLD}Score total : $all_p/$all_t · Aucune erreur Valgrind ✓ · Il reste" \
                 "make check.${RESET}"
        fi
        ;;
    *)
        echo "${BOLD}Score total : $all_p/$all_t · Tout passe. Valide avec make check.${RESET}"
        ;;
esac
exit 0

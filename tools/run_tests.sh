#!/usr/bin/env bash
# TriLab — lanceur de tests : 1 test = 1 processus, timeout, détection de crash.
# NE PAS MODIFIER CE FICHIER.
#
# Usage : run_tests.sh --mode test|check|memcheck --bin <binaire> --src "<fichiers .c>"
#                      [--out <dossier>] [--ex-num NN]
# Variables : BONUS=1 ajoute les étapes bonus ; TL_TIMEOUT change le timeout par défaut (5 s).

set -u
export LC_ALL=C.UTF-8

MODE=test
BIN=
SRCS=
OUT=build
EX_NUM=
while [ $# -gt 0 ]; do
    case "$1" in
        --mode) MODE=$2; shift 2 ;;
        --bin) BIN=$2; shift 2 ;;
        --src) SRCS=$2; shift 2 ;;
        --out) OUT=$2; shift 2 ;;
        --ex-num) EX_NUM=$2; shift 2 ;;
        *) echo "run_tests.sh : option inconnue « $1 »" >&2; exit 2 ;;
    esac
done

if [ ! -x "$BIN" ]; then
    echo "Binaire de test introuvable : $BIN" >&2
    exit 2
fi

BONUS=${BONUS:-0}
DEFAULT_TIMEOUT=${TL_TIMEOUT:-5}
FACTOR=1
RUNNER=()
if [ "$MODE" = memcheck ]; then
    FACTOR=20
    read -r -a RUNNER <<< "${TL_VALGRIND:-valgrind -q --leak-check=full \
--errors-for-leak-kinds=definite,indirect --track-origins=yes --error-exitcode=42}"
fi

export ASAN_OPTIONS="exitcode=23:detect_leaks=1:abort_on_error=0:print_summary=1"
export UBSAN_OPTIONS="exitcode=24:print_stacktrace=1:halt_on_error=1"

# --- Couleurs (seulement dans un terminal) ---
if [ -t 1 ] && [ -z "${NO_COLOR:-}" ]; then
    RED=$'\e[31m'; GREEN=$'\e[32m'; YELLOW=$'\e[33m'; BOLD=$'\e[1m'; DIM=$'\e[2m'; RESET=$'\e[0m'
else
    RED=; GREEN=; YELLOW=; BOLD=; DIM=; RESET=
fi

# Expression régulière qui reconnaît les fichiers de l'apprenant dans un rapport.
SRC_RE=""
for s in $SRCS; do
    b=$(basename "$s" | sed 's/[.]/[.]/g')
    SRC_RE="${SRC_RE:+$SRC_RE|}$b"
done
SRC_RE="(^|/)(${SRC_RE:-__aucun__}):[0-9]+"

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

# --- Lecture des tables du binaire ---
IFS=$'\t' read -r EX_TITLE_NUM EX_TITLE < <("$BIN" --title)

STEP_NUM=(); STEP_BONUS=(); STEP_NAME=()
while IFS=$'\t' read -r num bonus name; do
    if [ "$bonus" = 1 ] && [ "$BONUS" != 1 ]; then
        continue
    fi
    STEP_NUM+=("$num"); STEP_BONUS+=("$bonus"); STEP_NAME+=("$name")
done < <("$BIN" --steps)

T_ID=(); T_STEP=(); T_TIMEOUT=(); T_LABEL=()
while IFS=$'\t' read -r id step timeout label; do
    keep=0
    for s in "${STEP_NUM[@]}"; do
        [ "$s" = "$step" ] && keep=1
    done
    [ $keep = 1 ] || continue
    T_ID+=("$id"); T_STEP+=("$step"); T_TIMEOUT+=("$timeout"); T_LABEL+=("$label")
done < <("$BIN" --list)

# --- Traduction des rapports ---

# Question fournie par le test (@@plantage / @@temps), sinon la question par défaut.
hint() { # fichier, marqueur, défaut
    local q
    q=$(grep -m1 "^@@$2 " "$1" | sed "s/^@@$2 //")
    printf '%s' "${q:-$3}"
}

# Affiche « prefix + texte » coupé à 74 colonnes.
wrap() { # prefix, texte
    local prefix=$1 text=$2 indent line word prev=""
    local -
    set -f # un « ? » isolé ne doit pas être pris pour un motif de fichier
    indent=$(printf '%*s' "${#prefix}" '')
    line=$prefix
    for word in $text; do
        # Ponctuation et » ne commencent jamais une ligne ; « ne la termine jamais.
        if [ "${#line}" -gt "${#prefix}" ] && [ $(( ${#line} + 1 + ${#word} )) -gt 74 ] \
           && [[ "$word" != [?!:\;] && "$word" != »* && "$prev" != « ]]; then
            printf '%s\n' "$line"
            line="$indent$word"
        elif [ "${#line}" -gt "${#prefix}" ]; then
            line="$line $word"
        else
            line="$line$word"
        fi
        prev=$word
    done
    printf '%s\n' "$line"
}

# Rapport AddressSanitizer / LeakSanitizer -> « type<TAB>message ».
asan_summary() { # fichier stderr
    awk -v srcre="$SRC_RE" '
    function frame(line,   f, fn, n, parts) {
        n = split(line, parts, " ")
        f = parts[n]
        fn = ""
        if (match(line, / in [^ ]+ /)) { fn = substr(line, RSTART + 4, RLENGTH - 5) }
        sub(/.*\//, "", f)
        if (f ~ /:[0-9]+:[0-9]+$/) { sub(/:[0-9]+$/, "", f) }
        return fn " (" f ")"
    }
    /ERROR: AddressSanitizer: / && kind == "" {
        s = $0; sub(/.*ERROR: AddressSanitizer: /, "", s)
        if (s ~ /^attempting double-free/) kind = "double-free"
        else if (s ~ /^attempting free/) kind = "bad-free"
        else { split(s, w, " "); kind = w[1] }
        if (s ~ /address 0x0+ /) null = 1
        stack = 1; next
    }
    /ERROR: LeakSanitizer: / && kind == "" { kind = "leak"; next }
    kind == "leak" && /(Direct|Indirect) leak of / && bytes == "" {
        s = $0; sub(/.*leak of /, "", s); split(s, w, " "); bytes = w[1]; stack = 1; next
    }
    /^(READ|WRITE) of size/ && access == "" { access = $1 }
    /The signal is caused by a WRITE/ { access = "WRITE" }
    /The signal is caused by a READ/ { access = "READ" }
    /zero page/ { null = 1 }
    /located [0-9]+ bytes (before|to the left of)/ { where = "before" }
    /located [0-9]+ bytes (after|to the right of)/ { where = "after" }
    /underflows this variable/ { where = "before" }
    /overflows this variable/ { where = "after" }
    stack && /^ *#[0-9]+ / {
        seen = 1
        n = split($0, parts, " ")
        if (parts[n] ~ srcre && nf < 3) { frames[++nf] = frame($0) }
        next
    }
    stack && seen && /^ *$/ { stack = 0 }
    /allocated by thread/ { alloc = 1; next }
    alloc && /^ *#[0-9]+ / {
        n = split($0, parts, " ")
        if (parts[n] ~ srcre && allocfr == "") { allocfr = frame($0) }
        next
    }
    alloc && /^ *$/ { alloc = 0 }
    END {
        loc = nf > 0 ? " — " frames[1] : ""
        verb = access == "WRITE" ? "écriture" : "lecture"
        if (kind == "leak") {
            if (nf > 0) {
                print "leak\tFuite mémoire : " bytes " octets alloués dans " frames[1] \
                      " ne sont jamais rendus."
            } else {
                print "leak\tFuite mémoire : " bytes " octets ne sont jamais rendus."
            }
        } else if (kind ~ /buffer-overflow$/) {
            pos = where == "before" ? "juste avant le début du tableau" : \
                  where == "after" ? "juste après la fin du tableau" : "en dehors du tableau"
            print "overflow\tErreur mémoire : " verb " " pos loc "."
        } else if (kind == "heap-use-after-free") {
            print "uaf\tErreur mémoire : " verb " dans une zone déjà rendue par free" loc "."
        } else if (kind == "double-free") {
            print "double\tErreur mémoire : la même zone est libérée deux fois" loc "."
        } else if (kind == "bad-free") {
            print "double\tErreur mémoire : free sur une adresse qui ne vient pas de malloc" loc "."
        } else if (kind == "stack-overflow") {
            print "stack\tErreur mémoire : pile dépassée" loc "."
        } else if (kind == "SEGV") {
            what = null ? "accès par un pointeur NULL" : "accès à une adresse invalide"
            print "segv\tErreur mémoire : " what loc "."
        } else {
            print "other\tErreur mémoire : " kind loc "."
        }
        for (i = 2; i <= nf; i++) print "frame\t      appelée depuis " frames[i]
        if (kind != "leak" && allocfr != "") {
            print "frame\t    Ce bloc a été alloué dans " allocfr "."
        }
    }' "$1"
}

# Rapport UndefinedBehaviorSanitizer -> « type<TAB>message » (type : calcul ou memoire).
ubsan_summary() { # fichier stderr
    awk '
    /runtime error: / && msg == "" {
        loc = $0; sub(/: runtime error: .*/, "", loc); sub(/.*\//, "", loc)
        if (loc ~ /:[0-9]+:[0-9]+$/) sub(/:[0-9]+$/, "", loc)
        msg = $0; sub(/.*runtime error: /, "", msg)
    }
    END {
        m = msg
        kind = (m ~ /out of bounds|null pointer|^pointer index|offset|misaligned/) \
               ? "memoire" : "calcul"
        if (m ~ /0xbebebebe/) {
            m = "lecture par un pointeur jamais initialisé (0xbebebebe… est le contenu " \
                "d\047un bloc de malloc jamais rempli)"
        } else if (m ~ /misaligned address/) {
            t = m; sub(/.* for type /, "", t)
            sub(/, which requires /, " exige un alignement de ", t)
            sub(/ byte alignment/, " octets", t)
            m = "accès à une adresse mal alignée : le type " t ", et l\047adresse utilisée " \
                "n\047en est pas un multiple"
        } else if (m ~ /^signed integer overflow: /) {
            sub(/^signed integer overflow: /, "", m)
            sub(/ cannot be represented in type /, " ne tient pas dans le type ", m)
            m = "débordement d\047entier signé : " m
        } else if (m ~ /out of bounds/) {
            sub(/^index /, "l\047indice ", m)
            sub(/ out of bounds for type /, " sort d\047un tableau de type ", m)
            m = "indice hors des bornes : " m
        } else if (m ~ /null pointer/) {
            m = "accès par un pointeur NULL"
        } else if (m ~ /^pointer index expression/ || m ~ /^applying .*offset/) {
            m = "calcul d\047adresse invalide : un indice est sans doute passé sous 0 " \
                "ou au-delà du tableau"
        } else if (m ~ /shift/) {
            m = "décalage de bits invalide : " m
        } else if (m ~ /^negation of/) {
            m = "négation impossible : " m
        } else if (m ~ /division/) {
            m = "division invalide : " m
        } else if (m ~ /not a valid value for type .bool./) {
            m = "valeur invalide pour un bool (variable jamais initialisée ?)"
        }
        print kind "\tComportement indéfini : " m (loc != "" ? " — " loc : "") "."
    }' "$1"
}

# Rapport Valgrind -> « type<TAB>message ».
valgrind_summary() { # fichier stderr
    awk -v srcre="$SRC_RE" '
    { sub(/^==[0-9]+== ?/, "") }
    head == "" && (/^Invalid (read|write|free)/ || /^(Conditional jump|Use of uninitialised)/ \
                   || /^(Mismatched|Source and destination|Syscall param|Argument)/) {
        head = $0; stack = 1; next
    }
    head == "" && /definitely lost in loss record|indirectly lost in loss record/ {
        head = "leak"; bytes = $1; gsub(/,/, "", bytes); stack = 1; next
    }
    head != "" && /is [0-9]+ bytes after a block/ { where = "after" }
    head != "" && /is [0-9]+ bytes before a block/ { where = "before" }
    head != "" && /inside a block of size [0-9]+ free/ { where = "freed" }
    stack && /^ +(at|by) 0x/ {
        if (match($0, /\([^()]+:[0-9]+\)$/)) {
            f = substr($0, RSTART + 1, RLENGTH - 2)
            if (f ~ srcre && nf < 3) {
                fn = $0; sub(/^ +(at|by) 0x[0-9A-Fa-f]+: /, "", fn); sub(/ \(.*/, "", fn)
                frames[++nf] = fn " (" f ")"
            }
        }
        next
    }
    stack { stack = 0 }   # fin de la première pile
    END {
        loc = nf > 0 ? " — " frames[1] : ""
        if (head == "leak") {
            print "leak\tValgrind : " bytes " octets alloués" (nf > 0 ? " dans " frames[1] : "") \
                  " ne sont jamais rendus."
        } else if (head ~ /^Invalid (read|write)/) {
            verb = head ~ /write/ ? "écriture" : "lecture"
            pos = where == "after" ? "juste après la fin d\047un bloc alloué" : \
                  where == "before" ? "juste avant le début d\047un bloc alloué" : \
                  where == "freed" ? "dans un bloc déjà rendu par free" : "à une adresse invalide"
            print "overflow\tValgrind : " verb " " pos loc "."
        } else if (head ~ /^Conditional jump/) {
            print "uninit\tValgrind : une décision (if, while...) dépend d\047une valeur " \
                  "jamais initialisée" loc "."
        } else if (head ~ /^Use of uninitialised/) {
            print "uninit\tValgrind : utilisation d\047une valeur jamais initialisée" loc "."
        } else if (head ~ /^Invalid free/ || head ~ /^Mismatched/) {
            print "double\tValgrind : free sur une zone déjà libérée ou jamais allouée" loc "."
        } else if (head != "") {
            print "other\tValgrind : " head loc "."
        } else {
            print "other\tValgrind a signalé une erreur."
        }
        for (i = 2; i <= nf; i++) print "frame\t      appelée depuis " frames[i]
    }' "$1"
}

default_question() { # type d'erreur
    case "$1" in
        leak) echo "Où se termine la vie de ce bloc, et qui doit le rendre ?" ;;
        uaf) echo "Ce bloc est-il encore utilisé après avoir été rendu ?" ;;
        double) echo "Quel chemin de ton code libère ce bloc une deuxième fois ?" ;;
        stack) echo "Quel cas de base arrête ta récursion, et est-il toujours atteint ?" ;;
        uninit) echo "Quelle case lis-tu avant d'y avoir écrit quoi que ce soit ?" ;;
        segv) echo "Quel pointeur ou quel indice est invalide à cet endroit ?" ;;
        *) echo "Que vaut ton indice à cet endroit, au moment de l'erreur ?" ;;
    esac
}

# Affiche le détail d'un test en échec.
show_failure() { # index
    local i=$1 res=${T_RES[$1]} out="$TMP/$1.out" err="$TMP/$1.err" label=${T_LABEL[$1]}
    local summary kind msg q t
    case "$res" in
        echec)
            echo "  ${RED}[ÉCHEC]${RESET} $label"
            grep -v '^@@' "$out"
            ;;
        temps)
            t=$(( ${T_TIMEOUT[$i]:-0} > 0 ? ${T_TIMEOUT[$i]} : DEFAULT_TIMEOUT ))
            t=$(( t * FACTOR ))
            echo "  ${RED}[TEMPS DÉPASSÉ]${RESET} $label, arrêt après $t s"
            echo "    Une boucle ne s'arrête probablement jamais, ou l'algorithme fait"
            echo "    beaucoup trop de travail pour cette taille."
            q="Quelle variable doit changer à chaque tour pour que la condition de ta boucle"
            q="$q devienne fausse ?"
            wrap "    Question : " "$(hint "$out" temps "$q")"
            ;;
        plantage)
            echo "  ${RED}[PLANTAGE]${RESET} $label"
            echo "    Erreur mémoire : le programme a été arrêté (${T_SIG[$i]})."
            echo "    Pour voir la ligne exacte : make check, ou make debug T=${T_ID[$i]}"
            q="Quel accès au tableau peut sortir des bornes dans ce cas ? Si ta fonction est"
            q="$q récursive : s'arrête-t-elle toujours ?"
            wrap "    Question : " "$(hint "$out" plantage "$q")"
            ;;
        asan|valgrind)
            if [ "$res" = asan ]; then
                summary=$(asan_summary "$err")
            else
                summary=$(valgrind_summary "$err")
            fi
            kind=$(printf '%s\n' "$summary" | head -n1 | cut -f1)
            msg=$(printf '%s\n' "$summary" | head -n1 | cut -f2-)
            if [ "$kind" = leak ]; then
                echo "  ${RED}[FUITE]${RESET} $label"
            else
                echo "  ${RED}[PLANTAGE]${RESET} $label"
            fi
            wrap "    " "$msg"
            printf '%s\n' "$summary" | tail -n +2 | grep '^frame' | cut -f2-
            case "$kind" in
                leak|uaf|double|stack|uninit) q=$(default_question "$kind") ;;
                *) q=$(hint "$out" plantage "$(default_question "$kind")") ;;
            esac
            wrap "    Question : " "$q"
            ;;
        ubsan)
            echo "  ${RED}[COMPORTEMENT INDÉFINI]${RESET} $label"
            summary=$(ubsan_summary "$err")
            wrap "    " "$(printf '%s' "$summary" | cut -f2-)"
            if [ "$(printf '%s' "$summary" | cut -f1)" = memoire ]; then
                q="Quel indice ou quel pointeur est invalide à cette ligne ?"
                q=$(hint "$out" plantage "$q")
            else
                q="Quel calcul, à cette ligne, peut dépasser les limites de son type ?"
                q=$(hint "$out" indefini "$q")
            fi
            wrap "    Question : " "$q"
            ;;
    esac
}

# --- Exécution : chaque test dans son propre processus ---
T_RES=(); T_SIG=()
for i in "${!T_ID[@]}"; do
    t=${T_TIMEOUT[$i]}
    [ "$t" -gt 0 ] 2>/dev/null || t=$DEFAULT_TIMEOUT
    t=$(( t * FACTOR ))
    timeout -k 2 "${t}s" "${RUNNER[@]}" "$BIN" "${T_ID[$i]}" >"$TMP/$i.out" 2>"$TMP/$i.err"
    code=$?
    sig=""
    if [ $code -eq 0 ]; then
        res=ok
    elif [ $code -eq 124 ] || [ $code -eq 137 ]; then
        res=temps
    elif grep -q 'ERROR: AddressSanitizer' "$TMP/$i.err"; then
        res=asan
    elif grep -q 'runtime error:' "$TMP/$i.err"; then
        res=ubsan
    elif grep -q '^    Symptôme' "$TMP/$i.out"; then
        res=echec   # un échec logique passe avant une éventuelle fuite
    elif grep -q 'ERROR: LeakSanitizer' "$TMP/$i.err"; then
        res=asan
    elif [ $code -eq 42 ]; then
        res=valgrind
    elif [ $code -eq 1 ]; then
        res=echec
    elif [ $code -eq 23 ]; then
        res=asan
    elif [ $code -eq 24 ]; then
        res=ubsan
    else
        res=plantage
        case $code in
            139) sig="erreur de segmentation, SIGSEGV" ;;
            134) sig="abandon, SIGABRT" ;;
            136) sig="erreur de calcul, SIGFPE" ;;
            135) sig="erreur de bus, SIGBUS" ;;
            *) sig="code de sortie $code" ;;
        esac
    fi
    T_RES[$i]=$res
    T_SIG[$i]=$sig
done

# --- Affichage par étape ---
case "$MODE" in
    check) mode_label=" ${DIM}[mode check : ASan + UBSan]${RESET}" ;;
    memcheck) mode_label=" ${DIM}[mode memcheck : Valgrind]${RESET}" ;;
    *) mode_label="" ;;
esac
echo
echo "${BOLD}== Exercice $EX_TITLE_NUM · $EX_TITLE ==${RESET}$mode_label"
echo

total=0; passed=0; core_total=0; core_passed=0
first_fail_step=""
for si in "${!STEP_NUM[@]}"; do
    s=${STEP_NUM[$si]}
    if [ "${STEP_BONUS[$si]}" = 1 ]; then
        title="Bonus · ${STEP_NAME[$si]}"
    else
        title="Étape $s · ${STEP_NAME[$si]}"
    fi
    st=0; sp=0; first_fail=""; nfail=0
    for i in "${!T_ID[@]}"; do
        [ "${T_STEP[$i]}" = "$s" ] || continue
        st=$((st + 1))
        if [ "${T_RES[$i]}" = ok ]; then
            sp=$((sp + 1))
        else
            nfail=$((nfail + 1))
            [ -n "$first_fail" ] || first_fail=$i
        fi
    done
    total=$((total + st)); passed=$((passed + sp))
    if [ "${STEP_BONUS[$si]}" != 1 ]; then
        core_total=$((core_total + st)); core_passed=$((core_passed + sp))
    fi
    ndots=$(( 38 - ${#title} )); [ $ndots -lt 3 ] && ndots=3
    dots=$(printf '%*s' "$ndots" '' | tr ' ' '.')
    if [ $sp -eq $st ]; then
        echo "$title $dots $sp/$st ${GREEN}✓${RESET}"
    elif [ -z "$first_fail_step" ]; then
        first_fail_step=$s
        echo "$title $dots $sp/$st"
        show_failure "$first_fail"
        if [ $nfail -gt 1 ]; then
            echo "  ${DIM}(+ $((nfail - 1)) autre(s) test(s) en échec dans cette étape)${RESET}"
        fi
        echo
    else
        echo "$title $dots $sp/$st · ${YELLOW}en attente${RESET}" \
             "(termine d'abord l'étape $first_fail_step)"
    fi
done

echo
# Résultat pour make status : seules les étapes obligatoires comptent.
mkdir -p "$OUT"
echo "$core_passed $core_total" > "$OUT/result_$MODE"

if [ -n "$first_fail_step" ]; then
    echo "${BOLD}Score : $passed/$total · Prochaine étape : $first_fail_step${RESET}"
    exit 1
fi

# Projet final : d'autres tests suivent (outil tri), le verdict viendra après eux.
if [ -n "${TL_SUMMARY_ONLY:-}" ]; then
    echo "${BOLD}Score : $passed/$total ${GREEN}✓${RESET}"
    exit 0
fi

num=$((10#${EX_NUM:-0}))
case "$MODE" in
    test)
        echo "${BOLD}Score : $passed/$total · Toutes les étapes passent." \
             "Valide avec make check.${RESET}"
        ;;
    check)
        if [ "$num" -ge 8 ]; then
            echo "${BOLD}Score : $passed/$total · make check ${GREEN}✓${RESET}${BOLD}" \
                 "· Termine la validation avec make memcheck.${RESET}"
        else
            echo "${BOLD}Score : $passed/$total · ${GREEN}Exercice $EX_TITLE_NUM validé ✓${RESET}"
        fi
        ;;
    memcheck)
        msg="Aucune erreur Valgrind ✓"
        if [ -f "$OUT/result_check" ] && read -r cp ct < "$OUT/result_check" \
           && [ "$cp" = "$ct" ]; then
            msg="Aucune erreur Valgrind · Exercice $EX_TITLE_NUM validé ✓"
        else
            msg="$msg · Il reste make check."
        fi
        echo "${BOLD}Score : $passed/$total · ${GREEN}$msg${RESET}"
        ;;
esac
exit 0

#!/usr/bin/env bash
# TriLab — banc de mesure (make bench) : 1 mesure = 1 processus. NE PAS MODIFIER.
# Seuls les tris des exercices validés sont mesurés. n part de 1000 et double ; un tri
# arrête de grandir dès qu'une mesure dépasse 2 s. Chaque point est une médiane.
# Variables : ALGO=<tri> (un seul tri), PROFIL=<profil>, MAX_N (défaut 4194304).

set -u
export LC_ALL=C.UTF-8

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT" || exit 1

if [ -t 1 ] && [ -z "${NO_COLOR:-}" ]; then
    BOLD=$'\e[1m'; DIM=$'\e[2m'; YELLOW=$'\e[33m'; RESET=$'\e[0m'
else
    BOLD=; DIM=; YELLOW=; RESET=
fi

LIMIT_MS=2000
MAX_N=${MAX_N:-4194304}
PROC_TIMEOUT=120
EX07=exercises/07_mesure
OUT=build/bench
declare -A PROFILE_LABEL=([aleatoire]="aléatoire" [trie]="trié" [inverse]="inversé"
                          [presque_trie]="presque trié" [unitaire]="double dans [0, 1)")

# Exercices de tri : numéro et macro du registre de bench_main.c.
SORTS=("01:BUBBLE" "02:SELECTION" "03:INSERTION" "04:COCKTAIL" "05:COMB" "06:SHELL" "08:MERGE"
       "09:QUICK" "10:BST" "11:AVL" "12:HEAP" "13:COUNTING" "14:RADIX" "15:BUCKET"
       "16:GENERIC" "17:INTRO" "18:TIM")

# Vrai si l'exercice est validé (make check, plus make memcheck dès 08) et que son code
# n'a pas changé depuis.
validated() { # dossier, numéro
    local dir=$1 num=$2 mode f p t src
    local modes=(check)
    [ "$((10#$num))" -ge 8 ] && modes+=(memcheck)
    for mode in "${modes[@]}"; do
        f="$dir/build/result_$mode"
        [ -f "$f" ] || return 1
        read -r p t < "$f"
        [ "$p" = "$t" ] || return 1
        for src in "$dir"/*.c; do
            case "$(basename "$src")" in test_*|bench_main.c) continue ;; esac
            [ "$src" -nt "$f" ] && return 1
        done
    done
    return 0
}

srcs_of() { # dossier : fichiers de l'apprenant, lus dans le Makefile de l'exercice
    local s
    for s in $(sed -n 's/^SRCS := //p' "$1/Makefile"); do
        printf '%s ' "$1/$s"
    done
}

if ! validated "$EX07" 07; then
    echo "Le banc utilise tes générateurs et ton chronomètre de l'exercice 07."
    echo "Valide-le d'abord : make check EX=07"
    exit 1
fi

# --- Compilation du banc avec les exercices validés ---
CFLAGS=$(sed -n 's/^CFLAGS_BENCH := //p' mk/config.mk)
args=(-I"$EX07")
sources=("$EX07/bench_main.c" $(srcs_of "$EX07"))
included=()
for entry in "${SORTS[@]}"; do
    num=${entry%%:*}
    macro=${entry#*:}
    dir=$(ls -d exercises/"${num}"_* 2>/dev/null | head -n1)
    [ -n "$dir" ] && validated "$dir" "$num" || continue
    args+=(-I"$dir" -DWITH_"$macro")
    sources+=($(srcs_of "$dir"))
    included+=("$num")
done

mkdir -p "$OUT"
BIN="$OUT/bench"
# shellcheck disable=SC2086
if ! gcc $CFLAGS "${args[@]}" -o "$BIN" "${sources[@]}" -lm 2>"$OUT/compile.log"; then
    echo "La compilation du banc a échoué :"
    cat "$OUT/compile.log"
    exit 1
fi

echo
echo "${BOLD}== Banc de mesure TriLab ==${RESET}"
echo "${DIM}Exercices inclus : ${included[*]:-aucun} (+ qsort comme référence)."
echo "Chaque point : médiane de 5 mesures (3 au-delà de 2 s), compilé en -O2.${RESET}"
echo "${DIM}Ratio = temps(n) / temps(n / 2) : ≈ 2 linéaire, ≈ 4 quadratique"
echo "(« - » quand le temps précédent est trop court pour être fiable).${RESET}"

ROWS="$OUT/lignes.txt"
: > "$ROWS"

while IFS=$'\t' read -r name label profiles; do
    [ -n "${ALGO:-}" ] && [ "$name" != "$ALGO" ] && continue
    for prof in ${profiles//,/ }; do
        [ -n "${PROFIL:-}" ] && [ "$prof" != "$PROFIL" ] && continue
        echo
        echo "${BOLD}$label · ${PROFILE_LABEL[$prof]}${RESET}"
        printf '  %10s  %12s  %7s\n' "n" "temps (ms)" "ratio"
        n=1000
        prev=""
        while :; do
            out=$(timeout "$PROC_TIMEOUT" "$BIN" run "$name" "$prof" "$n" 2>&1)
            code=$?
            if [ $code -ne 0 ]; then
                case $code in
                    124) msg="trop lent : plus de $PROC_TIMEOUT s" ;;
                    139) msg="pile dépassée (récursion trop profonde ?)" ;;
                    3) msg="résultat NON TRIÉ : le tri est faux pour ce profil" ;;
                    4) msg="mémoire insuffisante" ;;
                    5) msg="refusé : écart entre min et max trop grand pour la table" ;;
                    *) msg="plantage (code $code)" ;;
                esac
                printf '  %10s  %s\n' "$n" "${YELLOW}$msg${RESET}"
                break
            fi
            read -r ms runs <<< "$out"
            ratio="-"
            ratio_csv=0
            if [ -n "$prev" ] && awk -v p="$prev" 'BEGIN { exit !(p >= 0.5) }'; then
                ratio=$(awk -v a="$ms" -v p="$prev" 'BEGIN { printf "%.2f", a / p }')
                ratio_csv=$ratio
            fi
            note=""
            [ "$runs" != 5 ] && note=" ${DIM}($runs mesures)${RESET}"
            printf '  %10s  %12s  %7s%s\n' "$n" "$ms" "$ratio" "$note"
            echo "$name $prof $n $ms $ratio_csv" >> "$ROWS"
            if awk -v a="$ms" -v l="$LIMIT_MS" 'BEGIN { exit !(a > l) }'; then
                echo "  ${DIM}arrêt : plus de 2 s${RESET}"
                break
            fi
            if [ $((n * 2)) -gt "$MAX_N" ]; then
                echo "  ${DIM}arrêt : taille maximale ($MAX_N) atteinte${RESET}"
                break
            fi
            prev=$ms
            n=$((n * 2))
        done
    done
done < <("$BIN" --list)

echo
if "$BIN" csv "$ROWS" "$OUT/bench.csv"; then
    echo "Résultats écrits par ton bench_write_csv : $OUT/bench.csv"
fi
echo "Note tes observations dans $EX07/RESULTATS.md."

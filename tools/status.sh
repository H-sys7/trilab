#!/usr/bin/env bash
# TriLab — tableau de progression des 20 exercices (make status). NE PAS MODIFIER.

set -u
export LC_ALL=C.UTF-8

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT" || exit 1

if [ -t 1 ] && [ -z "${NO_COLOR:-}" ]; then
    GREEN=$'\e[32m'; YELLOW=$'\e[33m'; DIM=$'\e[2m'; BOLD=$'\e[1m'; RESET=$'\e[0m'
else
    GREEN=; YELLOW=; DIM=; BOLD=; RESET=
fi

TITLES=(
    "Échauffement : les pointeurs"
    "Tri à bulles"
    "Tri par sélection"
    "Tri par insertion"
    "Tri cocktail"
    "Tri à peigne"
    "Tri de Shell"
    "Mesure et complexité"
    "Tri fusion"
    "Tri rapide"
    "Tri par arbre binaire de recherche"
    "Tri par arbre AVL"
    "Tri par tas"
    "Tri par comptage"
    "Tri par base (radix LSD)"
    "Tri par seaux"
    "Tri générique façon qsort"
    "Introsort"
    "Timsort simplifié"
    "Projet final : libtri"
)

pad() { # texte, largeur
    local s=$1
    while [ "${#s}" -lt "$2" ]; do s="$s "; done
    printf '%s' "$s"
}

# Lit « réussis total » dans un fichier de résultat, s'il est plus récent que le code.
# Renvoie : vide si absent, « stale » si le code a changé depuis, sinon « p t ».
result() { # dossier, mode
    local f="$1/build/result_$2" src
    [ -f "$f" ] || return 0
    for src in "$1"/*.c "$1"/src/*.c "$1"/cli/*.c; do
        [ -f "$src" ] || continue
        case "$(basename "$src")" in test_*|bench_main.c) continue ;; esac
        if [ "$src" -nt "$f" ]; then
            echo stale
            return 0
        fi
    done
    cat "$f"
}

echo
echo "${BOLD}TriLab · progression${RESET}"
echo
validated=0
next=""
for n in $(seq 0 19); do
    num=$(printf '%02d' "$n")
    if [ "$n" -eq 19 ]; then
        dir=final
    else
        dir=$(ls -d exercises/"${num}"_* 2>/dev/null | head -n1)
    fi
    line="  $num  $(pad "${TITLES[$n]}" 36)"
    if [ -z "$dir" ] || [ ! -d "$dir" ]; then
        echo "$line${DIM}pas encore livré${RESET}"
        continue
    fi
    rt=$(result "$dir" test)
    rc=$(result "$dir" check)
    rmem=$(result "$dir" memcheck)
    ok_check=0
    ok_mem=0
    if [ -n "$rc" ] && [ "$rc" != stale ]; then
        read -r p t <<< "$rc"
        [ "$p" = "$t" ] && ok_check=1
    fi
    if [ -n "$rmem" ] && [ "$rmem" != stale ]; then
        read -r p t <<< "$rmem"
        [ "$p" = "$t" ] && ok_mem=1
    fi
    if [ $ok_check = 1 ] && { [ "$n" -lt 8 ] || [ $ok_mem = 1 ]; }; then
        echo "$line${GREEN}validé ✓${RESET}"
        validated=$((validated + 1))
        continue
    fi
    [ -n "$next" ] || next=$num
    if [ $ok_check = 1 ]; then
        echo "$line${YELLOW}make check ✓ · reste make memcheck${RESET}"
    elif [ "$rt" = stale ] || [ "$rc" = stale ]; then
        echo "$line${YELLOW}code modifié · relance make test${RESET}"
    elif [ -n "$rt" ]; then
        read -r p t <<< "$rt"
        if [ "$p" = "$t" ]; then
            echo "$line${YELLOW}tests OK · lance make check${RESET}"
        else
            echo "$line${YELLOW}en cours · $p/$t tests${RESET}"
        fi
    else
        echo "${line}à faire"
    fi
done
echo
if [ -n "$next" ]; then
    echo "${BOLD}Validés : $validated/20 · Prochain exercice : $next${RESET} (make test EX=$next)"
else
    echo "${BOLD}Validés : $validated/20${RESET}"
fi

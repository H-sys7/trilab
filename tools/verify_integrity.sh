#!/usr/bin/env bash
# TriLab — compare les fichiers protégés à leurs empreintes (.checksums). NE PAS MODIFIER.
# Usage : verify_integrity.sh [dossier_exercice]
#   sans argument : tout le dépôt ; avec « 01_bulle » : outils communs + cet exercice.

set -u
export LC_ALL=C.UTF-8

ROOT=$(cd "$(dirname "$0")/.." && pwd)
SUMS="$ROOT/.checksums"
FILTER=${1:-}

if [ ! -f "$SUMS" ]; then
    echo "Intégrité : fichier .checksums introuvable à la racine du projet."
    exit 1
fi

cd "$ROOT" || exit 1
bad=()
count=0
while read -r hash path; do
    [ -n "${hash:-}" ] || continue
    case "$path" in
        exercises/*)
            if [ -n "$FILTER" ] && [[ "$path" != "exercises/$FILTER/"* ]]; then
                continue
            fi
            ;;
    esac
    count=$((count + 1))
    if [ ! -f "$path" ]; then
        bad+=("$path (absent)")
    elif [ "$(sha256sum "$path" | cut -d' ' -f1)" != "$hash" ]; then
        bad+=("$path")
    fi
done < "$SUMS"

if [ ${#bad[@]} -gt 0 ]; then
    echo "Intégrité : ces fichiers protégés ont été modifiés :"
    for f in "${bad[@]}"; do
        echo "  - $f"
    done
    echo "Ils ne doivent pas changer. Récupère leur version d'origine"
    echo "(par exemple : git checkout -- <fichier>), puis relance."
    exit 1
fi
echo "Intégrité : OK ($count fichiers protégés)."

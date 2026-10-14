#!/usr/bin/env bash
# TriLab — assemble le paquet pour le relecteur IA (make review). NE PAS MODIFIER.
# Usage, depuis le dossier d'un exercice : review.sh <NAME> "<fichiers .c de l'apprenant>"

set -u
export LC_ALL=C.UTF-8
export NO_COLOR=1

NAME=$1
SRCS=$2
TOOLS=$(cd "$(dirname "$0")" && pwd)
EX_DIR=$(basename "$PWD")
NUM=${EX_DIR%%_*}
MAKE_Q=(make -s --no-print-directory)

mkdir -p build
OUTF="build/review_${NUM}.md"

echo "Préparation du paquet de relecture (compilation et tests)..."

# Compilation en mode check, forcée pour voir tous les avertissements.
rm -f "build/check/test_$NAME"
compile=$("${MAKE_Q[@]}" build-check 2>&1)
compile_ok=$?
compile=$(printf '%s\n' "$compile" | grep -v '^make')

# Sortie de make test.
tests=$("${MAKE_Q[@]}" test 2>&1 | grep -v '^make')

title=""
if [ -x "build/base/test_$NAME" ]; then
    title=$("./build/base/test_$NAME" --title | cut -f2)
fi

# Sanitizers : seulement s'ils signalent quelque chose.
sanitizers=""
if [ $compile_ok -eq 0 ]; then
    san=$(bash "$TOOLS/run_tests.sh" --mode check --bin "build/check/test_$NAME" --src "$SRCS" \
          --out build --ex-num "$NUM" 2>&1)
    if printf '%s\n' "$san" | grep -q '\[PLANTAGE\]\|\[FUITE\]\|\[COMPORTEMENT INDÉFINI\]'; then
        sanitizers=$san
    fi
fi

{
    echo "# Relecture TriLab · Exercice $NUM${title:+ · $title}"
    echo
    echo "## Ma question"
    echo
    echo "(écris ta question ici)"
    echo
    echo "## Ce que j'ai déjà essayé"
    echo
    echo "(décris ce que tu as essayé)"
    echo
    for h in *.h; do
        [ -f "$h" ] || continue
        echo "## Contrat : $h"
        echo
        echo '```c'
        cat "$h"
        echo '```'
        echo
    done
    for s in $SRCS; do
        echo "## Mon code : $s"
        echo
        echo '```c'
        cat "$s"
        echo '```'
        echo
    done
    echo "## Compilation en mode check"
    echo
    echo '```text'
    if [ -n "$compile" ]; then printf '%s\n' "$compile"; else echo "(aucun avertissement)"; fi
    echo '```'
    echo
    echo "## Tests (make test)"
    echo
    echo '```text'
    printf '%s\n' "$tests"
    echo '```'
    if [ -n "$sanitizers" ]; then
        echo
        echo "## Sanitizers (make check)"
        echo
        echo '```text'
        printf '%s\n' "$sanitizers"
        echo '```'
    fi
} > "$OUTF"

echo "Paquet prêt : $EX_DIR/$OUTF"
if command -v clip.exe >/dev/null 2>&1 && command -v iconv >/dev/null 2>&1; then
    # clip.exe attend de l'UTF-16 : sans conversion, les accents seraient abîmés.
    { printf '\xff\xfe'; iconv -f UTF-8 -t UTF-16LE "$OUTF"; } | clip.exe
    echo "Il est copié dans le presse-papiers Windows."
fi
echo "Colle-le dans le projet Claude « TriLab — Relecteur », puis complète"
echo "« Ma question » et « Ce que j'ai déjà essayé »."

#!/usr/bin/env bash
# TriLab — vérifie l'environnement : WSL2, outils, sanitizers, Valgrind (make doctor).
# NE PAS MODIFIER CE FICHIER.

set -u
export LC_ALL=C.UTF-8

ROOT=$(cd "$(dirname "$0")/.." && pwd)

if [ -t 1 ] && [ -z "${NO_COLOR:-}" ]; then
    GREEN=$'\e[32m'; RED=$'\e[31m'; YELLOW=$'\e[33m'; BOLD=$'\e[1m'; RESET=$'\e[0m'
else
    GREEN=; RED=; YELLOW=; BOLD=; RESET=
fi

problems=0
ok() { echo "  ${GREEN}✓${RESET} $1"; }
ko() {
    echo "  ${RED}✗${RESET} $1"
    [ -n "${2:-}" ] && echo "      → $2"
    problems=$((problems + 1))
}
warn() { echo "  ${YELLOW}!${RESET} $1"; [ -n "${2:-}" ] && echo "      → $2"; }

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

echo
echo "${BOLD}TriLab · vérification de l'environnement${RESET}"
echo
echo "Système"

kernel=$(uname -r)
if grep -qi microsoft /proc/version 2>/dev/null; then
    if [[ "$kernel" == *WSL2* ]] || [[ "$kernel" == *microsoft-standard* ]]; then
        ok "WSL 2 ($kernel)"
    else
        ko "WSL 1 détecté : ASan et Valgrind n'y marchent pas correctement" \
           "dans PowerShell : wsl --set-version Ubuntu 2"
    fi
else
    warn "Pas sous WSL : Linux natif ($kernel), ça marche aussi."
fi

if [ -r /etc/os-release ]; then
    . /etc/os-release
    if [ "${ID:-}" = ubuntu ] && [ "${VERSION_ID:-}" = 24.04 ]; then
        ok "Ubuntu 24.04"
    else
        warn "${PRETTY_NAME:-système inconnu} : la version de référence est Ubuntu 24.04 LTS."
    fi
fi

case "$ROOT" in
    *" "*)
        ko "Le chemin du projet contient un espace : $ROOT" \
           "make ne gère pas les espaces : place le projet dans ~/trilab" ;;
    /mnt/*)
        warn "Projet sur le disque Windows ($ROOT) : lent, avec des soucis de droits." \
             "copie-le dans ~/trilab (système de fichiers Linux)" ;;
    *)
        ok "Emplacement du projet : $ROOT" ;;
esac

echo
echo "Outils"
for tool in gcc make gdb valgrind timeout sha256sum awk; do
    if command -v "$tool" >/dev/null 2>&1; then
        ok "$tool"
    else
        case "$tool" in
            gcc|make) pkg=build-essential ;;
            timeout|sha256sum) pkg=coreutils ;;
            awk) pkg=mawk ;;
            *) pkg=$tool ;;
        esac
        ko "$tool est absent" "sudo apt install $pkg"
    fi
done
if command -v clip.exe >/dev/null 2>&1 && command -v iconv >/dev/null 2>&1; then
    ok "clip.exe (copie de make review dans le presse-papiers Windows)"
else
    warn "clip.exe introuvable : make review écrira le paquet sans le copier."
fi

echo
echo "Compilation et diagnostics"
if command -v gcc >/dev/null 2>&1; then
    cat > "$TMP/ok.c" <<'EOF'
#include <stdio.h>
int main(void)
{
    printf("ok\n");
    return 0;
}
EOF
    if gcc -std=c11 -Wall -Wextra -pedantic -o "$TMP/ok" "$TMP/ok.c" 2>"$TMP/ok.log" \
       && [ "$("$TMP/ok")" = ok ]; then
        ok "gcc compile en C11"
    else
        ko "gcc ne compile pas un programme C11 minimal" "$(head -n1 "$TMP/ok.log")"
    fi

    cat > "$TMP/asan.c" <<'EOF'
#include <stdlib.h>
int main(void)
{
    int *a = malloc(3 * sizeof *a);
    a[3] = 1;
    free(a);
    return 0;
}
EOF
    if gcc -std=c11 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
           -o "$TMP/asan" "$TMP/asan.c" 2>"$TMP/asan.log"; then
        ASAN_OPTIONS=exitcode=23 timeout 10 "$TMP/asan" >/dev/null 2>"$TMP/asan.err"
        code=$?
        if [ $code -eq 23 ] && grep -q 'heap-buffer-overflow' "$TMP/asan.err"; then
            if grep -q 'asan.c:5' "$TMP/asan.err"; then
                ok "AddressSanitizer détecte un accès hors du tableau, avec sa ligne"
            else
                warn "AddressSanitizer détecte l'erreur, mais sans numéro de ligne."
            fi
        elif grep -q 'DEADLYSIGNAL' "$TMP/asan.err" || [ $code -eq 124 ]; then
            ko "AddressSanitizer plante au démarrage (aléa d'adresses trop grand)" \
               "sudo sysctl vm.mmap_rnd_bits=28   (à refaire après chaque redémarrage de WSL)"
        else
            ko "AddressSanitizer ne détecte pas un accès hors du tableau (code $code)"
        fi
    else
        ko "gcc ne sait pas compiler avec -fsanitize=address,undefined" \
           "sudo apt install build-essential"
    fi

    cat > "$TMP/ubsan.c" <<'EOF'
#include <limits.h>
#include <stdio.h>
int main(int argc, char **argv)
{
    (void)argv;
    int x = INT_MAX - 1 + argc;
    printf("%d\n", x + 1);
    return 0;
}
EOF
    if gcc -std=c11 -g -fsanitize=address,undefined -fno-sanitize-recover=all \
           -o "$TMP/ubsan" "$TMP/ubsan.c" 2>/dev/null; then
        UBSAN_OPTIONS=exitcode=24:halt_on_error=1 ASAN_OPTIONS=exitcode=23 \
            timeout 10 "$TMP/ubsan" >/dev/null 2>"$TMP/ubsan.err"
        code=$?
        if [ $code -eq 24 ] && grep -q 'signed integer overflow' "$TMP/ubsan.err"; then
            ok "UndefinedBehaviorSanitizer détecte un débordement d'entier signé"
        else
            ko "UndefinedBehaviorSanitizer ne détecte pas un débordement (code $code)"
        fi
    fi

    cat > "$TMP/leak.c" <<'EOF'
#include <stdlib.h>
int main(void)
{
    int *volatile a = malloc(100);
    a[0] = 1;
    a = NULL;
    return 0;
}
EOF
    if gcc -std=c11 -g -fsanitize=address -o "$TMP/leak" "$TMP/leak.c" 2>/dev/null; then
        ASAN_OPTIONS=exitcode=23:detect_leaks=1 timeout 10 "$TMP/leak" 2>"$TMP/leak.err"
        code=$?
        if [ $code -eq 23 ] && grep -q 'LeakSanitizer' "$TMP/leak.err"; then
            ok "LeakSanitizer détecte une fuite mémoire"
        else
            ko "LeakSanitizer ne détecte pas une fuite (code $code)"
        fi
    fi
fi

if command -v valgrind >/dev/null 2>&1; then
    cat > "$TMP/vg.c" <<'EOF'
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    int *a = malloc(2 * sizeof *a);
    if (a[1] > 0) {
        printf("?\n");
    }
    free(a);
    return 0;
}
EOF
    if gcc -std=c11 -g -O0 -o "$TMP/vg" "$TMP/vg.c" 2>/dev/null; then
        timeout 60 valgrind -q --error-exitcode=42 "$TMP/vg" >/dev/null 2>"$TMP/vg.err"
        code=$?
        if [ $code -eq 42 ] && grep -q 'uninitialised' "$TMP/vg.err"; then
            ok "Valgrind détecte une lecture de mémoire non initialisée"
        else
            ko "Valgrind ne détecte pas une lecture non initialisée (code $code)"
        fi
    fi
fi

if command -v gdb >/dev/null 2>&1; then
    if gdb -q -batch -ex quit >/dev/null 2>&1; then
        ok "gdb démarre"
    else
        ko "gdb ne démarre pas"
    fi
fi

echo
if [ $problems -eq 0 ]; then
    echo "${BOLD}${GREEN}Tout est prêt ✓${RESET}  Commence par : make test EX=00"
    exit 0
fi
echo "${BOLD}${RED}$problems problème(s) à corriger.${RESET} Installation de base :"
echo "  sudo apt update && sudo apt install build-essential gdb valgrind"
exit 1

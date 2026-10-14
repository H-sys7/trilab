# TriLab — compilateur et drapeaux par mode. NE PAS MODIFIER CE FICHIER.

CC := gcc

# Mode base : tolérant, utilisé par make et make test.
CFLAGS_BASE := -std=c11 -Wall -Wextra -pedantic -Wshadow -Wvla -Wstrict-prototypes \
               -Wmissing-prototypes -g -O0

# Mode check : critère de validation (avertissements = erreurs, ASan + UBSan).
SAN_FLAGS := -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer
CFLAGS_CHECK := $(CFLAGS_BASE) -Werror -Wconversion $(SAN_FLAGS)
LDFLAGS_CHECK := $(SAN_FLAGS)

# Mode memcheck : binaire du mode base, exécuté sous Valgrind.
VALGRIND := valgrind -q --leak-check=full --errors-for-leak-kinds=definite,indirect \
            --track-origins=yes --error-exitcode=42

# Mode bench : mesures de temps.
CFLAGS_BENCH := -std=c11 -O2 -DNDEBUG

LDLIBS := -lm

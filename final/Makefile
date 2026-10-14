# TriLab — projet final : libtri et ses outils. NE PAS MODIFIER CE FICHIER.
# make · make test · make check · make memcheck · make bench · make debug T=<test> · make clean

SHELL := /bin/bash
ROOT := $(abspath ..)
include $(ROOT)/mk/config.mk

TOOLS := $(ROOT)/tools
FRAMEWORK := $(ROOT)/tests/framework
FW_HDRS := $(wildcard $(FRAMEWORK)/*.h)

SRC := $(sort $(wildcard src/*.c))
HDRS := include/libtri.h src/internal.h
TEST_SRC := tests/test_libtri.c
INC := -Iinclude -Isrc

OBJ_BASE := $(SRC:src/%.c=build/base/obj/%.o)
OBJ_CHECK := $(SRC:src/%.c=build/check/obj/%.o)
OBJ_BENCH := $(SRC:src/%.c=build/bench/obj/%.o)

RUN := TL_SUMMARY_ONLY=1 bash $(TOOLS)/run_tests.sh --src "$(SRC) cli/tri.c" --out build \
       --ex-num 19
CLI := bash $(TOOLS)/test_cli.sh --out build

REFUSED = { echo; echo "Compilation refusée en mode check : chaque avertissement ci-dessus"; \
            echo "compte comme une erreur. Corrige-les un par un, en partant du premier."; \
            exit 1; }

.PHONY: all test check memcheck bench debug review clean build-check

all: build/base/test_libtri build/base/tri build/tri-bench

# --- Bibliothèque : un objet par fichier de src/, puis une archive ---
build/base/obj/%.o: src/%.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_BASE) $(INC) -c $< -o $@

build/check/obj/%.o: src/%.c $(HDRS)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS_CHECK) $(INC) -c $< -o $@ || $(REFUSED)

build/bench/obj/%.o: src/%.c $(HDRS)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS_BENCH) $(INC) -c $< -o $@

build/base/libtri.a: $(OBJ_BASE)
	ar rcs $@ $^

build/check/libtri.a: $(OBJ_CHECK)
	@ar rcs $@ $^

build/bench/libtri.a: $(OBJ_BENCH)
	@ar rcs $@ $^

# --- Tests et outils ---
build/base/test_libtri: $(TEST_SRC) build/base/libtri.a $(HDRS) $(FW_HDRS)
	$(CC) $(CFLAGS_BASE) $(INC) -I$(FRAMEWORK) -o $@ $(TEST_SRC) build/base/libtri.a $(LDLIBS)

build/check/test_libtri: $(TEST_SRC) build/check/libtri.a $(HDRS) $(FW_HDRS)
	@$(CC) $(CFLAGS_CHECK) $(INC) -I$(FRAMEWORK) -o $@ $(TEST_SRC) build/check/libtri.a \
	    $(LDFLAGS_CHECK) $(LDLIBS) || $(REFUSED)

build/base/tri: cli/tri.c build/base/libtri.a include/libtri.h
	$(CC) $(CFLAGS_BASE) -Iinclude -o $@ cli/tri.c build/base/libtri.a $(LDLIBS)

build/check/tri: cli/tri.c build/check/libtri.a include/libtri.h
	@$(CC) $(CFLAGS_CHECK) -Iinclude -o $@ cli/tri.c build/check/libtri.a $(LDFLAGS_CHECK) \
	    $(LDLIBS) || $(REFUSED)

build/tri-bench: cli/tri_bench.c build/bench/libtri.a include/libtri.h
	$(CC) $(CFLAGS_BENCH) -Iinclude -o $@ cli/tri_bench.c build/bench/libtri.a $(LDLIBS)

build-check: build/check/test_libtri build/check/tri

# --- Cibles ---
test: build/base/test_libtri build/base/tri
	@$(RUN) --mode test --bin build/base/test_libtri; \
	 $(CLI) --mode test --bin build/base/tri

check:
	@bash $(TOOLS)/verify_integrity.sh final
	@$(MAKE) --no-print-directory build-check
	@$(RUN) --mode check --bin build/check/test_libtri; \
	 $(CLI) --mode check --bin build/check/tri

memcheck: build/base/test_libtri build/base/tri
	@command -v valgrind >/dev/null \
	    || { echo "Valgrind est absent : sudo apt install valgrind"; exit 1; }
	@export TL_VALGRIND="$(VALGRIND)"; \
	 $(RUN) --mode memcheck --bin build/base/test_libtri; \
	 $(CLI) --mode memcheck --bin build/base/tri

bench: build/tri-bench
	@./build/tri-bench

debug: build/base/test_libtri
	@if [ -z "$(T)" ]; then \
	    echo "Précise le test à déboguer : make debug T=<test>. Tests disponibles :"; \
	    ./build/base/test_libtri --list | awk -F'\t' '{ printf "  %-28s %s\n", $$1, $$4 }'; \
	else \
	    command -v gdb >/dev/null || { echo "gdb est absent : sudo apt install gdb"; exit 1; }; \
	    gdb -q --args ./build/base/test_libtri $(T); \
	fi

review:
	@echo "Projet final : pour une relecture, colle dans le projet « TriLab — Relecteur » le"
	@echo "fichier concerné (par exemple src/merge.c), include/libtri.h, et la sortie de"
	@echo "make test ou make check."

clean:
	rm -rf build

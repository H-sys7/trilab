# TriLab — règles communes à tous les exercices. NE PAS MODIFIER CE FICHIER.
# Chaque exercice définit NAME (nom du test) et SRCS (fichiers de l'apprenant),
# puis inclut ce fichier.

SHELL := /bin/bash
ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
include $(ROOT)/mk/config.mk

TOOLS := $(ROOT)/tools
FRAMEWORK := $(ROOT)/tests/framework
EX_DIR := $(notdir $(CURDIR))
EX_NUM := $(firstword $(subst _, ,$(EX_DIR)))

TEST_SRC := test_$(NAME).c
HDRS := $(wildcard *.h)
FW_HDRS := $(wildcard $(FRAMEWORK)/*.h)
DEPS := $(SRCS) $(TEST_SRC) $(HDRS) $(FW_HDRS)

BIN_BASE := build/base/test_$(NAME)
BIN_CHECK := build/check/test_$(NAME)

RUN := bash $(TOOLS)/run_tests.sh --src "$(SRCS)" --out build

.PHONY: all test check memcheck debug review clean build-check

all: $(BIN_BASE)

$(BIN_BASE): $(DEPS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_BASE) -I$(FRAMEWORK) -o $@ $(SRCS) $(TEST_SRC) $(LDLIBS)

$(BIN_CHECK): $(DEPS)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS_CHECK) -I$(FRAMEWORK) -o $@ $(SRCS) $(TEST_SRC) $(LDFLAGS_CHECK) $(LDLIBS) \
	    || { echo; echo "Compilation refusée en mode check : chaque avertissement ci-dessus"; \
	         echo "compte comme une erreur. Corrige-les un par un, en partant du premier."; exit 1; }

build-check: $(BIN_CHECK)

test: $(BIN_BASE)
	@$(RUN) --mode test --bin $(BIN_BASE)

check:
	@bash $(TOOLS)/verify_integrity.sh $(EX_DIR)
	@$(MAKE) --no-print-directory build-check
	@$(RUN) --mode check --bin $(BIN_CHECK) --ex-num $(EX_NUM)

memcheck: $(BIN_BASE)
	@command -v valgrind >/dev/null \
	    || { echo "Valgrind est absent : sudo apt install valgrind"; exit 1; }
	@TL_VALGRIND="$(VALGRIND)" $(RUN) --mode memcheck --bin $(BIN_BASE) --ex-num $(EX_NUM)

debug: $(BIN_BASE)
	@if [ -z "$(T)" ]; then \
	    echo "Précise le test à déboguer : make debug T=<test>. Tests disponibles :"; \
	    ./$(BIN_BASE) --list | awk -F'\t' '{ printf "  %-28s %s\n", $$1, $$4 }'; \
	else \
	    command -v gdb >/dev/null || { echo "gdb est absent : sudo apt install gdb"; exit 1; }; \
	    gdb -q --args ./$(BIN_BASE) $(T); \
	fi

review:
	@bash $(TOOLS)/review.sh "$(NAME)" "$(SRCS)"

clean:
	rm -rf build

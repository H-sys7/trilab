# TriLab — Makefile racine : délègue aux exercices. NE PAS MODIFIER CE FICHIER.
# Exemples : make test EX=01 · make check EX=01 · make status · make doctor

SHELL := /bin/bash

EX ?=
EX_PAD := $(if $(filter 0 1 2 3 4 5 6 7 8 9,$(EX)),0$(EX),$(EX))
EX_DIR := $(strip $(if $(filter 19,$(EX_PAD)),$(wildcard final),\
                $(firstword $(wildcard exercises/$(EX_PAD)_*))))

.PHONY: default help doctor status bench integrity all test check memcheck debug review clean

default:
	@if [ -n "$(EX)" ]; then $(MAKE) --no-print-directory all; \
	else $(MAKE) --no-print-directory help; fi

help:
	@echo "TriLab · commandes"
	@echo
	@echo "  make doctor              vérifie l'installation (WSL2, gcc, gdb, valgrind, ASan)"
	@echo "  make status              progression des 20 exercices"
	@echo "  make bench               mesure les tris validés (à partir de l'exercice 07)"
	@echo
	@echo "  make test EX=01          lance les tests (BONUS=1 ajoute les bonus)"
	@echo "  make check EX=01         critère de validation : intégrité + ASan/UBSan + tests"
	@echo "  make memcheck EX=08      tous les tests sous Valgrind (dès l'exercice 08)"
	@echo "  make debug EX=01 T=<t>   ouvre gdb sur un test précis"
	@echo "  make review EX=01        prépare le paquet pour le relecteur IA"
	@echo "  make clean [EX=01]       supprime build/"
	@echo
	@echo "Dans le dossier d'un exercice, les mêmes commandes s'utilisent sans EX."

doctor:
	@bash tools/doctor.sh

status:
	@bash tools/status.sh

integrity:
	@bash tools/verify_integrity.sh

bench:
	@if [ -f tools/bench.sh ]; then bash tools/bench.sh; \
	else echo "make bench arrive avec l'exercice 07 (lot 3)."; fi

all test check memcheck debug review:
	@if [ -z "$(EX)" ]; then echo "Précise l'exercice : make $@ EX=01"; exit 1; fi
	@if [ -z "$(EX_DIR)" ]; then echo "Exercice $(EX) introuvable (pas encore livré ?)."; exit 1; fi
	@$(MAKE) --no-print-directory -C $(EX_DIR) $@

clean:
	@if [ -n "$(EX)" ]; then \
	    [ -n "$(EX_DIR)" ] && $(MAKE) --no-print-directory -C $(EX_DIR) clean; \
	else \
	    for d in exercises/*/ final/; do \
	        [ -f "$$d/Makefile" ] && $(MAKE) --no-print-directory -C "$$d" clean; \
	    done; true; \
	fi

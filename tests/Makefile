# Makefile
# Copyright (c) 2026 Derek B. Clegg
# All rights reserved.

all: check

TEST_LIBS := -lhap

PRODUCTS := coin deck dice die estimate game log-combinatorics		    \
  playing-cards random reroll select stack

C++_FLAGS := -Wno-nonportable-include-path

LIBS := -lnyx

# The tests, in the order they run. Each test prints its own "Checking
# ..." description as its first line. The game and reroll programs are
# checked by scripts, which compare their output with what's expected,
# using the shared code in check-common.

TEST_PROGRAMS := random select log-combinatorics die coin dice stack deck   \
  playing-cards game-check estimate reroll-check

include makefiles/base.make

# Each check script needs its program built first.
$(foreach test, game reroll, $(eval check-$(test)-check: $(test)))

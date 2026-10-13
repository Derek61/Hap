# Makefile
# Copyright (c) 2025 Derek B. Clegg
# All rights reserved.

INSTALLED_LIBRARIES := hap

C++_FLAGS := -Wno-nonportable-include-path

hap[HEADERS] := coin.h deck.h dice.h die.h estimate.h log-combinatorics.h   \
  playing-cards.h random.h select.h stack.h

hap[SRCS] := coin.cc deck.cc dice.cc die.cc estimate.cc			    \
  log-combinatorics.cc playing-cards.cc random.cc select.cc stack.cc

hap[LIBS] := -lnyx

TEST_DIR := tests

include makefiles/base.make

# vpath %.dylib
# vpath libnyx.dylib /usr/local/lib

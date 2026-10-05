# Makefile
# Copyright (c) 2025 Derek B. Clegg
# All rights reserved.

INSTALLED_LIBRARIES := hap

C++_FLAGS := -Wno-nonportable-include-path -DHAP_USE_GENERATOR=0

PRODUCTS := game

game[LIBS] = hap -lnyx

hap[HEADERS] := coin.h deck.h dice.h die.h generator.h playing-cards.h	    \
  stack.h

hap[LIBS] := -lnyx

hap[SRCS] := coin.cc deck.cc dice.cc die.cc generator.cc playing-cards.cc   \
  stack.cc

# TEST_DIR := tests

include makefiles/base.make

vpath %.dylib
vpath libnyx.dylib /usr/local/lib

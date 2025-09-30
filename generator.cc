/* hap/generator.cc -*- mode: c++ -*-
   Copyright (c) 2025 Derek B Clegg
   All rights reserved. */

#include <hap/generator.h>

std::mt19937_64 hap::generator{std::random_device{}()};

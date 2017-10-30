// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once

struct Benchmark;

class ContextSearchBm {
public:
  static Benchmark Search(void);
  static Benchmark Cache(void);
  static Benchmark Fast(void);
};

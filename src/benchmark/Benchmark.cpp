// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "Benchmark.h"
#include "PrintableDuration.h"

static const int sc_ncbOuterLoop =
#ifdef _DEBUG
  100;
#else
  1000;
#endif

BenchmarkEntry::BenchmarkEntry(const char* name, void(*pfnBM)(Stopwatch&)) :
  name(name)
{
  // Benchmark the whole function call
  Stopwatch sw;
  for (int i = sc_ncbOuterLoop; i--;)
    pfnBM(sw);

  // Difference over scale
  duration = sw;
  duration /= sc_ncbOuterLoop;
}


std::ostream& operator<<(std::ostream& os, Benchmark& benchmark) {
  // Need a base duration to print all values against--the base duration is always the first entry
  std::chrono::duration<double> base;
  if (!benchmark.entries.empty())
    base = benchmark.entries[0].duration;

  for (auto& entry : benchmark.entries)
    os<< std::setw(40) << std::left << entry.name
      << std::setw(7) << PrintableDuration(entry.duration)
      << "  " << std::fixed << std::setprecision(2) << (100 * entry.duration.count() / base.count()) << "%"
      << std::endl;
  return os;
}

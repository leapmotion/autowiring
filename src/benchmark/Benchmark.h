// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include <chrono>
#include <initializer_list>
#include <vector>

struct Stopwatch {
  void Start(void) {
    start = std::chrono::profiling_clock::now();
  }

  /// <summary>
  /// Stops timing, with [n] operations having been conducted between start and stop
  /// </summary>
  void Stop(size_t n) {
    auto stop = std::chrono::profiling_clock::now();
    std::chrono::duration<double> delta = stop - start;
    duration += delta / static_cast<double>(n);
  }

private:
  std::chrono::profiling_clock::time_point start;
  std::chrono::duration<double> duration{0};

public:
  operator std::chrono::duration<double>(void) const { return duration; }
};

struct BenchmarkEntry {
  /// <summary>
  /// Constructs a BenchmarkEntry based on the specified benchmark routine
  /// </summary>
  /// <param name="name">The name of this entry</param>
  /// <param name="pfnBM">The benchmark routine proper</param>
  BenchmarkEntry(const char* name, void(*pfnBM)(Stopwatch&));

  /// <summary>
  /// Standard initialization constructor
  /// </summary>
  BenchmarkEntry(const char* name, std::chrono::duration<double> duration):
    name(name),
    duration(duration)
  {}

  const char* name;
  std::chrono::duration<double> duration;
};

struct Benchmark {
  Benchmark(std::initializer_list<BenchmarkEntry> entries) :
    entries(entries.begin(), entries.end())
  {}

  // Successive entries:
  std::vector<BenchmarkEntry> entries;

  friend std::ostream& operator<<(std::ostream& os, Benchmark& benchmark);
};

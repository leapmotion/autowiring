// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "Benchmark.h"
#include "ContextSearchBm.h"
#include "ContextTrackingBm.h"
#include "DispatchQueueBm.h"
#include "ObjectPoolBm.h"
#include "PrintableDuration.h"
#include "PriorityBoost.h"
#include <map>
#include <iomanip>
#include <iostream>
#include <string>

Benchmark PrintUsage(void);

struct Entry {
  bool utility = false;
  const char* name = nullptr;
  const char* desc = nullptr;
  Benchmark(*pfn)() = nullptr;
};

std::pair<std::string, Entry> MakeEntry(const char* name, const char* desc, Benchmark(*pfn)()) {
  Entry retVal;
  retVal.name = name;
  retVal.desc = desc;
  retVal.pfn = pfn;
  return std::make_pair(name, retVal);
}

std::pair<std::string, Entry> MakeUtilEntry(const char* name, const char* desc, Benchmark(*pfn)()) {
  auto retVal = MakeEntry(name, desc, pfn);
  retVal.second.utility = true;
  return retVal;
}

static Benchmark All(void);

static std::map<std::string, Entry> sc_commands = {
  MakeUtilEntry("help", "Displays this information", &PrintUsage),
  MakeUtilEntry("all", "Runs all benchmarks", &All),
  MakeEntry("priority", "Thread priority boost behavior", &PriorityBoost::CanBoostPriority),
  MakeEntry("search", "Autowiring context search cost", &ContextSearchBm::Search),
  MakeEntry("cache", "Autowiring cache behavior", &ContextSearchBm::Cache),
  MakeEntry("fast", "Autowired versus AutowiredFast", &ContextSearchBm::Fast),
  MakeEntry("dispatch", "Dispatch queue execution rate", &DispatchQueueBm::Dispatch),
  MakeEntry("contextenum", "CoreContextEnumerator profiling", &ContextTrackingBm::ContextEnum),
  MakeEntry("contextmap", "ContextMap profiling", &ContextTrackingBm::ContextMap),
  MakeEntry("objpool", "Object pool behaviors", &ObjectPoolBm::Allocation),
};

static Benchmark All(void) {
  for (auto& command : sc_commands) {
    // Skip commands that are just utility:
    if (command.second.utility)
      continue;

    Benchmark bm = command.second.pfn();
    std::cout
      << command.first << std::endl
      << bm << std::endl;
  }

  // Empty benchmark, causes the parent to avoid printing this out:
  return Benchmark{};
}

Benchmark PrintUsage(void) {
  std::cout
    << "Usage: AutoBench <command>" << std::endl
    << "Supported commands:" << std::endl
    << std::endl;

  for (auto& command : sc_commands)
    std::cout << " " << std::setw(12) << std::left << command.first << command.second.desc << std::endl;
  return Benchmark{};
}

int main(int argc, const char* argv[]) {
  if (argc <= 1) {
    PrintUsage();
    return -1;
  }

  // First pass, ensure we recognize all commands:
  for (int i = 1; i < argc; i++)
    if (!sc_commands[argv[i]].name) {
      std::cerr << "Unrecognized command" << std::endl;
      PrintUsage();
      return -1;
    }

  // Now execute everything:
  bool errors = false;
  for (int i = 1; i < argc; i++) {
    auto& cur = sc_commands[argv[i]];
    try {
      Benchmark benchmark = cur.pfn();
      if (!cur.utility)
        std::cout
          << cur.name << std::endl
          << benchmark << std::endl;
    }
    catch (std::exception& ex) {
      std::cerr << ex.what() << std::endl;
      errors = true;
    }
  }
  return errors ? -1 : 0;
}

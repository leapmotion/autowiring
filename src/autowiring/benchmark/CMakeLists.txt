set(AutoBench_SRCS
  AutoBench.cpp
  Benchmark.h
  Benchmark.cpp
  ContextSearchBm.h
  ContextSearchBm.cpp
  ContextTrackingBm.h
  ContextTrackingBm.cpp
  DispatchQueueBm.h
  DispatchQueueBm.cpp
  Foo.h
  ObjectPoolBm.h
  ObjectPoolBm.cpp
  PriorityBoost.h
  PriorityBoost.cpp
  PrintableDuration.h
)

add_pch(AutoBench_SRCS "stdafx.h" "stdafx.cpp")
add_executable(AutoBench ${AutoBench_SRCS})
target_link_libraries(AutoBench Autowiring AutowiringFixture AutoTesting)

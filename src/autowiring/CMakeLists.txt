#
# Setup testing
#

add_subdirectory(test)
add_subdirectory(benchmark)

#
# Configure source files
#

set(Autowiring_SRCS
  AnySharedPointer.cpp
  AnySharedPointer.h
  atomic_list.h
  atomic_list.cpp
  atomic_object.h
  at_exit.h
  altitude.h
  auto_arg.h
  auto_arg.cpp
  auto_id.h
  auto_id.cpp
  auto_in.h
  auto_future.h
  auto_out.h
  auto_prev.h
  auto_signal.h
  auto_signal.cpp
  auto_tuple.h
  AutoCurrentPacketPusher.h
  AutoFilterDescriptor.h
  AutoFilterDescriptor.cpp
  AutoFilterArgument.h
  AutoFuture.cpp
  AutoFuture.h
  AutoPacket.cpp
  AutoPacket.h
  AutoPacketInternal.cpp
  AutoPacketInternal.hpp
  AutoPacketFactory.cpp
  AutoPacketFactory.h
  AutoPacketGraph.cpp
  AutoPacketGraph.h
  AutoPacketProfiler.cpp
  AutoPacketProfiler.h
  AutoSelfUpdate.h
  AutoTimeStamp.h
  AutowirableSlot.cpp
  AutowirableSlot.h
  Autowired.cpp
  Autowired.h
  autowiring.h
  AutowiringConfig.h
  AutowiringDebug.h
  AutowiringDebug.cpp
  AutowiringEvents.cpp
  AutowiringEvents.h
  autowiring_error.cpp
  autowiring_error.h
  BasicThread.cpp
  BasicThread.h
  BasicThreadStateBlock.cpp
  BasicThreadStateBlock.h
  Bolt.h
  BoltBase.cpp
  BoltBase.h
  callable.h
  CallExtractor.h
  CallExtractor.cpp
  ContextEnumerator.cpp
  ContextEnumerator.h
  ContextMap.h
  ContextMember.cpp
  ContextMember.h
  CoreContext.cpp
  CoreContext.h
  CoreContextStateBlock.cpp
  CoreContextStateBlock.h
  CoreJob.cpp
  CoreJob.h
  CoreObject.cpp
  CoreObject.h
  CoreObjectDescriptor.h
  CoreRunnable.cpp
  CoreRunnable.h
  CoreThread.cpp
  CoreThread.h
  CreationRules.h
  CurrentContextPusher.cpp
  CurrentContextPusher.h
  Decompose.h
  DecorationDisposition.h
  Deferred.h
  demangle.cpp
  demangle.h
  Deserialize.h
  dispatch_aborted_exception.h
  dispatch_aborted_exception.cpp
  DispatchQueue.cpp
  DispatchQueue.h
  DispatchThunk.h
  EventRegistry.cpp
  EventRegistry.h
  ExceptionFilter.cpp
  ExceptionFilter.h
  fast_pointer_cast.h
  GlobalCoreContext.cpp
  GlobalCoreContext.h
  hash_tuple.h
  has_autofilter.h
  has_autoinit.h
  has_simple_constructor.h
  has_static_new.h
  has_validate.h
  hash_tuple.h
  HeteroBlock.h
  HeteroBlock.cpp
  index_tuple.h
  InterlockedExchange.h
  InvokeRelay.h
  is_any.h
  is_shared_ptr.h
  JunctionBox.h
  JunctionBoxBase.cpp
  JunctionBoxBase.h
  JunctionBoxEntry.h
  JunctionBoxManager.cpp
  JunctionBoxManager.h
  ManualThreadPool.h
  ManualThreadPool.cpp
  member_new_type.h
  MemoEntry.h
  MemoEntry.cpp
  MicroAutoFilter.h
  MicroBolt.h
  noop.h
  NullPool.h
  NullPool.cpp
  ObjectPool.h
  ObjectPoolMonitor.cpp
  ObjectPoolMonitor.h
  once.h
  once.cpp
  Parallel.h
  Parallel.cpp
  registration.h
  SatCounter.h
  SatCounter.cpp
  signal_base.h
  SlotInformation.cpp
  SlotInformation.h
  spin_lock.h
  sum.h
  SystemThreadPool.cpp
  SystemThreadPool.h
  SystemThreadPoolStl.cpp
  SystemThreadPoolStl.h
  TeardownNotifier.cpp
  TeardownNotifier.h
  thread_specific_ptr.h
  ThreadPool.h
  ThreadPool.cpp
  TypeIdentifier.h
  TypeRegistry.cpp
  TypeRegistry.h
  TypeUnifier.h
  unlock_object.h
  uuid.h
  var_logic.h
)

add_conditional_sources(
  Autowiring_SRCS
  "ON"
  GROUP_NAME "Autoboost"
  FILES
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/system/src/error_code.cpp
)

add_conditional_sources(
  Autowiring_SRCS
  "NOT MSVC"
  GROUP_NAME "Autoboost"
  FILES
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/chrono/src/process_cpu_clocks.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/chrono/src/thread_clock.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/chrono/src/chrono.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/thread/src/pthread/once_atomic.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/thread/src/pthread/thread.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/thread/src/future.cpp
)

add_windows_sources(Autowiring_SRCS
  auto_future_win.h
  CoreThreadWin.cpp
  CreationRulesWin.cpp
  SystemThreadPoolWin.cpp
  SystemThreadPoolWin.hpp
  SystemThreadPoolWinXP.cpp
  SystemThreadPoolWinXP.hpp
  SystemThreadPoolWinLH.cpp
  SystemThreadPoolWinLH.hpp
  InterlockedExchangeWin.cpp
  thread_specific_ptr_win.h
)

add_mac_sources(Autowiring_SRCS
  auto_future_mac.h
  CoreThreadMac.cpp
)

add_unix_sources(Autowiring_SRCS
  CreationRulesUnix.cpp
  InterlockedExchangeUnix.cpp
  thread_specific_ptr_unix.h
)

set(Autowiring_Linux_SRCS
  CoreThreadLinux.cpp
)

add_conditional_sources(Autowiring_SRCS "NOT MSVC" GROUP_NAME "Non-Windows Source" FILES ${Autowiring_Unix_SRCS})

add_conditional_sources(Autowiring_SRCS "WIN32 AND NOT autowiring_BUILD_64"
  GROUP_NAME "ASMx86 Source"
  FILES interlocked.asm
)

if(WIN32 AND NOT BUILD_64_BIT)
  enable_language(ASM_MASM)
endif()

add_conditional_sources(Autowiring_SRCS "NOT WIN32 AND NOT APPLE"
  GROUP_NAME "Linux Source"
  FILES CoreThreadLinux.cpp
)

rewrite_header_paths(Autowiring_SRCS)
add_pch(Autowiring_SRCS "stdafx.h" "stdafx.cpp")

#
# Configure library
#

add_library(Autowiring STATIC ${Autowiring_SRCS})

target_include_directories(Autowiring
  PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/contrib/autoboost>"
  INTERFACE
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:include>"
)

# Need multithreading services if available
find_package(Threads)
if(Threads_FOUND)
  if(CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(Autowiring ${CMAKE_THREAD_LIBS_INIT})
  endif()
endif()

#
# Install library
#

if(NOT AUTOWIRING_IS_EMBEDDED)
  install(TARGETS Autowiring EXPORT AutowiringTargets
    DESTINATION lib
    COMPONENT autowiring
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
  )
endif()

#
# Setup testing
#

add_googletest(test)
add_googletest(benchmark)

#
# Configure source files
#

set(Autowiring_SRCS
  AnySharedPointer.cpp
  AnySharedPointer.h
  atomic_object.h
  at_exit.h
  auto_id.h
  AutoCheckout.h
  AutoConfig.cpp
  AutoConfig.h
  AutoConfigManager.cpp
  AutoConfigManager.h
  AutoConfigParser.cpp
  AutoConfigParser.hpp
  AutoFilterDescriptor.h
  AutoFuture.cpp
  AutoFuture.h
  AutoInjectable.cpp
  AutoInjectable.h
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
  AutoParameter.h
  AutoRestarter.h
  AutoSelfUpdate.h
  AutoTimeStamp.h
  AutowirableSlot.cpp
  AutowirableSlot.h
  Autowired.cpp
  Autowired.h
  autowiring.h
  AutowiringConfig.h
  AutowiringConfig.h
  AutowiringEvents.cpp
  AutowiringEvents.h
  autowiring_error.cpp
  autowiring_error.h
  auto_arg.h
  auto_in.h
  auto_out.h
  BasicThread.cpp
  BasicThread.h
  BasicThreadStateBlock.cpp
  BasicThreadStateBlock.h
  Bolt.h
  BoltBase.cpp
  BoltBase.h
  CallExtractor.h
  ConfigRegistry.cpp
  ConfigRegistry.h
  ContextCreator.h
  ContextCreatorBase.cpp
  ContextCreatorBase.h
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
  CoreRunnable.cpp
  CoreRunnable.h
  CoreThread.cpp
  CoreThread.h
  CreationRules.h
  CurrentContextPusher.cpp
  CurrentContextPusher.h
  DeclareAutoFilter.h
  Decompose.h
  DecorationDisposition.h
  Deferred.h
  demangle.cpp
  demangle.h
  Deserialize.h
  DispatchQueue.cpp
  DispatchQueue.h
  DispatchThunk.h
  EventRegistry.cpp
  EventRegistry.h
  ExceptionFilter.cpp
  ExceptionFilter.h
  expect.hpp
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
  member_new_type.h
  MicroAutoFilter.h
  MicroBolt.h
  NewAutoFilter.h
  Object.cpp
  Object.h
  ObjectPool.h
  ObjectPoolMonitor.cpp
  ObjectPoolMonitor.h
  ObjectTraits.h
  SatCounter.h
  SatCounter.cpp
  SharedPointerSlot.h
  SlotInformation.cpp
  SlotInformation.h
  TeardownNotifier.cpp
  TeardownNotifier.h
  thread_specific_ptr.h
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

if(NOT AUTOWIRING_BUILD_AUTONET)
  list(APPEND Autowiring_SRCS DefaultAutoNetServer.cpp)
endif()

if(NOT APPLE)
  # avoid warning: has no symbols
  set(Autowiring_SRCS
    ${Autowiring_SRCS}
    uuid.cpp
  )
endif()

add_windows_sources(Autowiring_SRCS
  CoreThreadWin.cpp
  InterlockedExchangeWin.cpp
  thread_specific_ptr_win.h
)

add_mac_sources(Autowiring_SRCS
  CoreThreadMac.cpp
)

add_unix_sources(Autowiring_SRCS
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
set_property(TARGET Autowiring PROPERTY FOLDER "Autowiring")

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

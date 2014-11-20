#
# Setup testing
#

add_googletest(test)
add_googletest(benchmark)

#
# Configure source files
#

set(Autowiring_SRCS
  at_exit.h
  auto_in.h
  auto_out.h
  auto_arg.h
  auto_arg.cpp
  AnySharedPointer.h
  AnySharedPointer.cpp
  AutoCheckout.h
  AutoRestarter.h
  AutoFuture.h
  AutoFuture.cpp
  AutowiringConfig.h
  CoreJob.h
  CoreJob.cpp
  AutoFilterDescriptor.h
  AutoInjectable.h
  AutoInjectable.cpp
  AutoMerge.h
  AutoPacket.h
  AutoPacket.cpp
  AutoPacketFactory.h
  AutoPacketFactory.cpp
  AutoPacketProfiler.h
  AutoPacketProfiler.cpp
  AutoSelfUpdate.h
  AutoStile.h
  AutoTimeStamp.h
  Autowired.h
  Autowired.cpp
  AutowirableSlot.h
  AutowirableSlot.cpp
  AutowiringConfig.h
  AutowiringEvents.h
  AutowiringEvents.cpp
  autowiring.h
  autowiring_error.h
  autowiring_error.cpp
  BasicThread.h
  BasicThread.cpp
  BasicThreadStateBlock.h
  BasicThreadStateBlock.cpp
  Bolt.h
  BoltBase.h
  BoltBase.cpp
  CallExtractor.h
  ContextCreator.h
  ContextCreatorBase.h
  ContextCreatorBase.cpp
  ContextEnumerator.h
  ContextEnumerator.cpp
  ContextMap.h
  ContextMember.cpp
  ContextMember.h
  CoreContext.cpp
  CoreContext.h
  CoreContextStateBlock.h
  CoreContextStateBlock.cpp
  CoreThread.cpp
  CoreThread.h
  CoreRunnable.h
  CoreRunnable.cpp
  CreationRules.h
  CurrentContextPusher.cpp
  CurrentContextPusher.h
  DataFlow.h
  DeclareAutoFilter.h
  DeclareElseFilter.h
  Decompose.h
  DecorationDisposition.h
  Deserialize.h
  Deferred.h
  demangle.h
  demangle.cpp
  DispatchQueue.h
  DispatchQueue.cpp
  DispatchThunk.h
  EventInputStream.h
  EventOutputStream.h
  EventOutputStream.cpp
  EventRegistry.h
  EventRegistry.cpp
  fast_pointer_cast.h
  JunctionBox.h
  JunctionBoxBase.h
  JunctionBoxBase.cpp
  JunctionBoxEntry.h
  JunctionBoxManager.h
  JunctionBoxManager.cpp
  ExceptionFilter.h
  ExceptionFilter.cpp
  GlobalCoreContext.cpp
  GlobalCoreContext.h
  has_autoinit.h
  has_autofilter.h
  has_simple_constructor.h
  has_static_new.h
  hash_tuple.h
  index_tuple.h
  is_any.h
  is_shared_ptr.h
  InterlockedExchange.h
  InvokeRelay.h
  atomic_object.h
  member_new_type.h
  Object.h
  Object.cpp
  ObjectPool.h
  ObjectPoolMonitor.h
  ObjectPoolMonitor.cpp
  ObjectTraits.h
  optional_ptr.h
  SatCounter.h
  MicroAutoFilter.h
  MicroBolt.h
  NewAutoFilter.h
  SharedPointerSlot.h
  SlotInformation.h
  SlotInformation.cpp
  TeardownNotifier.h
  TeardownNotifier.cpp
  ThreadStatusBlock.h
  ThreadStatusBlock.cpp
  thread_specific_ptr.h
  TypeIdentifier.h
  TypeRegistry.h
  TypeRegistry.cpp
  TypeUnifier.h
  unlock_object.h
  uuid.h
  var_logic.h
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

set(Autowiring_Win_SRCS
  CoreThreadWin.cpp
  InterlockedExchangeWin.cpp
  thread_specific_ptr_win.h
)

set(Autowiring_Unix_SRCS
  InterlockedExchangeUnix.cpp
  thread_specific_ptr_unix.h
)

set(Autowiring_Mac_SRCS
  CoreThreadMac.cpp
)

set(Autowiring_Linux_SRCS
  CoreThreadLinux.cpp
)

ADD_MSVC_DISABLED_FILES("Unix Source" Autowiring_SRCS ${Autowiring_Unix_SRCS})

if(WIN32)
 set(Autowiring_SRCS ${Autowiring_Win_SRCS} ${Autowiring_SRCS})

  if(!BUILD_64_BIT)
    enable_language(ASM_MASM)
    set(Autowiring_SRCS interlocked.asm ${Autowiring_SRCS} )
  endif()
else()
  if(APPLE)
    set(Autowiring_SRCS ${Autowiring_Mac_SRCS} ${Autowiring_SRCS})
  else()
    set(Autowiring_SRCS ${Autowiring_Linux_SRCS} ${Autowiring_SRCS})
  endif()

  set(Autowiring_SRCS ${Autowiring_Unix_SRCS} ${Autowiring_SRCS})
endif()

rewrite_header_paths(Autowiring_SRCS)
add_pch(Autowiring_SRCS "stdafx.h" "stdafx.cpp")

#
# Configure library
#

add_library(Autowiring STATIC ${Autowiring_SRCS})

target_include_directories(Autowiring INTERFACE
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)
set_property(TARGET Autowiring PROPERTY FOLDER "Autowiring")

# The boost C++11 shim is required when building without autowiring_USE_LIBCXX off Windows
# We do not mention the link libraries; rather, we require that downstream clients
# satisfy this link dependency.
if(NOT WIN32 AND NOT autowiring_USE_LIBCXX)
  find_package(Boost COMPONENTS thread atomic chrono system date_time QUIET)
endif()

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

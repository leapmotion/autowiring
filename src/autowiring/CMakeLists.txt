add_googletest(test)
add_googletest(benchmark)

set(Autowiring_SRCS
  at_exit.h
  auto_out.h
  AnySharedPointer.h
  AutoAnchor.h
  AutoCheckout.h
  AutoRestarter.h
  AutoFuture.h
  AutoFuture.cpp
  CoreJob.h
  CoreJob.cpp
  AutoFilterDescriptor.h
  AutoInjectable.h
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
  AutowiringEvents.h
  autowiring.h
  autowiring_error.h
  BasicThread.h
  BasicThread.cpp
  BasicThreadStateBlock.h
  BasicThreadStateBlock.cpp
  Bolt.h
  BoltBase.h
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
  DefaultAutoNetServer.cpp
  demangle.h
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
  is_autofilter.h
  InterlockedExchange.h
  InvokeRelay.h
  atomic_object.h
  unlock_object.h
  Object.h
  ObjectPool.h
  ObjectPoolMonitor.h
  ObjectPoolMonitor.cpp
  optional_ptr.h
  SatCounter.h
  MicroAutoFilter.h
  MicroBolt.h
  NewAutoFilter.h
  NewAutoFilter.cpp
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
  uuid.h
)

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
ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" Autowiring_SRCS)

add_library(Autowiring STATIC ${Autowiring_SRCS})

if(CMAKE_MAJOR_VERSION GREATER 2)
  target_include_directories(Autowiring INTERFACE "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>" "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>")
endif()
set_property(TARGET Autowiring PROPERTY FOLDER "Autowiring")

# Might as well reference the boost libraries to satisfy link dependencies, if we know they exist
find_package(Boost COMPONENTS thread atomic chrono system date_time QUIET)
if(Boost_FOUND)
  target_link_libraries(Autowiring ${Boost_LIBRARIES})
endif()

# Need multithreading services if available
find_package(Threads)
if(Threads_FOUND)
  if(CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(Autowiring ${CMAKE_THREAD_LIBS_INIT})
  endif()
endif()

if(NOT AUTOWIRING_IS_EMBEDDED)
  install(TARGETS Autowiring EXPORT AutowiringTargets
    DESTINATION lib
    COMPONENT autowiring 
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
  )

  foreach (src ${Autowiring_SRCS})
    string(REGEX MATCH ".*\\.h" hfile ${src})
    if(hfile)
      install(
        FILES ${hfile}
        DESTINATION "include/Autowiring"
        COMPONENT autowiring
      )
    endif()
  endforeach()
endif()
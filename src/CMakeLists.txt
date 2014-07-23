include_directories(
    ..
  )

add_subdirectory(AutoNet)

set(Autowiring_SRCS
  AnySharedPointer.h
  at_exit.h
  AnySharedPointer.h
  AutoAnchor.h
  AutoCheckout.h
  NewAutoFilter.h
  NewAutoFilter.cpp
  AutoFuture.h
  AutoFuture.cpp
  CoreJob.h
  CoreJob.cpp
  AutoFilterDescriptor.h
  AutoInjectable.h
  AutoPacket.h
  AutoPacket.cpp
  AutoPacketFactory.h
  AutoPacketFactory.cpp
  AutoPacketProfiler.h
  AutoPacketProfiler.cpp
  AutoSelfUpdate.h
  Autowired.h
  Autowired.cpp
  AutowirableSlot.h
  AutowirableSlot.cpp
  AutowiringEvents.h
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
  Decompose.h
  DecorationDisposition.h
  Deserialize.h
  deferred_ptr.h
  DispatchQueue.h
  DispatchQueue.cpp
  DispatchThunk.h
  EventInputStream.h
  EventOutputStream.h
  EventOutputStream.cpp
  EventReceiver.h
  EventReceiver.cpp
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
  has_autofilter.h
  has_simple_constructor.h
  has_static_new.h
  is_any.h
  is_shared_ptr.h
  InterlockedExchange.h
  InvokeRelay.h
  atomic_object.h
  shared_object.h
  unlock_object.h
  Jzon.h
  Jzon.cpp
  Object.h
  ObjectPool.h
  ObjectPoolMonitor.h
  ObjectPoolMonitor.cpp
  optional_ptr.h
  SatCounter.h
  move_only.h
  MicroBolt.h
  SharedPointerSlot.h
  SlotInformation.h
  SlotInformation.cpp
  TeardownNotifier.h
  TeardownNotifier.cpp
  ThreadStatusBlock.h
  ThreadStatusBlock.cpp
  thread_specific_ptr.h
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
 set(Autowiring_SRCS
	  ${Autowiring_Win_SRCS}
    ${Autowiring_SRCS}
  )

  if(!BUILD_64_BIT)
    enable_language(ASM_MASM)
    set(Autowiring_SRCS interlocked.asm ${Autowiring_SRCS} )
  endif()
else()
  if(APPLE)
    set(Autowiring_SRCS
      ${Autowiring_Mac_SRCS}
      ${Autowiring_SRCS}
    )
  else()
    set(Autowiring_SRCS
      ${Autowiring_Linux_SRCS}
      ${Autowiring_SRCS}
    )
  endif()

  set(Autowiring_SRCS
    ${Autowiring_Unix_SRCS}
    ${Autowiring_SRCS}
  )
endif()

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" Autowiring_SRCS)
add_library(Autowiring STATIC ${Autowiring_SRCS})
set_property(TARGET Autowiring PROPERTY FOLDER "Autowiring")
install(TARGETS Autowiring DESTINATION lib)

if(NOT WIN32)
  set(Boost_THREAD_LIBRARY libboost_thread)
  add_library(${Boost_THREAD_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${Boost_THREAD_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_THREAD_LIBRARY}${STATIC_LIBRARY_SUFFIX})

  set(Boost_CHRONO_LIBRARY libboost_chrono)
  add_library(${Boost_CHRONO_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${Boost_CHRONO_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_CHRONO_LIBRARY}${STATIC_LIBRARY_SUFFIX})

  set(Boost_REGEX_LIBRARY libboost_regex)
  add_library(${Boost_REGEX_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${Boost_REGEX_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_REGEX_LIBRARY}${STATIC_LIBRARY_SUFFIX})
  
  set(Boost_DATE_TIME_LIBRARY libboost_date_time)
  add_library(${Boost_DATE_TIME_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${Boost_DATE_TIME_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_DATE_TIME_LIBRARY}${STATIC_LIBRARY_SUFFIX})

  set(Boost_SYSTEM_LIBRARY libboost_system)
  add_library(${Boost_SYSTEM_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${Boost_SYSTEM_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_SYSTEM_LIBRARY}${STATIC_LIBRARY_SUFFIX})

  if(Boost_VERSION GREATER 105200)
    set(Boost_ATOMIC_LIBRARY libboost_atomic)
    add_library(${Boost_ATOMIC_LIBRARY} STATIC IMPORTED)
    set_property(TARGET ${Boost_ATOMIC_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_ATOMIC_LIBRARY}${STATIC_LIBRARY_SUFFIX})
  endif()

  if(NOT APPLE AND NOT BUILD_ANDROID)
    # Linux non-Android only
    target_link_libraries(Autowiring -lrt)
  endif()
  if(NOT BUILD_ANDROID)
    target_link_libraries(Autowiring -lpthread)
  endif()
  target_link_libraries(Autowiring ${Boost_THREAD_LIBRARY} ${Boost_CHRONO_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  if(Boost_VERSION GREATER 105200)
    target_link_libraries(Autowiring ${Boost_ATOMIC_LIBRARY})
  endif()
endif()

foreach (src ${Autowiring_SRCS})
  string(REGEX MATCH ".*\\.h" hfile ${src})
  if(hfile)
    install(
      FILES ${hfile}
      DESTINATION "include/Autowiring"
      COMPONENT dev
    )
  endif()
endforeach()

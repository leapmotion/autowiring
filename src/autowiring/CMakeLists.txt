#
# Setup testing
#

add_subdirectory(test)

#
# Configure source files
#

set(Autowiring_SRCS
  AnySharedPointer.cpp
  AnySharedPointer.h
  atomic_list.h
  atomic_list.cpp
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
  AutowirableSlot.cpp
  AutowirableSlot.h
  Autowired.cpp
  Autowired.h
  autowiring.h
  autowiring_error.cpp
  autowiring_error.h
  AutowiringConfig.h
  AutowiringDebug.h
  AutowiringDebug.cpp
  AutowiringVersion.h
  basic_timer.h
  BasicThread.cpp
  BasicThread.h
  BasicThreadStateBlock.cpp
  BasicThreadStateBlock.h
  Bolt.h
  BoltBase.cpp
  BoltBase.h
  bounds.h
  callable.h
  CallExtractor.h
  CallExtractor.cpp
  chrono_types.h
  config.h
  config_descriptor.h
  config_descriptor.cpp
  config_event.h
  ConfigBolt.h
  ConfigBolt.cpp
  ConfigManager.h
  ConfigManager.cpp
  ConfigRegistry.h
  ConfigRegistry.cpp
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
  deref_error.h
  deref_error.cpp
  dereferencer.h
  Deserialize.h
  dispatch_aborted_exception.h
  dispatch_aborted_exception.cpp
  DispatchQueue.cpp
  DispatchQueue.h
  DispatchThunk.h
  ExceptionFilter.cpp
  ExceptionFilter.h
  fast_pointer_cast.h
  GlobalCoreContext.cpp
  GlobalCoreContext.h
  hash_tuple.h
  has_autofilter.h
  has_autoinit.h
  has_getconfigdescriptor.h
  has_simple_constructor.h
  has_static_new.h
  has_validate.h
  hash_tuple.h
  HeteroBlock.h
  HeteroBlock.cpp
  index_tuple.h
  is_any.h
  is_shared_ptr.h
  ManualThreadPool.h
  ManualThreadPool.cpp
  marshaller.h
  member_new_type.h
  MemoEntry.h
  MemoEntry.cpp
  MicroBolt.h
  noop.h
  NullPool.h
  NullPool.cpp
  ObjectPool.h
  ObjectPoolMonitor.cpp
  ObjectPoolMonitor.h
  observable.h
  once.h
  once.cpp
  optional.h
  Parallel.h
  Parallel.cpp
  registration.h
  SatCounter.h
  signal.h
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
  uuid.h
  var_logic.h
  vector_from_tuple.h
)

add_conditional_sources(
  Autowiring_SRCS
  "TRUE"
  GROUP_NAME "C++11"
  FILES
  C++11/boost_array.h
  C++11/boost_exception_ptr.h
  C++11/boost_functional.h
  C++11/boost_future.h
  C++11/boost_shared_ptr.h
  C++11/boost_type_traits.h
  C++11/chrono_with_profiling_clock.h
  C++11/cpp11.h
  C++11/filesystem.h
  C++11/make_unique.h
  C++11/memory.h
  C++11/memory_nostl11.h
  C++11/mutex.h
  C++11/unique_ptr.h
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
  thread_specific_ptr_win.cpp
)

add_mac_sources(Autowiring_SRCS
  auto_future_mac.h
  CoreThreadMac.cpp
)

add_unix_sources(Autowiring_SRCS
  CreationRulesUnix.cpp
  thread_specific_ptr_unix.cpp
)

set(Autowiring_Linux_SRCS
  CoreThreadLinux.cpp
)

add_conditional_sources(Autowiring_SRCS "NOT MSVC" GROUP_NAME "Non-Windows Source" FILES ${Autowiring_Unix_SRCS})

add_conditional_sources(
  Autowiring_SRCS
  "NOT WIN32 AND NOT APPLE"
  GROUP_NAME "Linux Source"
  FILES CoreThreadLinux.cpp
)

#
# Configure library
#

add_library(Autowiring STATIC ${Autowiring_SRCS})
add_pch(Autowiring "stdafx.h" "stdafx.cpp")
target_link_libraries(Autowiring INTERFACE Autoboost)
parse_version(autowiring ${autowiring_VERSION})
configure_file(AutowiringVersion.h.in AutowiringVersion.h)

target_include_directories(
  Autowiring
  PRIVATE
  "${PROJECT_SOURCE_DIR}/contrib/autoboost"
  PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
  "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>"
  INTERFACE
  "$<INSTALL_INTERFACE:include>"
)

# Need multithreading services if available
find_package(Threads)
if(Threads_FOUND)
  if(CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(Autowiring INTERFACE ${CMAKE_THREAD_LIBS_INIT})
  endif()
endif()

#
# Install library
#
install(TARGETS Autowiring EXPORT AutowiringTargets
  DESTINATION lib
  COMPONENT autowiring
  CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
)
install_headers(TARGET Autowiring DESTINATION include/autowiring COMPONENT autowiring)

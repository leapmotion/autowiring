include_directories(
    ..
  )

set(
  Autowiring_SRCS
  Autowired.cpp
  Autowired.h
  Autowirer.cpp
  Autowirer.h
  CMakeLists.txt
  ContextMember.cpp
  ContextMember.h
  CoreContext.cpp
  CoreContext.h
  CoreThread.cpp
  CoreThread.h
  GlobalCoreContext.cpp
  GlobalCoreContext.h
  Object.h
  SharedPtrWrap.cpp
  SharedPtrWrap.h
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" Autowiring_SRCS)
add_library(Autowiring STATIC ${Autowiring_SRCS})

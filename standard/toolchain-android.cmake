set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

# Deal with weirdnesses caused by our standard_project setup. See
# notes in StandardProject.cmake
if(NOT LLVM_ANDROID_TOOLCHAIN_DIR)
  set(LLVM_ANDROID_TOOLCHAIN_DIR $ENV{LLVM_ANDROID_TOOLCHAIN_DIR})
else()
  set(ENV{LLVM_ANDROID_TOOLCHAIN_DIR} ${LLVM_ANDROID_TOOLCHAIN_DIR} CACHE PATH)
endif()

find_path(ANDROID_NDK_ROOT bin/${ANDROID_NDK_TOOL_PREFIX}-gcc${_exe_suffix} PATHS
  ${LLVM_ANDROID_TOOLCHAIN_DIR}${_ndk_suffix} # legacy variable
  /opt/android-standalone-toolchain${_ndk_suffix}
  ${LLVM_ANDROID_TOOLCHAIN_DIR}
  /opt/android-standalone-toolchain
)
if(NOT ANDROID_NDK_ROOT)
  message(FATAL_ERROR "ANDROID_NDK_ROOT could not be located. Have you installed the NDK?")
endif()

set(ANDROID_COMMON_FLAGS " --sysroot=${ANDROID_NDK_ROOT}/sysroot -B${ANDROID_NDK_ROOT} ")
set(CMAKE_C_COMPILER ${ANDROID_NDK_ROOT}/bin/${ANDROID_NDK_TOOL_PREFIX}-gcc${_exe_suffix} CACHE FILEPATH "C Compiler" FORCE)
set(CMAKE_CXX_COMPILER ${ANDROID_NDK_ROOT}/bin/${ANDROID_NDK_TOOL_PREFIX}-g++${_exe_suffix} CACHE FILEPATH "C++ Compiler" FORCE)
set(ANDROID_STRIP_COMMAND ${ANDROID_NDK_ROOT}/bin/${ANDROID_NDK_TOOL_PREFIX}-strip)
set(CMAKE_C_FLAGS "${ANDROID_COMMON_FLAGS}" CACHE STRING "toolchain_cflags" FORCE)
set(CMAKE_CXX_FLAGS "${ANDROID_COMMON_FLAGS}" CACHE STRING "toolchain_cxxflags" FORCE)
set(CMAKE_LINK_FLAGS "${ANDROID_COMMON_FLAGS}" CACHE STRING "toolchain_linkflags" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

if(NOT DEFINED CMAKE_ANDROID_ARCH_ABI)
  set(CMAKE_ANDROID_ARCH_ABI armeabi)
endif()
include(${CMAKE_CURRENT_LIST_DIR}/toolchain-android.cmake)

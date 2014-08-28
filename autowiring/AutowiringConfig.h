#pragma once
//
// Defines preprocessor macros from CMake variables
//

// Are we building autonet?
#define AUTOWIRING_BUILD_AUTONET 1

// Are we linking with with C++11 STL?
#define USE_LIBCXX 1
#if USE_LIBCXX
#define AUTOWIRING_USE_LIBCXX 1
#else
#define AUTOWIRING_USE_LIBCXX 0
#endif

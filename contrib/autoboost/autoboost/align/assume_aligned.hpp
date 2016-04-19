/*
(c) 2015 NumScale SAS
(c) 2015 LRI UMR 8623 CNRS/University Paris Sud XI

(c) 2015 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_ASSUME_ALIGNED_HPP
#define AUTOBOOST_ALIGN_ASSUME_ALIGNED_HPP

#include <autoboost/config.hpp>

#if defined(AUTOBOOST_MSVC)
#include <autoboost/align/detail/assume_aligned_msvc.hpp>
#elif defined(AUTOBOOST_CLANG) && defined(__has_builtin)
#include <autoboost/align/detail/assume_aligned_clang.hpp>
#elif AUTOBOOST_GCC_VERSION >= 40700
#include <autoboost/align/detail/assume_aligned_gcc.hpp>
#elif defined(__INTEL_COMPILER)
#include <autoboost/align/detail/assume_aligned_intel.hpp>
#else
#include <autoboost/align/detail/assume_aligned.hpp>
#endif

#endif

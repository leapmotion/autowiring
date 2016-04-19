/*
(c) 2015 NumScale SAS
(c) 2015 LRI UMR 8623 CNRS/University Paris Sud XI

(c) 2015 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_ASSUME_ALIGNED_MSVC_HPP
#define AUTOBOOST_ALIGN_DETAIL_ASSUME_ALIGNED_MSVC_HPP

#include <stddef.h>

#define AUTOBOOST_ALIGN_ASSUME_ALIGNED(ptr, alignment) \
__assume((uintptr_t(ptr) & ((alignment) - 1)) == 0)

#endif

/*
Copyright 2015 NumScale SAS
Copyright 2015 LRI UMR 8623 CNRS/University Paris Sud XI

Copyright 2015 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_ASSUME_ALIGNED_MSVC_HPP
#define AUTOBOOST_ALIGN_DETAIL_ASSUME_ALIGNED_MSVC_HPP

#include <cstddef>

#define AUTOBOOST_ALIGN_ASSUME_ALIGNED(ptr, alignment) \
__assume((reinterpret_cast<std::size_t>(ptr) & ((alignment) - 1)) == 0)

#endif

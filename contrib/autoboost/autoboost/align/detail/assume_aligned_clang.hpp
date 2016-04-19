/*
(c) 2015 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_ASSUME_ALIGNED_CLANG_HPP
#define AUTOBOOST_ALIGN_DETAIL_ASSUME_ALIGNED_CLANG_HPP

#if __has_builtin(__builtin_assume_aligned)
#define AUTOBOOST_ALIGN_ASSUME_ALIGNED(ptr, alignment) \
(ptr) = __builtin_assume_aligned((ptr), (alignment))
#else
#define AUTOBOOST_ALIGN_ASSUME_ALIGNED(ptr, alignment)
#endif

#endif

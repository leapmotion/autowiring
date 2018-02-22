/*
Copyright 2014-2016 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_ALIGNMENT_OF_HPP
#define AUTOBOOST_ALIGN_ALIGNMENT_OF_HPP

#include <autoboost/align/detail/element_type.hpp>
#include <autoboost/align/alignment_of_forward.hpp>

#if defined(AUTOBOOST_MSVC)
#include <autoboost/align/detail/alignment_of_msvc.hpp>
#elif defined(__GNUC__) && defined(__unix__) && !defined(__LP64__)
#include <autoboost/align/detail/alignment_of.hpp>
#elif defined(AUTOBOOST_CLANG) && !defined(__x86_64__)
#include <autoboost/align/detail/alignment_of.hpp>
#elif !defined(AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS)
#include <autoboost/align/detail/alignment_of_cxx11.hpp>
#elif defined(__ghs__) && (__GHS_VERSION_NUMBER >= 600)
#include <autoboost/align/detail/alignment_of_gcc.hpp>
#elif defined(__CODEGEARC__)
#include <autoboost/align/detail/alignment_of_codegear.hpp>
#elif defined(AUTOBOOST_CLANG)
#include <autoboost/align/detail/alignment_of_clang.hpp>
#elif __GNUC__ > 4
#include <autoboost/align/detail/alignment_of_gcc.hpp>
#elif (__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)
#include <autoboost/align/detail/alignment_of_gcc.hpp>
#else
#include <autoboost/align/detail/alignment_of.hpp>
#endif

namespace autoboost {
namespace alignment {

template<class T>
struct alignment_of
    : detail::alignment_of<typename
        detail::element_type<T>::type>::type { };

#if !defined(AUTOBOOST_NO_CXX14_VARIABLE_TEMPLATES)
template<class T>
constexpr std::size_t alignment_of_v = alignment_of<T>::value;
#endif

} /* alignment */
} /* autoboost */

#endif

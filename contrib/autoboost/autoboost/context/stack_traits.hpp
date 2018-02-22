
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CONTEXT_STACK_TRAITS_H
#define AUTOBOOST_CONTEXT_STACK_TRAITS_H

#include <cstddef>

#include <autoboost/config.hpp>

#include <autoboost/context/detail/config.hpp>

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_PREFIX
#endif

namespace autoboost {
namespace context {

struct AUTOBOOST_CONTEXT_DECL stack_traits
{
    static bool is_unbounded() AUTOBOOST_NOEXCEPT_OR_NOTHROW;

    static std::size_t page_size() AUTOBOOST_NOEXCEPT_OR_NOTHROW;

    static std::size_t default_size() AUTOBOOST_NOEXCEPT_OR_NOTHROW;

    static std::size_t minimum_size() AUTOBOOST_NOEXCEPT_OR_NOTHROW;

    static std::size_t maximum_size() AUTOBOOST_NOEXCEPT_OR_NOTHROW;
};

}}

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_SUFFIX
#endif

#endif // AUTOBOOST_CONTEXT_STACK_TRAITS_H

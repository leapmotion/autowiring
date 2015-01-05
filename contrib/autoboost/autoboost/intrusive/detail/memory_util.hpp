//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Pablo Halpern 2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2011-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTRUSIVE_ALLOCATOR_MEMORY_UTIL_HPP
#define AUTOBOOST_INTRUSIVE_ALLOCATOR_MEMORY_UTIL_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/intrusive/detail/workaround.hpp>
#include <autoboost/intrusive/detail/mpl.hpp>
#include <autoboost/intrusive/detail/preprocessor.hpp>
#include <autoboost/preprocessor/iteration/iterate.hpp>

namespace autoboost {
namespace intrusive {
namespace detail {

template <typename T>
inline T* addressof(T& obj)
{
   return static_cast<T*>
      (static_cast<void*>
         (const_cast<char*>
            (&reinterpret_cast<const char&>(obj))
         )
      );
}

template <typename T>
struct LowPriorityConversion
{
    // Convertible from T with user-defined-conversion rank.
    LowPriorityConversion(const T&) { }
};

}}}   //namespace autoboost::intrusive::detail

#include <autoboost/intrusive/detail/has_member_function_callable_with.hpp>

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME pointer_to
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN namespace autoboost { namespace intrusive { namespace detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (1, 1, <autoboost/intrusive/detail/has_member_function_callable_with.hpp>))
#include AUTOBOOST_PP_ITERATE()

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME static_cast_from
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN namespace autoboost { namespace intrusive { namespace detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (1, 1, <autoboost/intrusive/detail/has_member_function_callable_with.hpp>))
#include AUTOBOOST_PP_ITERATE()

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME const_cast_from
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN namespace autoboost { namespace intrusive { namespace detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (1, 1, <autoboost/intrusive/detail/has_member_function_callable_with.hpp>))
#include AUTOBOOST_PP_ITERATE()

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME dynamic_cast_from
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN namespace autoboost { namespace intrusive { namespace detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (1, 1, <autoboost/intrusive/detail/has_member_function_callable_with.hpp>))
#include AUTOBOOST_PP_ITERATE()

namespace autoboost {
namespace intrusive {
namespace detail {

AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(element_type)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(difference_type)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(reference)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(value_traits_ptr)

}  //namespace detail {
}  //namespace intrusive {
}  //namespace autoboost {

#endif // ! defined(AUTOBOOST_INTRUSIVE_ALLOCATOR_MEMORY_UTIL_HPP)

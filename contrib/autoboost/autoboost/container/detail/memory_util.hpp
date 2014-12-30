//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2011-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_ALLOCATOR_MEMORY_UTIL_HPP
#define AUTOBOOST_CONTAINER_ALLOCATOR_MEMORY_UTIL_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>

#include <autoboost/container/detail/preprocessor.hpp>
#include <autoboost/intrusive/detail/mpl.hpp>
#include <autoboost/intrusive/detail/has_member_function_callable_with.hpp>


#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME allocate
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN namespace autoboost { namespace container { namespace container_detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (2, 2, <autoboost/intrusive/detail/has_member_function_callable_with.hpp>))
#include AUTOBOOST_PP_ITERATE()

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME destroy
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN namespace autoboost { namespace container { namespace container_detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (1, 1, <autoboost/intrusive/detail/has_member_function_callable_with.hpp>))
#include AUTOBOOST_PP_ITERATE()

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME max_size
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN namespace autoboost { namespace container { namespace container_detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (0, 0, <autoboost/intrusive/detail/has_member_function_callable_with.hpp>))
#include AUTOBOOST_PP_ITERATE()

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME select_on_container_copy_construction
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN namespace autoboost { namespace container { namespace container_detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (0, 0, <autoboost/intrusive/detail/has_member_function_callable_with.hpp>))
#include AUTOBOOST_PP_ITERATE()

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME construct
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN namespace autoboost { namespace container { namespace container_detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#ifdef AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_SINGLE_ITERATION
#  define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (1, 1, <autoboost/intrusive/detail/has_member_function_callable_with.hpp>))
#else
#  define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (1, AUTOBOOST_CONTAINER_MAX_CONSTRUCTOR_PARAMETERS+1, <autoboost/intrusive/detail/has_member_function_callable_with.hpp>))
#endif
#include AUTOBOOST_PP_ITERATE()

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME swap
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN namespace autoboost { namespace container { namespace container_detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_PP_ITERATION_PARAMS_1 (3, (1, 1, <autoboost/intrusive/detail/has_member_function_callable_with.hpp>))
#include AUTOBOOST_PP_ITERATE()

namespace autoboost {
namespace container {
namespace container_detail {

AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(pointer)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(const_pointer)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(reference)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(const_reference)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(void_pointer)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(const_void_pointer)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(size_type)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(propagate_on_container_copy_assignment)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(propagate_on_container_move_assignment)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(propagate_on_container_swap)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(difference_type)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(value_compare)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(wrapped_value_compare)

}  //namespace container_detail {
}  //namespace container {
}  //namespace autoboost {

#include <autoboost/container/detail/config_end.hpp>

#endif // ! defined(AUTOBOOST_CONTAINER_ALLOCATOR_MEMORY_UTIL_HPP)

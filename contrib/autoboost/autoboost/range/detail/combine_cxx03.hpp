// Boost.Range library
//
//  Copyright Neil Groves 2014. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_DETAIL_COMBINE_CXX03_HPP
#define AUTOBOOST_RANGE_DETAIL_COMBINE_CXX03_HPP

#ifndef AUTOBOOST_RANGE_MIN_COMBINE_ARGS
#define AUTOBOOST_RANGE_MIN_COMBINE_ARGS 2
#endif

#ifndef AUTOBOOST_RANGE_MAX_COMBINE_ARGS
#define AUTOBOOST_RANGE_MAX_COMBINE_ARGS 5
#endif

#include <autoboost/config.hpp>
#include <autoboost/iterator/zip_iterator.hpp>
#include <autoboost/preprocessor/arithmetic/dec.hpp>
#include <autoboost/preprocessor/arithmetic/div.hpp>
#include <autoboost/preprocessor/arithmetic/mul.hpp>
#include <autoboost/preprocessor/control.hpp>
#include <autoboost/preprocessor/control/while.hpp>
#include <autoboost/preprocessor/facilities/empty.hpp>
#include <autoboost/preprocessor/facilities/identity.hpp>
#include <autoboost/preprocessor/iteration/local.hpp>
#include <autoboost/preprocessor/punctuation/comma.hpp>
#include <autoboost/preprocessor/repetition.hpp>
#include <autoboost/preprocessor/tuple/elem.hpp>
#include <autoboost/range/iterator_range_core.hpp>
#include <autoboost/type_traits/remove_reference.hpp>
#include <autoboost/mpl/transform.hpp>
#include <autoboost/utility/result_of.hpp>

#include <vector>
#include <list>

namespace autoboost
{
    namespace range_detail
    {

template<typename F, typename T, int SIZE>
struct combined_result_impl;

template<typename F, typename T>
struct combined_result
    : combined_result_impl<F, T, tuples::length<T>::value>
{
};

#define AUTOBOOST_RANGE_combined_element(z, n, data) \
    typename tuples::element<n, T>::type

#define AUTOBOOST_RANGE_combined_result(z, n, data) \
    template<typename F, typename T> \
    struct combined_result_impl <F,T,n> \
        : result_of<F(AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_element, ~))> \
    { \
    };

#define AUTOBOOST_PP_LOCAL_MACRO(n) AUTOBOOST_RANGE_combined_result(~,n,~)

#define AUTOBOOST_PP_LOCAL_LIMITS (AUTOBOOST_RANGE_MIN_COMBINE_ARGS, \
                               AUTOBOOST_RANGE_MAX_COMBINE_ARGS)
#include AUTOBOOST_PP_LOCAL_ITERATE()

#define AUTOBOOST_RANGE_combined_get(z, n, data) get<n>(tuple)

#define AUTOBOOST_RANGE_combined_unpack(z, n, data) \
    template<typename F, typename T> inline \
    typename combined_result<F,T>::type \
    unpack_(mpl::int_<n>, F f, const T& tuple) \
    { \
        return f(AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_get, ~)); \
    }

#define AUTOBOOST_PP_LOCAL_MACRO(n) AUTOBOOST_RANGE_combined_unpack(~,n,~)
#define AUTOBOOST_PP_LOCAL_LIMITS (AUTOBOOST_RANGE_MIN_COMBINE_ARGS, \
                               AUTOBOOST_RANGE_MAX_COMBINE_ARGS)
#include AUTOBOOST_PP_LOCAL_ITERATE()

} // namespace range_detail

namespace range
{

#define AUTOBOOST_RANGE_combined_seq(z, n, data) autoboost::data(AUTOBOOST_PP_CAT(r,n))

#ifdef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#include <autoboost/range/detail/combine_no_rvalue.hpp>

#else // by using rvalue references we avoid requiring 2^n overloads.

#include <autoboost/range/detail/combine_rvalue.hpp>

#endif

#define AUTOBOOST_PP_LOCAL_MACRO(n) AUTOBOOST_RANGE_combine(~,n,~)
#define AUTOBOOST_PP_LOCAL_LIMITS (AUTOBOOST_RANGE_MIN_COMBINE_ARGS, \
                               AUTOBOOST_RANGE_MAX_COMBINE_ARGS)
#include AUTOBOOST_PP_LOCAL_ITERATE()

    } // namespace range

    using autoboost::range::combine;

} // namespace autoboost

#endif // include guard

#undef AUTOBOOST_RANGE_combined_element
#undef AUTOBOOST_RANGE_combined_result
#undef AUTOBOOST_RANGE_combined_get
#undef AUTOBOOST_RANGE_combined_unpack
#undef AUTOBOOST_RANGE_combined_seq
#undef AUTOBOOST_RANGE_combined_exp_pred
#undef AUTOBOOST_RANGE_combined_exp_op
#undef AUTOBOOST_RANGE_combined_exp
#undef AUTOBOOST_RANGE_combined_bitset_pred
#undef AUTOBOOST_RANGE_combined_bitset_op
#undef AUTOBOOST_RANGE_combined_bitset
#undef AUTOBOOST_RANGE_combined_range_iterator
#undef AUTOBOOST_RANGE_combined_args
#undef AUTOBOOST_RANGE_combine_impl
#undef AUTOBOOST_RANGE_combine

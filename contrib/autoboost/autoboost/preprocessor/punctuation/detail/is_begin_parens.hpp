# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2014.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
#ifndef AUTOBOOST_PREPROCESSOR_DETAIL_IS_BEGIN_PARENS_HPP
#define AUTOBOOST_PREPROCESSOR_DETAIL_IS_BEGIN_PARENS_HPP

#if AUTOBOOST_PP_VARIADICS_MSVC

#include <autoboost/preprocessor/facilities/empty.hpp>

#define AUTOBOOST_PP_DETAIL_VD_IBP_CAT(a, b) AUTOBOOST_PP_DETAIL_VD_IBP_CAT_I(a, b)
#define AUTOBOOST_PP_DETAIL_VD_IBP_CAT_I(a, b) AUTOBOOST_PP_DETAIL_VD_IBP_CAT_II(a ## b)
#define AUTOBOOST_PP_DETAIL_VD_IBP_CAT_II(res) res

#define AUTOBOOST_PP_DETAIL_IBP_SPLIT(i, ...) \
    AUTOBOOST_PP_DETAIL_VD_IBP_CAT(AUTOBOOST_PP_DETAIL_IBP_PRIMITIVE_CAT(AUTOBOOST_PP_DETAIL_IBP_SPLIT_,i)(__VA_ARGS__),AUTOBOOST_PP_EMPTY()) \
/**/

#define AUTOBOOST_PP_DETAIL_IBP_IS_VARIADIC_C(...) 1 1

#else

#define AUTOBOOST_PP_DETAIL_IBP_SPLIT(i, ...) \
    AUTOBOOST_PP_DETAIL_IBP_PRIMITIVE_CAT(AUTOBOOST_PP_DETAIL_IBP_SPLIT_,i)(__VA_ARGS__) \
/**/

#define AUTOBOOST_PP_DETAIL_IBP_IS_VARIADIC_C(...) 1

#endif /* AUTOBOOST_PP_VARIADICS_MSVC */

#define AUTOBOOST_PP_DETAIL_IBP_SPLIT_0(a, ...) a
#define AUTOBOOST_PP_DETAIL_IBP_SPLIT_1(a, ...) __VA_ARGS__

#define AUTOBOOST_PP_DETAIL_IBP_CAT(a, ...) AUTOBOOST_PP_DETAIL_IBP_PRIMITIVE_CAT(a,__VA_ARGS__)
#define AUTOBOOST_PP_DETAIL_IBP_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define AUTOBOOST_PP_DETAIL_IBP_IS_VARIADIC_R_1 1,
#define AUTOBOOST_PP_DETAIL_IBP_IS_VARIADIC_R_AUTOBOOST_PP_DETAIL_IBP_IS_VARIADIC_C 0,

#endif /* AUTOBOOST_PREPROCESSOR_DETAIL_IS_BEGIN_PARENS_HPP */

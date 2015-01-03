# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_FACILITIES_APPLY_HPP
# define AUTOBOOST_PREPROCESSOR_FACILITIES_APPLY_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/expr_iif.hpp>
# include <autoboost/preprocessor/detail/is_unary.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_APPLY */
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_APPLY(x) AUTOBOOST_PP_APPLY_I(x)
#    define AUTOBOOST_PP_APPLY_I(x) AUTOBOOST_PP_EXPR_IIF(AUTOBOOST_PP_IS_UNARY(x), AUTOBOOST_PP_TUPLE_REM_1 x)
# elif AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_BCC()
#    define AUTOBOOST_PP_APPLY(x) AUTOBOOST_PP_APPLY_I(x)
#    define AUTOBOOST_PP_APPLY_I(x) AUTOBOOST_PP_APPLY_ ## x
#    define AUTOBOOST_PP_APPLY_(x) x
#    define AUTOBOOST_PP_APPLY_AUTOBOOST_PP_NIL
# else
#    define AUTOBOOST_PP_APPLY(x) AUTOBOOST_PP_EXPR_IIF(AUTOBOOST_PP_IS_UNARY(x), AUTOBOOST_PP_TUPLE_REM_1 x)
# endif
#
# endif

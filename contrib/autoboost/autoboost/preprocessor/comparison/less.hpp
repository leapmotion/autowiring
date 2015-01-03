# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_COMPARISON_LESS_HPP
# define AUTOBOOST_PREPROCESSOR_COMPARISON_LESS_HPP
#
# include <autoboost/preprocessor/comparison/less_equal.hpp>
# include <autoboost/preprocessor/comparison/not_equal.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/logical/bitand.hpp>
# include <autoboost/preprocessor/tuple/eat.hpp>
#
# /* AUTOBOOST_PP_LESS */
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & (AUTOBOOST_PP_CONFIG_MWCC() | AUTOBOOST_PP_CONFIG_DMC())
#    define AUTOBOOST_PP_LESS(x, y) AUTOBOOST_PP_BITAND(AUTOBOOST_PP_NOT_EQUAL(x, y), AUTOBOOST_PP_LESS_EQUAL(x, y))
# elif ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LESS(x, y) AUTOBOOST_PP_IIF(AUTOBOOST_PP_NOT_EQUAL(x, y), AUTOBOOST_PP_LESS_EQUAL, 0 AUTOBOOST_PP_TUPLE_EAT_2)(x, y)
# else
#    define AUTOBOOST_PP_LESS(x, y) AUTOBOOST_PP_LESS_I(x, y)
#    define AUTOBOOST_PP_LESS_I(x, y) AUTOBOOST_PP_IIF(AUTOBOOST_PP_NOT_EQUAL(x, y), AUTOBOOST_PP_LESS_EQUAL, 0 AUTOBOOST_PP_TUPLE_EAT_2)(x, y)
# endif
#
# /* AUTOBOOST_PP_LESS_D */
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & (AUTOBOOST_PP_CONFIG_MWCC() | AUTOBOOST_PP_CONFIG_DMC())
#    define AUTOBOOST_PP_LESS_D(d, x, y) AUTOBOOST_PP_BITAND(AUTOBOOST_PP_NOT_EQUAL(x, y), AUTOBOOST_PP_LESS_EQUAL_D(d, x, y))
# elif ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LESS_D(d, x, y) AUTOBOOST_PP_IIF(AUTOBOOST_PP_NOT_EQUAL(x, y), AUTOBOOST_PP_LESS_EQUAL_D, 0 AUTOBOOST_PP_TUPLE_EAT_3)(d, x, y)
# else
#    define AUTOBOOST_PP_LESS_D(d, x, y) AUTOBOOST_PP_LESS_D_I(d, x, y)
#    define AUTOBOOST_PP_LESS_D_I(d, x, y) AUTOBOOST_PP_IIF(AUTOBOOST_PP_NOT_EQUAL(x, y), AUTOBOOST_PP_LESS_EQUAL_D, 0 AUTOBOOST_PP_TUPLE_EAT_3)(d, x, y)
# endif
#
# endif

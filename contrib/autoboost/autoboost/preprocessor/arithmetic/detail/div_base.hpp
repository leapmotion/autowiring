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
# ifndef AUTOBOOST_PREPROCESSOR_ARITHMETIC_DETAIL_DIV_BASE_HPP
# define AUTOBOOST_PREPROCESSOR_ARITHMETIC_DETAIL_DIV_BASE_HPP
#
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/arithmetic/sub.hpp>
# include <autoboost/preprocessor/comparison/less_equal.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_DIV_BASE */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_DIV_BASE(x, y) AUTOBOOST_PP_WHILE(AUTOBOOST_PP_DIV_BASE_P, AUTOBOOST_PP_DIV_BASE_O, (0, x, y))
# else
#    define AUTOBOOST_PP_DIV_BASE(x, y) AUTOBOOST_PP_DIV_BASE_I(x, y)
#    define AUTOBOOST_PP_DIV_BASE_I(x, y) AUTOBOOST_PP_WHILE(AUTOBOOST_PP_DIV_BASE_P, AUTOBOOST_PP_DIV_BASE_O, (0, x, y))
# endif
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_DIV_BASE_P(d, rxy) AUTOBOOST_PP_DIV_BASE_P_IM(d, AUTOBOOST_PP_TUPLE_REM_3 rxy)
#    define AUTOBOOST_PP_DIV_BASE_P_IM(d, im) AUTOBOOST_PP_DIV_BASE_P_I(d, im)
# else
#    define AUTOBOOST_PP_DIV_BASE_P(d, rxy) AUTOBOOST_PP_DIV_BASE_P_I(d, AUTOBOOST_PP_TUPLE_ELEM(3, 0, rxy), AUTOBOOST_PP_TUPLE_ELEM(3, 1, rxy), AUTOBOOST_PP_TUPLE_ELEM(3, 2, rxy))
# endif
#
# define AUTOBOOST_PP_DIV_BASE_P_I(d, r, x, y) AUTOBOOST_PP_LESS_EQUAL_D(d, y, x)
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_DIV_BASE_O(d, rxy) AUTOBOOST_PP_DIV_BASE_O_IM(d, AUTOBOOST_PP_TUPLE_REM_3 rxy)
#    define AUTOBOOST_PP_DIV_BASE_O_IM(d, im) AUTOBOOST_PP_DIV_BASE_O_I(d, im)
# else
#    define AUTOBOOST_PP_DIV_BASE_O(d, rxy) AUTOBOOST_PP_DIV_BASE_O_I(d, AUTOBOOST_PP_TUPLE_ELEM(3, 0, rxy), AUTOBOOST_PP_TUPLE_ELEM(3, 1, rxy), AUTOBOOST_PP_TUPLE_ELEM(3, 2, rxy))
# endif
#
# define AUTOBOOST_PP_DIV_BASE_O_I(d, r, x, y) (AUTOBOOST_PP_INC(r), AUTOBOOST_PP_SUB_D(d, x, y), y)
#
# /* AUTOBOOST_PP_DIV_BASE_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_DIV_BASE_D(d, x, y) AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_DIV_BASE_P, AUTOBOOST_PP_DIV_BASE_O, (0, x, y))
# else
#    define AUTOBOOST_PP_DIV_BASE_D(d, x, y) AUTOBOOST_PP_DIV_BASE_D_I(d, x, y)
#    define AUTOBOOST_PP_DIV_BASE_D_I(d, x, y) AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_DIV_BASE_P, AUTOBOOST_PP_DIV_BASE_O, (0, x, y))
# endif
#
# endif

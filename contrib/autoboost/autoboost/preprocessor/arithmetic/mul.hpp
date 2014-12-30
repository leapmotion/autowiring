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
# ifndef AUTOBOOST_PREPROCESSOR_ARITHMETIC_MUL_HPP
# define AUTOBOOST_PREPROCESSOR_ARITHMETIC_MUL_HPP
#
# include <autoboost/preprocessor/arithmetic/add.hpp>
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_MUL */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_MUL(x, y) AUTOBOOST_PP_TUPLE_ELEM(3, 0, AUTOBOOST_PP_WHILE(AUTOBOOST_PP_MUL_P, AUTOBOOST_PP_MUL_O, (0, x, y)))
# else
#    define AUTOBOOST_PP_MUL(x, y) AUTOBOOST_PP_MUL_I(x, y)
#    define AUTOBOOST_PP_MUL_I(x, y) AUTOBOOST_PP_TUPLE_ELEM(3, 0, AUTOBOOST_PP_WHILE(AUTOBOOST_PP_MUL_P, AUTOBOOST_PP_MUL_O, (0, x, y)))
# endif
#
# define AUTOBOOST_PP_MUL_P(d, rxy) AUTOBOOST_PP_TUPLE_ELEM(3, 2, rxy)
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_MUL_O(d, rxy) AUTOBOOST_PP_MUL_O_IM(d, AUTOBOOST_PP_TUPLE_REM_3 rxy)
#    define AUTOBOOST_PP_MUL_O_IM(d, im) AUTOBOOST_PP_MUL_O_I(d, im)
# else
#    define AUTOBOOST_PP_MUL_O(d, rxy) AUTOBOOST_PP_MUL_O_I(d, AUTOBOOST_PP_TUPLE_ELEM(3, 0, rxy), AUTOBOOST_PP_TUPLE_ELEM(3, 1, rxy), AUTOBOOST_PP_TUPLE_ELEM(3, 2, rxy))
# endif
#
# define AUTOBOOST_PP_MUL_O_I(d, r, x, y) (AUTOBOOST_PP_ADD_D(d, r, x), x, AUTOBOOST_PP_DEC(y))
#
# /* AUTOBOOST_PP_MUL_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_MUL_D(d, x, y) AUTOBOOST_PP_TUPLE_ELEM(3, 0, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_MUL_P, AUTOBOOST_PP_MUL_O, (0, x, y)))
# else
#    define AUTOBOOST_PP_MUL_D(d, x, y) AUTOBOOST_PP_MUL_D_I(d, x, y)
#    define AUTOBOOST_PP_MUL_D_I(d, x, y) AUTOBOOST_PP_TUPLE_ELEM(3, 0, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_MUL_P, AUTOBOOST_PP_MUL_O, (0, x, y)))
# endif
#
# endif

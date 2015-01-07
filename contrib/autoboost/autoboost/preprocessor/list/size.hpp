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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_SIZE_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_SIZE_HPP
#
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/list/adt.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_LIST_SIZE */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_SIZE(list) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE(AUTOBOOST_PP_LIST_SIZE_P, AUTOBOOST_PP_LIST_SIZE_O, (0, list)))
# else
#    define AUTOBOOST_PP_LIST_SIZE(list) AUTOBOOST_PP_LIST_SIZE_I(list)
#    define AUTOBOOST_PP_LIST_SIZE_I(list) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE(AUTOBOOST_PP_LIST_SIZE_P, AUTOBOOST_PP_LIST_SIZE_O, (0, list)))
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_SIZE_P(d, rl) AUTOBOOST_PP_LIST_IS_CONS(AUTOBOOST_PP_TUPLE_ELEM(2, 1, rl))
# else
#    define AUTOBOOST_PP_LIST_SIZE_P(d, rl) AUTOBOOST_PP_LIST_SIZE_P_I(AUTOBOOST_PP_TUPLE_REM_2 rl)
#    define AUTOBOOST_PP_LIST_SIZE_P_I(im) AUTOBOOST_PP_LIST_SIZE_P_II(im)
#    define AUTOBOOST_PP_LIST_SIZE_P_II(r, l) AUTOBOOST_PP_LIST_IS_CONS(l)
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_SIZE_O(d, rl) (AUTOBOOST_PP_INC(AUTOBOOST_PP_TUPLE_ELEM(2, 0, rl)), AUTOBOOST_PP_LIST_REST(AUTOBOOST_PP_TUPLE_ELEM(2, 1, rl)))
# else
#    define AUTOBOOST_PP_LIST_SIZE_O(d, rl) AUTOBOOST_PP_LIST_SIZE_O_I(AUTOBOOST_PP_TUPLE_REM_2 rl)
#    define AUTOBOOST_PP_LIST_SIZE_O_I(im) AUTOBOOST_PP_LIST_SIZE_O_II(im)
#    define AUTOBOOST_PP_LIST_SIZE_O_II(r, l) (AUTOBOOST_PP_INC(r), AUTOBOOST_PP_LIST_REST(l))
# endif
#
# /* AUTOBOOST_PP_LIST_SIZE_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_SIZE_D(d, list) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_LIST_SIZE_P, AUTOBOOST_PP_LIST_SIZE_O, (0, list)))
# else
#    define AUTOBOOST_PP_LIST_SIZE_D(d, list) AUTOBOOST_PP_LIST_SIZE_D_I(d, list)
#    define AUTOBOOST_PP_LIST_SIZE_D_I(d, list) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_LIST_SIZE_P, AUTOBOOST_PP_LIST_SIZE_O, (0, list)))
# endif
#
# endif

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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_FIRST_N_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_FIRST_N_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/list/adt.hpp>
# include <autoboost/preprocessor/list/reverse.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_LIST_FIRST_N */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_FIRST_N(count, list) AUTOBOOST_PP_LIST_REVERSE(AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_WHILE(AUTOBOOST_PP_LIST_FIRST_N_P, AUTOBOOST_PP_LIST_FIRST_N_O, (count, list, AUTOBOOST_PP_NIL))))
# else
#    define AUTOBOOST_PP_LIST_FIRST_N(count, list) AUTOBOOST_PP_LIST_FIRST_N_I(count, list)
#    define AUTOBOOST_PP_LIST_FIRST_N_I(count, list) AUTOBOOST_PP_LIST_REVERSE(AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_WHILE(AUTOBOOST_PP_LIST_FIRST_N_P, AUTOBOOST_PP_LIST_FIRST_N_O, (count, list, AUTOBOOST_PP_NIL))))
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_FIRST_N_P(d, data) AUTOBOOST_PP_TUPLE_ELEM(3, 0, data)
# else
#    define AUTOBOOST_PP_LIST_FIRST_N_P(d, data) AUTOBOOST_PP_LIST_FIRST_N_P_I data
#    define AUTOBOOST_PP_LIST_FIRST_N_P_I(c, l, nl) c
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_LIST_FIRST_N_O(d, data) AUTOBOOST_PP_LIST_FIRST_N_O_D data
# else
#    define AUTOBOOST_PP_LIST_FIRST_N_O(d, data) AUTOBOOST_PP_LIST_FIRST_N_O_D(AUTOBOOST_PP_TUPLE_ELEM(3, 0, data), AUTOBOOST_PP_TUPLE_ELEM(3, 1, data), AUTOBOOST_PP_TUPLE_ELEM(3, 2, data))
# endif
#
# define AUTOBOOST_PP_LIST_FIRST_N_O_D(c, l, nl) (AUTOBOOST_PP_DEC(c), AUTOBOOST_PP_LIST_REST(l), (AUTOBOOST_PP_LIST_FIRST(l), nl))
#
# /* AUTOBOOST_PP_LIST_FIRST_N_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_FIRST_N_D(d, count, list) AUTOBOOST_PP_LIST_REVERSE_D(d, AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_LIST_FIRST_N_P, AUTOBOOST_PP_LIST_FIRST_N_O, (count, list, AUTOBOOST_PP_NIL))))
# else
#    define AUTOBOOST_PP_LIST_FIRST_N_D(d, count, list) AUTOBOOST_PP_LIST_FIRST_N_D_I(d, count, list)
#    define AUTOBOOST_PP_LIST_FIRST_N_D_I(d, count, list) AUTOBOOST_PP_LIST_REVERSE_D(d, AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_LIST_FIRST_N_P, AUTOBOOST_PP_LIST_FIRST_N_O, (count, list, AUTOBOOST_PP_NIL))))
# endif
#
# endif

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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_REST_N_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_REST_N_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/list/adt.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_LIST_REST_N */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_REST_N(count, list) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE(AUTOBOOST_PP_LIST_REST_N_P, AUTOBOOST_PP_LIST_REST_N_O, (list, count)))
# else
#    define AUTOBOOST_PP_LIST_REST_N(count, list) AUTOBOOST_PP_LIST_REST_N_I(count, list)
#    define AUTOBOOST_PP_LIST_REST_N_I(count, list) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE(AUTOBOOST_PP_LIST_REST_N_P, AUTOBOOST_PP_LIST_REST_N_O, (list, count)))
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_REST_N_P(d, lc) AUTOBOOST_PP_TUPLE_ELEM(2, 1, lc)
# else
#    define AUTOBOOST_PP_LIST_REST_N_P(d, lc) AUTOBOOST_PP_LIST_REST_N_P_I lc
#    define AUTOBOOST_PP_LIST_REST_N_P_I(list, count) count
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_REST_N_O(d, lc) (AUTOBOOST_PP_LIST_REST(AUTOBOOST_PP_TUPLE_ELEM(2, 0, lc)), AUTOBOOST_PP_DEC(AUTOBOOST_PP_TUPLE_ELEM(2, 1, lc)))
# else
#    define AUTOBOOST_PP_LIST_REST_N_O(d, lc) AUTOBOOST_PP_LIST_REST_N_O_I lc
#    define AUTOBOOST_PP_LIST_REST_N_O_I(list, count) (AUTOBOOST_PP_LIST_REST(list), AUTOBOOST_PP_DEC(count))
# endif
#
# /* AUTOBOOST_PP_LIST_REST_N_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_REST_N_D(d, count, list) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_LIST_REST_N_P, AUTOBOOST_PP_LIST_REST_N_O, (list, count)))
# else
#    define AUTOBOOST_PP_LIST_REST_N_D(d, count, list) AUTOBOOST_PP_LIST_REST_N_D_I(d, count, list)
#    define AUTOBOOST_PP_LIST_REST_N_D_I(d, count, list) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_LIST_REST_N_P, AUTOBOOST_PP_LIST_REST_N_O, (list, count)))
# endif
#
# endif

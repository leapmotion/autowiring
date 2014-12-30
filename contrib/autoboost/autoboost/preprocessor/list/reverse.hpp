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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_REVERSE_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_REVERSE_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/list/fold_left.hpp>
#
# /* AUTOBOOST_PP_LIST_REVERSE */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_REVERSE(list) AUTOBOOST_PP_LIST_FOLD_LEFT(AUTOBOOST_PP_LIST_REVERSE_O, AUTOBOOST_PP_NIL, list)
# else
#    define AUTOBOOST_PP_LIST_REVERSE(list) AUTOBOOST_PP_LIST_REVERSE_I(list)
#    define AUTOBOOST_PP_LIST_REVERSE_I(list) AUTOBOOST_PP_LIST_FOLD_LEFT(AUTOBOOST_PP_LIST_REVERSE_O, AUTOBOOST_PP_NIL, list)
# endif
#
# define AUTOBOOST_PP_LIST_REVERSE_O(d, s, x) (x, s)
#
# /* AUTOBOOST_PP_LIST_REVERSE_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_REVERSE_D(d, list) AUTOBOOST_PP_LIST_FOLD_LEFT_ ## d(AUTOBOOST_PP_LIST_REVERSE_O, AUTOBOOST_PP_NIL, list)
# else
#    define AUTOBOOST_PP_LIST_REVERSE_D(d, list) AUTOBOOST_PP_LIST_REVERSE_D_I(d, list)
#    define AUTOBOOST_PP_LIST_REVERSE_D_I(d, list) AUTOBOOST_PP_LIST_FOLD_LEFT_ ## d(AUTOBOOST_PP_LIST_REVERSE_O, AUTOBOOST_PP_NIL, list)
# endif
#
# endif

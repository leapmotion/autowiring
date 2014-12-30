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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_CAT_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_CAT_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/list/adt.hpp>
# include <autoboost/preprocessor/list/fold_left.hpp>
#
# /* AUTOBOOST_PP_LIST_CAT */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_CAT(list) AUTOBOOST_PP_LIST_FOLD_LEFT(AUTOBOOST_PP_LIST_CAT_O, AUTOBOOST_PP_LIST_FIRST(list), AUTOBOOST_PP_LIST_REST(list))
# else
#    define AUTOBOOST_PP_LIST_CAT(list) AUTOBOOST_PP_LIST_CAT_I(list)
#    define AUTOBOOST_PP_LIST_CAT_I(list) AUTOBOOST_PP_LIST_FOLD_LEFT(AUTOBOOST_PP_LIST_CAT_O, AUTOBOOST_PP_LIST_FIRST(list), AUTOBOOST_PP_LIST_REST(list))
# endif
#
# define AUTOBOOST_PP_LIST_CAT_O(d, s, x) AUTOBOOST_PP_CAT(s, x)
#
# /* AUTOBOOST_PP_LIST_CAT_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_CAT_D(d, list) AUTOBOOST_PP_LIST_FOLD_LEFT_ ## d(AUTOBOOST_PP_LIST_CAT_O, AUTOBOOST_PP_LIST_FIRST(list), AUTOBOOST_PP_LIST_REST(list))
# else
#    define AUTOBOOST_PP_LIST_CAT_D(d, list) AUTOBOOST_PP_LIST_CAT_D_I(d, list)
#    define AUTOBOOST_PP_LIST_CAT_D_I(d, list) AUTOBOOST_PP_LIST_FOLD_LEFT_ ## d(AUTOBOOST_PP_LIST_CAT_O, AUTOBOOST_PP_LIST_FIRST(list), AUTOBOOST_PP_LIST_REST(list))
# endif
#
# endif

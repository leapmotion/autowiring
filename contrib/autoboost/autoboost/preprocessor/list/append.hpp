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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_APPEND_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_APPEND_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/list/fold_right.hpp>
#
# /* AUTOBOOST_PP_LIST_APPEND */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_APPEND(a, b) AUTOBOOST_PP_LIST_FOLD_RIGHT(AUTOBOOST_PP_LIST_APPEND_O, b, a)
# else
#    define AUTOBOOST_PP_LIST_APPEND(a, b) AUTOBOOST_PP_LIST_APPEND_I(a, b)
#    define AUTOBOOST_PP_LIST_APPEND_I(a, b) AUTOBOOST_PP_LIST_FOLD_RIGHT(AUTOBOOST_PP_LIST_APPEND_O, b, a)
# endif
#
# define AUTOBOOST_PP_LIST_APPEND_O(d, s, x) (x, s)
#
# /* AUTOBOOST_PP_LIST_APPEND_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_APPEND_D(d, a, b) AUTOBOOST_PP_LIST_FOLD_RIGHT_ ## d(AUTOBOOST_PP_LIST_APPEND_O, b, a)
# else
#    define AUTOBOOST_PP_LIST_APPEND_D(d, a, b) AUTOBOOST_PP_LIST_APPEND_D_I(d, a, b)
#    define AUTOBOOST_PP_LIST_APPEND_D_I(d, a, b) AUTOBOOST_PP_LIST_FOLD_RIGHT_ ## d(AUTOBOOST_PP_LIST_APPEND_O, b, a)
# endif
#
# endif

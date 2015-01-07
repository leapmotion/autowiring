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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_FOLD_RIGHT_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_FOLD_RIGHT_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/debug/error.hpp>
# include <autoboost/preprocessor/detail/auto_rec.hpp>
#
# if 0
#    define AUTOBOOST_PP_LIST_FOLD_RIGHT(op, state, list)
# endif
#
# define AUTOBOOST_PP_LIST_FOLD_RIGHT AUTOBOOST_PP_CAT(AUTOBOOST_PP_LIST_FOLD_RIGHT_, AUTOBOOST_PP_AUTO_REC(AUTOBOOST_PP_WHILE_P, 256))
#
# define AUTOBOOST_PP_LIST_FOLD_RIGHT_257(o, s, l) AUTOBOOST_PP_ERROR(0x0004)
#
# define AUTOBOOST_PP_LIST_FOLD_RIGHT_D(d, o, s, l) AUTOBOOST_PP_LIST_FOLD_RIGHT_ ## d(o, s, l)
# define AUTOBOOST_PP_LIST_FOLD_RIGHT_2ND AUTOBOOST_PP_LIST_FOLD_RIGHT
# define AUTOBOOST_PP_LIST_FOLD_RIGHT_2ND_D AUTOBOOST_PP_LIST_FOLD_RIGHT_D
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    include <autoboost/preprocessor/list/detail/edg/fold_right.hpp>
# else
#    include <autoboost/preprocessor/list/detail/fold_right.hpp>
# endif
#
# endif

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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_ENUM_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_ENUM_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/list/for_each_i.hpp>
# include <autoboost/preprocessor/punctuation/comma_if.hpp>
#
# /* AUTOBOOST_PP_LIST_ENUM */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_ENUM(list) AUTOBOOST_PP_LIST_FOR_EACH_I(AUTOBOOST_PP_LIST_ENUM_O, AUTOBOOST_PP_NIL, list)
# else
#    define AUTOBOOST_PP_LIST_ENUM(list) AUTOBOOST_PP_LIST_ENUM_I(list)
#    define AUTOBOOST_PP_LIST_ENUM_I(list) AUTOBOOST_PP_LIST_FOR_EACH_I(AUTOBOOST_PP_LIST_ENUM_O, AUTOBOOST_PP_NIL, list)
# endif
#
# define AUTOBOOST_PP_LIST_ENUM_O(r, _, i, elem) AUTOBOOST_PP_COMMA_IF(i) elem
#
# /* AUTOBOOST_PP_LIST_ENUM_R */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_ENUM_R(r, list) AUTOBOOST_PP_LIST_FOR_EACH_I_R(r, AUTOBOOST_PP_LIST_ENUM_O, AUTOBOOST_PP_NIL, list)
# else
#    define AUTOBOOST_PP_LIST_ENUM_R(r, list) AUTOBOOST_PP_LIST_ENUM_R_I(r, list)
#    define AUTOBOOST_PP_LIST_ENUM_R_I(r, list) AUTOBOOST_PP_LIST_FOR_EACH_I_R(r, AUTOBOOST_PP_LIST_ENUM_O, AUTOBOOST_PP_NIL, list)
# endif
#
# endif

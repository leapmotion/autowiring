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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_AT_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_AT_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/list/adt.hpp>
# include <autoboost/preprocessor/list/rest_n.hpp>
#
# /* AUTOBOOST_PP_LIST_AT */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_AT(list, index) AUTOBOOST_PP_LIST_FIRST(AUTOBOOST_PP_LIST_REST_N(index, list))
# else
#    define AUTOBOOST_PP_LIST_AT(list, index) AUTOBOOST_PP_LIST_AT_I(list, index)
#    define AUTOBOOST_PP_LIST_AT_I(list, index) AUTOBOOST_PP_LIST_FIRST(AUTOBOOST_PP_LIST_REST_N(index, list))
# endif
#
# /* AUTOBOOST_PP_LIST_AT_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_AT_D(d, list, index) AUTOBOOST_PP_LIST_FIRST(AUTOBOOST_PP_LIST_REST_N_D(d, index, list))
# else
#    define AUTOBOOST_PP_LIST_AT_D(d, list, index) AUTOBOOST_PP_LIST_AT_D_I(d, list, index)
#    define AUTOBOOST_PP_LIST_AT_D_I(d, list, index) AUTOBOOST_PP_LIST_FIRST(AUTOBOOST_PP_LIST_REST_N_D(d, index, list))
# endif
#
# endif

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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_TO_TUPLE_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_TO_TUPLE_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/list/enum.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
#
# /* AUTOBOOST_PP_LIST_TO_TUPLE */
#
# define AUTOBOOST_PP_LIST_TO_TUPLE(list) \
	AUTOBOOST_PP_IIF \
		( \
		AUTOBOOST_PP_LIST_IS_NIL(list), \
		AUTOBOOST_PP_LIST_TO_TUPLE_EMPTY, \
		AUTOBOOST_PP_LIST_TO_TUPLE_DO \
		) \
	(list) \
/**/
# define AUTOBOOST_PP_LIST_TO_TUPLE_EMPTY(list)
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_TO_TUPLE_DO(list) (AUTOBOOST_PP_LIST_ENUM(list))
# else
#    define AUTOBOOST_PP_LIST_TO_TUPLE_DO(list) AUTOBOOST_PP_LIST_TO_TUPLE_I(list)
#    define AUTOBOOST_PP_LIST_TO_TUPLE_I(list) (AUTOBOOST_PP_LIST_ENUM(list))
# endif
#
# /* AUTOBOOST_PP_LIST_TO_TUPLE_R */
#
# define AUTOBOOST_PP_LIST_TO_TUPLE_R(r, list) \
	AUTOBOOST_PP_IIF \
		( \
		AUTOBOOST_PP_LIST_IS_NIL(list), \
		AUTOBOOST_PP_LIST_TO_TUPLE_R_EMPTY, \
		AUTOBOOST_PP_LIST_TO_TUPLE_R_DO \
		) \
	(r, list) \
/**/
# define AUTOBOOST_PP_LIST_TO_TUPLE_R_EMPTY(r,list)
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_TO_TUPLE_R_DO(r, list) (AUTOBOOST_PP_LIST_ENUM_R(r, list))
# else
#    define AUTOBOOST_PP_LIST_TO_TUPLE_R_DO(r, list) AUTOBOOST_PP_LIST_TO_TUPLE_R_I(r, list)
#    define AUTOBOOST_PP_LIST_TO_TUPLE_R_I(r, list) (AUTOBOOST_PP_LIST_ENUM_R(r, list))
# endif
#
# endif

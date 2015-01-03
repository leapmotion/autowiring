# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_TO_LIST_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_TO_LIST_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/array/size.hpp>
# include <autoboost/preprocessor/control/if.hpp>
# include <autoboost/preprocessor/tuple/to_list.hpp>
#
# /* AUTOBOOST_PP_ARRAY_TO_LIST */
#
#    define AUTOBOOST_PP_ARRAY_TO_LIST(array) \
		AUTOBOOST_PP_IF \
			( \
			AUTOBOOST_PP_ARRAY_SIZE(array), \
			AUTOBOOST_PP_ARRAY_TO_LIST_DO, \
			AUTOBOOST_PP_ARRAY_TO_LIST_EMPTY \
			) \
		(array) \
/**/
#
#    define AUTOBOOST_PP_ARRAY_TO_LIST_EMPTY(array) AUTOBOOST_PP_NIL
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_ARRAY_TO_LIST_DO(array) AUTOBOOST_PP_ARRAY_TO_LIST_I(AUTOBOOST_PP_TUPLE_TO_LIST, array)
#    define AUTOBOOST_PP_ARRAY_TO_LIST_I(m, args) AUTOBOOST_PP_ARRAY_TO_LIST_II(m, args)
#    define AUTOBOOST_PP_ARRAY_TO_LIST_II(m, args) AUTOBOOST_PP_CAT(m ## args,)
# elif AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_ARRAY_TO_LIST_DO(array) AUTOBOOST_PP_ARRAY_TO_LIST_I(array)
#    define AUTOBOOST_PP_ARRAY_TO_LIST_I(array) AUTOBOOST_PP_TUPLE_TO_LIST ## array
# else
#    define AUTOBOOST_PP_ARRAY_TO_LIST_DO(array) AUTOBOOST_PP_TUPLE_TO_LIST array
# endif
#
# endif

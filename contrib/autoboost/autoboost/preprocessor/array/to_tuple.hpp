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
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_TO_TUPLE_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_TO_TUPLE_HPP
#
# include <autoboost/preprocessor/array/data.hpp>
# include <autoboost/preprocessor/array/size.hpp>
# include <autoboost/preprocessor/control/if.hpp>
#
# /* AUTOBOOST_PP_ARRAY_TO_TUPLE */
#
#    define AUTOBOOST_PP_ARRAY_TO_TUPLE(array) \
		AUTOBOOST_PP_IF \
			( \
			AUTOBOOST_PP_ARRAY_SIZE(array), \
			AUTOBOOST_PP_ARRAY_DATA, \
			AUTOBOOST_PP_ARRAY_TO_TUPLE_EMPTY \
			) \
		(array) \
/**/
#    define AUTOBOOST_PP_ARRAY_TO_TUPLE_EMPTY(array)
#
# endif

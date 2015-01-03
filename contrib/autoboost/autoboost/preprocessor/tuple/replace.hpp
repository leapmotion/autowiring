# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2013.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_TUPLE_REPLACE_HPP
# define AUTOBOOST_PREPROCESSOR_TUPLE_REPLACE_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# if AUTOBOOST_PP_VARIADICS
#
# include <autoboost/preprocessor/array/replace.hpp>
# include <autoboost/preprocessor/array/to_tuple.hpp>
# include <autoboost/preprocessor/tuple/to_array.hpp>
#
# /* AUTOBOOST_PP_TUPLE_REPLACE */
#
# define AUTOBOOST_PP_TUPLE_REPLACE(tuple, i, elem) \
	AUTOBOOST_PP_ARRAY_TO_TUPLE(AUTOBOOST_PP_ARRAY_REPLACE(AUTOBOOST_PP_TUPLE_TO_ARRAY(tuple), i, elem)) \
/**/
#
# /* AUTOBOOST_PP_TUPLE_REPLACE_D */
#
# define AUTOBOOST_PP_TUPLE_REPLACE_D(d, tuple, i, elem) \
	AUTOBOOST_PP_ARRAY_TO_TUPLE(AUTOBOOST_PP_ARRAY_REPLACE_D(d, AUTOBOOST_PP_TUPLE_TO_ARRAY(tuple), i, elem)) \
/**/
#
# endif // AUTOBOOST_PP_VARIADICS
#
# endif // AUTOBOOST_PREPROCESSOR_TUPLE_REPLACE_HPP

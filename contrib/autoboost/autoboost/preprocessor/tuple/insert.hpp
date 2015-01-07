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
# ifndef AUTOBOOST_PREPROCESSOR_TUPLE_INSERT_HPP
# define AUTOBOOST_PREPROCESSOR_TUPLE_INSERT_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# if AUTOBOOST_PP_VARIADICS
#
# include <autoboost/preprocessor/array/insert.hpp>
# include <autoboost/preprocessor/array/to_tuple.hpp>
# include <autoboost/preprocessor/tuple/to_array.hpp>
#
# /* AUTOBOOST_PP_TUPLE_INSERT */
#
# define AUTOBOOST_PP_TUPLE_INSERT(tuple, i, elem) \
	AUTOBOOST_PP_ARRAY_TO_TUPLE(AUTOBOOST_PP_ARRAY_INSERT(AUTOBOOST_PP_TUPLE_TO_ARRAY(tuple), i, elem)) \
/**/
#
# /* AUTOBOOST_PP_TUPLE_INSERT_D */
#
# define AUTOBOOST_PP_TUPLE_INSERT_D(d, tuple, i, elem) \
	AUTOBOOST_PP_ARRAY_TO_TUPLE(AUTOBOOST_PP_ARRAY_INSERT_D(d, AUTOBOOST_PP_TUPLE_TO_ARRAY(tuple), i, elem)) \
/**/
#
# endif // AUTOBOOST_PP_VARIADICS
#
# endif // AUTOBOOST_PREPROCESSOR_TUPLE_INSERT_HPP

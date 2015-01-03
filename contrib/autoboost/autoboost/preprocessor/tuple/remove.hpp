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
# ifndef AUTOBOOST_PREPROCESSOR_TUPLE_REMOVE_HPP
# define AUTOBOOST_PREPROCESSOR_TUPLE_REMOVE_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# if AUTOBOOST_PP_VARIADICS
#
# include <autoboost/preprocessor/array/remove.hpp>
# include <autoboost/preprocessor/array/to_tuple.hpp>
# include <autoboost/preprocessor/comparison/greater.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/tuple/size.hpp>
# include <autoboost/preprocessor/tuple/to_array.hpp>
#
# /* AUTOBOOST_PP_TUPLE_REMOVE */
#
# define AUTOBOOST_PP_TUPLE_REMOVE(tuple, i) \
	AUTOBOOST_PP_IIF \
		( \
		AUTOBOOST_PP_GREATER(AUTOBOOST_PP_TUPLE_SIZE(tuple),1), \
		AUTOBOOST_PP_TUPLE_REMOVE_EXEC, \
		AUTOBOOST_PP_TUPLE_REMOVE_RETURN \
		) \
	(tuple, i) \
/**/
#
# define AUTOBOOST_PP_TUPLE_REMOVE_EXEC(tuple, i) \
	AUTOBOOST_PP_ARRAY_TO_TUPLE(AUTOBOOST_PP_ARRAY_REMOVE(AUTOBOOST_PP_TUPLE_TO_ARRAY(tuple), i)) \
/**/
#
# define AUTOBOOST_PP_TUPLE_REMOVE_RETURN(tuple, i) tuple
#
# /* AUTOBOOST_PP_TUPLE_REMOVE_D */
#
# define AUTOBOOST_PP_TUPLE_REMOVE_D(d, tuple, i) \
	AUTOBOOST_PP_IIF \
		( \
		AUTOBOOST_PP_GREATER_D(d, AUTOBOOST_PP_TUPLE_SIZE(tuple), 1), \
		AUTOBOOST_PP_TUPLE_REMOVE_D_EXEC, \
		AUTOBOOST_PP_TUPLE_REMOVE_D_RETURN \
		) \
	(d, tuple, i) \
/**/
#
# define AUTOBOOST_PP_TUPLE_REMOVE_D_EXEC(d, tuple, i) \
	AUTOBOOST_PP_ARRAY_TO_TUPLE(AUTOBOOST_PP_ARRAY_REMOVE_D(d, AUTOBOOST_PP_TUPLE_TO_ARRAY(tuple), i)) \
/**/
#
# define AUTOBOOST_PP_TUPLE_REMOVE_D_RETURN(d, tuple, i) tuple
#
# endif // AUTOBOOST_PP_VARIADICS
#
# endif // AUTOBOOST_PREPROCESSOR_TUPLE_REMOVE_HPP

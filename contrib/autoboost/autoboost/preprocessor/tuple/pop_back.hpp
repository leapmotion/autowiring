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
# ifndef AUTOBOOST_PREPROCESSOR_TUPLE_POP_BACK_HPP
# define AUTOBOOST_PREPROCESSOR_TUPLE_POP_BACK_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# if AUTOBOOST_PP_VARIADICS
#
# include <autoboost/preprocessor/array/pop_back.hpp>
# include <autoboost/preprocessor/array/to_tuple.hpp>
# include <autoboost/preprocessor/comparison/greater.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/tuple/size.hpp>
# include <autoboost/preprocessor/tuple/to_array.hpp>
#
# /* AUTOBOOST_PP_TUPLE_POP_BACK */
#
# define AUTOBOOST_PP_TUPLE_POP_BACK(tuple) \
	AUTOBOOST_PP_IIF \
		( \
		AUTOBOOST_PP_GREATER(AUTOBOOST_PP_TUPLE_SIZE(tuple),1), \
		AUTOBOOST_PP_TUPLE_POP_BACK_EXEC, \
		AUTOBOOST_PP_TUPLE_POP_BACK_RETURN \
		) \
	(tuple) \
/**/
#
# define AUTOBOOST_PP_TUPLE_POP_BACK_EXEC(tuple) \
	AUTOBOOST_PP_ARRAY_TO_TUPLE(AUTOBOOST_PP_ARRAY_POP_BACK(AUTOBOOST_PP_TUPLE_TO_ARRAY(tuple))) \
/**/
#
# define AUTOBOOST_PP_TUPLE_POP_BACK_RETURN(tuple) tuple
#
# /* AUTOBOOST_PP_TUPLE_POP_BACK_Z */
#
# define AUTOBOOST_PP_TUPLE_POP_BACK_Z(z, tuple) \
	AUTOBOOST_PP_IIF \
		( \
		AUTOBOOST_PP_GREATER(AUTOBOOST_PP_TUPLE_SIZE(tuple),1), \
		AUTOBOOST_PP_TUPLE_POP_BACK_Z_EXEC, \
		AUTOBOOST_PP_TUPLE_POP_BACK_Z_RETURN \
		) \
	(z, tuple) \
/**/
#
# define AUTOBOOST_PP_TUPLE_POP_BACK_Z_EXEC(z, tuple) \
	AUTOBOOST_PP_ARRAY_TO_TUPLE(AUTOBOOST_PP_ARRAY_POP_BACK_Z(z, AUTOBOOST_PP_TUPLE_TO_ARRAY(tuple))) \
/**/
#
# define AUTOBOOST_PP_TUPLE_POP_BACK_Z_RETURN(z, tuple) tuple
#
# endif // AUTOBOOST_PP_VARIADICS
#
# endif // AUTOBOOST_PREPROCESSOR_TUPLE_POP_BACK_HPP

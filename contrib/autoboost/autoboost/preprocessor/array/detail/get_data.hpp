# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2014.                                    *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_DETAIL_GET_DATA_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_DETAIL_GET_DATA_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
# include <autoboost/preprocessor/control/if.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/facilities/is_1.hpp>
#
# /* AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA */
#
# define AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA_NONE(size, data)

# if AUTOBOOST_PP_VARIADICS && !(AUTOBOOST_PP_VARIADICS_MSVC && _MSC_VER <= 1400)
# 	 if AUTOBOOST_PP_VARIADICS_MSVC
# 		define AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA_ANY_VC_DEFAULT(size, data) AUTOBOOST_PP_TUPLE_REM(size) data
# 		define AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA_ANY_VC_CAT(size, data) AUTOBOOST_PP_TUPLE_REM_CAT(size) data
# 		define AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA_ANY(size, data) \
			AUTOBOOST_PP_IIF \
				( \
				AUTOBOOST_PP_IS_1(size), \
				AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA_ANY_VC_CAT, \
				AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA_ANY_VC_DEFAULT \
				) \
			(size,data) \
/**/
#    else
# 		define AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA_ANY(size, data) AUTOBOOST_PP_TUPLE_REM(size) data
#    endif
# else
# 	 define AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA_ANY(size, data) AUTOBOOST_PP_TUPLE_REM(size) data
# endif

# define AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA(size, data) \
	AUTOBOOST_PP_IF \
		( \
		size, \
		AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA_ANY, \
		AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA_NONE \
		) \
	(size,data) \
/**/
#
# endif /* AUTOBOOST_PREPROCESSOR_ARRAY_DETAIL_GET_DATA_HPP */

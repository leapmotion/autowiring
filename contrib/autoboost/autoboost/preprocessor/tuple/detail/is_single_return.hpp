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
# ifndef AUTOBOOST_PREPROCESSOR_TUPLE_DETAIL_IS_SINGLE_RETURN_HPP
# define AUTOBOOST_PREPROCESSOR_TUPLE_DETAIL_IS_SINGLE_RETURN_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_TUPLE_IS_SINGLE_RETURN */
#
# if AUTOBOOST_PP_VARIADICS && AUTOBOOST_PP_VARIADICS_MSVC
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/facilities/is_1.hpp>
# include <autoboost/preprocessor/tuple/size.hpp>
# define AUTOBOOST_PP_TUPLE_IS_SINGLE_RETURN(sr,nsr,tuple)	\
	AUTOBOOST_PP_IIF(AUTOBOOST_PP_IS_1(AUTOBOOST_PP_TUPLE_SIZE(tuple)),sr,nsr) \
	/**/
# endif /* AUTOBOOST_PP_VARIADICS && AUTOBOOST_PP_VARIADICS_MSVC */
#
# endif /* AUTOBOOST_PREPROCESSOR_TUPLE_DETAIL_IS_SINGLE_RETURN_HPP */

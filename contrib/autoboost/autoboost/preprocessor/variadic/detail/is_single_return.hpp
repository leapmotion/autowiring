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
# ifndef AUTOBOOST_PREPROCESSOR_VARIADIC_DETAIL_IS_SINGLE_RETURN_HPP
# define AUTOBOOST_PREPROCESSOR_VARIADIC_DETAIL_IS_SINGLE_RETURN_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_VARIADIC_IS_SINGLE_RETURN */
#
# if AUTOBOOST_PP_VARIADICS && AUTOBOOST_PP_VARIADICS_MSVC
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/facilities/is_1.hpp>
# include <autoboost/preprocessor/variadic/size.hpp>
# define AUTOBOOST_PP_VARIADIC_IS_SINGLE_RETURN(sr,nsr,...)	\
	AUTOBOOST_PP_IIF(AUTOBOOST_PP_IS_1(AUTOBOOST_PP_VARIADIC_SIZE(__VA_ARGS__)),sr,nsr) \
	/**/
# endif /* AUTOBOOST_PP_VARIADICS && AUTOBOOST_PP_VARIADICS_MSVC */
#
# endif /* AUTOBOOST_PREPROCESSOR_VARIADIC_DETAIL_IS_SINGLE_RETURN_HPP */

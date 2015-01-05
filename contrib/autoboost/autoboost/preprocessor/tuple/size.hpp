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
# ifndef AUTOBOOST_PREPROCESSOR_TUPLE_SIZE_HPP
# define AUTOBOOST_PREPROCESSOR_TUPLE_SIZE_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/variadic/size.hpp>
#
# if AUTOBOOST_PP_VARIADICS
#    if AUTOBOOST_PP_VARIADICS_MSVC
#        define AUTOBOOST_PP_TUPLE_SIZE(tuple) AUTOBOOST_PP_CAT(AUTOBOOST_PP_VARIADIC_SIZE tuple,)
#    else
#        define AUTOBOOST_PP_TUPLE_SIZE(tuple) AUTOBOOST_PP_VARIADIC_SIZE tuple
#    endif
# endif
#
# endif

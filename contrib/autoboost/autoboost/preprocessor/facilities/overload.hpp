# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_FACILITIES_OVERLOAD_HPP
# define AUTOBOOST_PREPROCESSOR_FACILITIES_OVERLOAD_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/variadic/size.hpp>
#
# /* AUTOBOOST_PP_OVERLOAD */
#
# if AUTOBOOST_PP_VARIADICS
#    define AUTOBOOST_PP_OVERLOAD(prefix, ...) AUTOBOOST_PP_CAT(prefix, AUTOBOOST_PP_VARIADIC_SIZE(__VA_ARGS__))
# endif
#
# endif

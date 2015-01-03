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
# ifndef AUTOBOOST_PREPROCESSOR_VARIADIC_TO_ARRAY_HPP
# define AUTOBOOST_PREPROCESSOR_VARIADIC_TO_ARRAY_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/tuple/to_array.hpp>
# if AUTOBOOST_PP_VARIADICS_MSVC
#    include <autoboost/preprocessor/variadic/size.hpp>
# endif
#
# /* AUTOBOOST_PP_VARIADIC_TO_ARRAY */
#
# if AUTOBOOST_PP_VARIADICS
#    if AUTOBOOST_PP_VARIADICS_MSVC
#        define AUTOBOOST_PP_VARIADIC_TO_ARRAY(...) AUTOBOOST_PP_TUPLE_TO_ARRAY_2(AUTOBOOST_PP_VARIADIC_SIZE(__VA_ARGS__),(__VA_ARGS__))
#    else
#        define AUTOBOOST_PP_VARIADIC_TO_ARRAY(...) AUTOBOOST_PP_TUPLE_TO_ARRAY((__VA_ARGS__))
#    endif
# endif
#
# endif

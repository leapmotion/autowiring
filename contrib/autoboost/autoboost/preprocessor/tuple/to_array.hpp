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
# ifndef AUTOBOOST_PREPROCESSOR_TUPLE_TO_ARRAY_HPP
# define AUTOBOOST_PREPROCESSOR_TUPLE_TO_ARRAY_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/facilities/overload.hpp>
# include <autoboost/preprocessor/tuple/size.hpp>
# include <autoboost/preprocessor/variadic/size.hpp>
#
# /* AUTOBOOST_PP_TUPLE_TO_ARRAY */
#
# if AUTOBOOST_PP_VARIADICS
#    if AUTOBOOST_PP_VARIADICS_MSVC
#        define AUTOBOOST_PP_TUPLE_TO_ARRAY(...) AUTOBOOST_PP_TUPLE_TO_ARRAY_I(AUTOBOOST_PP_OVERLOAD(AUTOBOOST_PP_TUPLE_TO_ARRAY_, __VA_ARGS__), (__VA_ARGS__))
#        define AUTOBOOST_PP_TUPLE_TO_ARRAY_I(m, args) AUTOBOOST_PP_TUPLE_TO_ARRAY_II(m, args)
#        define AUTOBOOST_PP_TUPLE_TO_ARRAY_II(m, args) AUTOBOOST_PP_CAT(m ## args,)
#        define AUTOBOOST_PP_TUPLE_TO_ARRAY_1(tuple) (AUTOBOOST_PP_TUPLE_SIZE(tuple), tuple)
#    else
#        define AUTOBOOST_PP_TUPLE_TO_ARRAY(...) AUTOBOOST_PP_OVERLOAD(AUTOBOOST_PP_TUPLE_TO_ARRAY_, __VA_ARGS__)(__VA_ARGS__)
#        define AUTOBOOST_PP_TUPLE_TO_ARRAY_1(tuple) (AUTOBOOST_PP_VARIADIC_SIZE tuple, tuple)
#    endif
#    define AUTOBOOST_PP_TUPLE_TO_ARRAY_2(size, tuple) (size, tuple)
# else
#    define AUTOBOOST_PP_TUPLE_TO_ARRAY(size, tuple) (size, tuple)
# endif
#
# endif

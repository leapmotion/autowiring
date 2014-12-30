# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_REVERSE_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_REVERSE_HPP
#
# include <autoboost/preprocessor/array/data.hpp>
# include <autoboost/preprocessor/array/size.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/tuple/reverse.hpp>
#
# /* AUTOBOOST_PP_ARRAY_REVERSE */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ARRAY_REVERSE(array) (AUTOBOOST_PP_ARRAY_SIZE(array), AUTOBOOST_PP_TUPLE_REVERSE(AUTOBOOST_PP_ARRAY_SIZE(array), AUTOBOOST_PP_ARRAY_DATA(array)))
# else
#    define AUTOBOOST_PP_ARRAY_REVERSE(array) AUTOBOOST_PP_ARRAY_REVERSE_I(array)
#    define AUTOBOOST_PP_ARRAY_REVERSE_I(array) (AUTOBOOST_PP_ARRAY_SIZE(array), AUTOBOOST_PP_TUPLE_REVERSE(AUTOBOOST_PP_ARRAY_SIZE(array), AUTOBOOST_PP_ARRAY_DATA(array)))
# endif
#
# endif

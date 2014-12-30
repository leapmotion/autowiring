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
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_ELEM_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_ELEM_HPP
#
# include <autoboost/preprocessor/array/data.hpp>
# include <autoboost/preprocessor/array/size.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_ARRAY_ELEM */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ARRAY_ELEM(i, array) AUTOBOOST_PP_TUPLE_ELEM(AUTOBOOST_PP_ARRAY_SIZE(array), i, AUTOBOOST_PP_ARRAY_DATA(array))
# else
#    define AUTOBOOST_PP_ARRAY_ELEM(i, array) AUTOBOOST_PP_ARRAY_ELEM_I(i, array)
#    define AUTOBOOST_PP_ARRAY_ELEM_I(i, array) AUTOBOOST_PP_TUPLE_ELEM(AUTOBOOST_PP_ARRAY_SIZE(array), i, AUTOBOOST_PP_ARRAY_DATA(array))
# endif
#
# endif

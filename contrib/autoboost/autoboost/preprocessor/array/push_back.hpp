# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     (C) Copyright Edward Diener 2014.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_PUSH_BACK_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_PUSH_BACK_HPP
#
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/array/data.hpp>
# include <autoboost/preprocessor/array/size.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/punctuation/comma_if.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
# include <autoboost/preprocessor/array/detail/get_data.hpp>
#
# /* AUTOBOOST_PP_ARRAY_PUSH_BACK */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ARRAY_PUSH_BACK(array, elem) AUTOBOOST_PP_ARRAY_PUSH_BACK_I(AUTOBOOST_PP_ARRAY_SIZE(array), AUTOBOOST_PP_ARRAY_DATA(array), elem)
# else
#    define AUTOBOOST_PP_ARRAY_PUSH_BACK(array, elem) AUTOBOOST_PP_ARRAY_PUSH_BACK_D(array, elem)
#    define AUTOBOOST_PP_ARRAY_PUSH_BACK_D(array, elem) AUTOBOOST_PP_ARRAY_PUSH_BACK_I(AUTOBOOST_PP_ARRAY_SIZE(array), AUTOBOOST_PP_ARRAY_DATA(array), elem)
# endif
#
# define AUTOBOOST_PP_ARRAY_PUSH_BACK_I(size, data, elem) (AUTOBOOST_PP_INC(size), (AUTOBOOST_PP_ARRAY_DETAIL_GET_DATA(size,data) AUTOBOOST_PP_COMMA_IF(size) elem))
#
# endif

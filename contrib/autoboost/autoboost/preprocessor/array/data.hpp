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
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_DATA_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_DATA_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_ARRAY_DATA */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ARRAY_DATA(array) AUTOBOOST_PP_TUPLE_ELEM(2, 1, array)
# else
#    define AUTOBOOST_PP_ARRAY_DATA(array) AUTOBOOST_PP_ARRAY_DATA_I(array)
#    define AUTOBOOST_PP_ARRAY_DATA_I(array) AUTOBOOST_PP_ARRAY_DATA_II array
#    define AUTOBOOST_PP_ARRAY_DATA_II(size, data) data
# endif
#
# endif

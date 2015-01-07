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
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_SIZE_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_SIZE_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_ARRAY_SIZE */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ARRAY_SIZE(array) AUTOBOOST_PP_TUPLE_ELEM(2, 0, array)
# else
#    define AUTOBOOST_PP_ARRAY_SIZE(array) AUTOBOOST_PP_ARRAY_SIZE_I(array)
#    define AUTOBOOST_PP_ARRAY_SIZE_I(array) AUTOBOOST_PP_ARRAY_SIZE_II array
#    define AUTOBOOST_PP_ARRAY_SIZE_II(size, data) size
# endif
#
# endif

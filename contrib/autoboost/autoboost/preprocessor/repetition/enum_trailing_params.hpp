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
# ifndef AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_TRAILING_PARAMS_HPP
# define AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_TRAILING_PARAMS_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/repetition/repeat.hpp>
#
# /* AUTOBOOST_PP_ENUM_TRAILING_PARAMS */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ENUM_TRAILING_PARAMS(count, param) AUTOBOOST_PP_REPEAT(count, AUTOBOOST_PP_ENUM_TRAILING_PARAMS_M, param)
# else
#    define AUTOBOOST_PP_ENUM_TRAILING_PARAMS(count, param) AUTOBOOST_PP_ENUM_TRAILING_PARAMS_I(count, param)
#    define AUTOBOOST_PP_ENUM_TRAILING_PARAMS_I(count, param) AUTOBOOST_PP_REPEAT(count, AUTOBOOST_PP_ENUM_TRAILING_PARAMS_M, param)
# endif
#
# define AUTOBOOST_PP_ENUM_TRAILING_PARAMS_M(z, n, param) , param ## n
#
# /* AUTOBOOST_PP_ENUM_TRAILING_PARAMS_Z */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ENUM_TRAILING_PARAMS_Z(z, count, param) AUTOBOOST_PP_REPEAT_ ## z(count, AUTOBOOST_PP_ENUM_TRAILING_PARAMS_M, param)
# else
#    define AUTOBOOST_PP_ENUM_TRAILING_PARAMS_Z(z, count, param) AUTOBOOST_PP_ENUM_TRAILING_PARAMS_Z_I(z, count, param)
#    define AUTOBOOST_PP_ENUM_TRAILING_PARAMS_Z_I(z, count, param) AUTOBOOST_PP_REPEAT_ ## z(count, AUTOBOOST_PP_ENUM_TRAILING_PARAMS_M, param)
# endif
#
# endif

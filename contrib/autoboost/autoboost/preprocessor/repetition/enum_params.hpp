# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_PARAMS_HPP
# define AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_PARAMS_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/punctuation/comma_if.hpp>
# include <autoboost/preprocessor/repetition/repeat.hpp>
#
# /* AUTOBOOST_PP_ENUM_PARAMS */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ENUM_PARAMS(count, param) AUTOBOOST_PP_REPEAT(count, AUTOBOOST_PP_ENUM_PARAMS_M, param)
# else
#    define AUTOBOOST_PP_ENUM_PARAMS(count, param) AUTOBOOST_PP_ENUM_PARAMS_I(count, param)
#    define AUTOBOOST_PP_ENUM_PARAMS_I(count, param) AUTOBOOST_PP_REPEAT(count, AUTOBOOST_PP_ENUM_PARAMS_M, param)
# endif
#
# define AUTOBOOST_PP_ENUM_PARAMS_M(z, n, param) AUTOBOOST_PP_COMMA_IF(n) param ## n
#
# /* AUTOBOOST_PP_ENUM_PARAMS_Z */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ENUM_PARAMS_Z(z, count, param) AUTOBOOST_PP_REPEAT_ ## z(count, AUTOBOOST_PP_ENUM_PARAMS_M, param)
# else
#    define AUTOBOOST_PP_ENUM_PARAMS_Z(z, count, param) AUTOBOOST_PP_ENUM_PARAMS_Z_I(z, count, param)
#    define AUTOBOOST_PP_ENUM_PARAMS_Z_I(z, count, param) AUTOBOOST_PP_REPEAT_ ## z(count, AUTOBOOST_PP_ENUM_PARAMS_M, param)
# endif
#
# endif

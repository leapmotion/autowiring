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
# ifndef AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_SHIFTED_PARAMS_HPP
# define AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_SHIFTED_PARAMS_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/punctuation/comma_if.hpp>
# include <autoboost/preprocessor/repetition/repeat.hpp>
#
# /* AUTOBOOST_PP_ENUM_SHIFTED_PARAMS */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ENUM_SHIFTED_PARAMS(count, param) AUTOBOOST_PP_REPEAT(AUTOBOOST_PP_DEC(count), AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_M, param)
# else
#    define AUTOBOOST_PP_ENUM_SHIFTED_PARAMS(count, param) AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_I(count, param)
#    define AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_I(count, param) AUTOBOOST_PP_REPEAT(AUTOBOOST_PP_DEC(count), AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_M, param)
# endif
#
# define AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_M(z, n, param) AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_PP_CAT(param, AUTOBOOST_PP_INC(n))
#
# /* AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_Z */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_Z(z, count, param) AUTOBOOST_PP_REPEAT_ ## z(AUTOBOOST_PP_DEC(count), AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_M, param)
# else
#    define AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_Z(z, count, param) AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_Z_I(z, count, param)
#    define AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_Z_I(z, count, param) AUTOBOOST_PP_REPEAT_ ## z(AUTOBOOST_PP_DEC(count), AUTOBOOST_PP_ENUM_SHIFTED_PARAMS_M, param)
# endif
#
# endif

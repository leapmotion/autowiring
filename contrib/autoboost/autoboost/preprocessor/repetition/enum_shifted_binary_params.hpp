# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2005.                                  *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_SHIFTED_BINARY_PARAMS_HPP
# define AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_SHIFTED_BINARY_PARAMS_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/punctuation/comma_if.hpp>
# include <autoboost/preprocessor/repetition/repeat.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS(count, p1, p2) AUTOBOOST_PP_REPEAT(AUTOBOOST_PP_DEC(count), AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_M, (p1, p2))
# else
#    define AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS(count, p1, p2) AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_I(count, p1, p2)
#    define AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_I(count, p1, p2) AUTOBOOST_PP_REPEAT(AUTOBOOST_PP_DEC(count), AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_M, (p1, p2))
# endif
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_M(z, n, pp) AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_M_IM(z, n, AUTOBOOST_PP_TUPLE_REM_2 pp)
#    define AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_M_IM(z, n, im) AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_M_I(z, n, im)
# else
#    define AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_M(z, n, pp) AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_M_I(z, n, AUTOBOOST_PP_TUPLE_ELEM(2, 0, pp), AUTOBOOST_PP_TUPLE_ELEM(2, 1, pp))
# endif
#
# define AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_M_I(z, n, p1, p2) AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_PP_CAT(p1, AUTOBOOST_PP_INC(n)) AUTOBOOST_PP_CAT(p2, AUTOBOOST_PP_INC(n))
#
# /* AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_Z */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_Z(z, count, p1, p2) AUTOBOOST_PP_REPEAT_ ## z(AUTOBOOST_PP_DEC(count), AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_M, (p1, p2))
# else
#    define AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_Z(z, count, p1, p2) AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_Z_I(z, count, p1, p2)
#    define AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_Z_I(z, count, p1, p2) AUTOBOOST_PP_REPEAT_ ## z(AUTOBOOST_PP_DEC(count), AUTOBOOST_PP_ENUM_SHIFTED_BINARY_PARAMS_M, (p1, p2))
# endif
#
# endif

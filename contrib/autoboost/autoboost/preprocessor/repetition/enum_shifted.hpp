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
# ifndef AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_SHIFTED_HPP
# define AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_SHIFTED_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/debug/error.hpp>
# include <autoboost/preprocessor/detail/auto_rec.hpp>
# include <autoboost/preprocessor/punctuation/comma_if.hpp>
# include <autoboost/preprocessor/repetition/repeat.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_ENUM_SHIFTED */
#
# if 0
#    define AUTOBOOST_PP_ENUM_SHIFTED(count, macro, data)
# endif
#
# define AUTOBOOST_PP_ENUM_SHIFTED AUTOBOOST_PP_CAT(AUTOBOOST_PP_ENUM_SHIFTED_, AUTOBOOST_PP_AUTO_REC(AUTOBOOST_PP_REPEAT_P, 4))
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ENUM_SHIFTED_1(c, m, d) AUTOBOOST_PP_REPEAT_1(AUTOBOOST_PP_DEC(c), AUTOBOOST_PP_ENUM_SHIFTED_M_1, (m, d))
#    define AUTOBOOST_PP_ENUM_SHIFTED_2(c, m, d) AUTOBOOST_PP_REPEAT_2(AUTOBOOST_PP_DEC(c), AUTOBOOST_PP_ENUM_SHIFTED_M_2, (m, d))
#    define AUTOBOOST_PP_ENUM_SHIFTED_3(c, m, d) AUTOBOOST_PP_REPEAT_3(AUTOBOOST_PP_DEC(c), AUTOBOOST_PP_ENUM_SHIFTED_M_3, (m, d))
# else
#    define AUTOBOOST_PP_ENUM_SHIFTED_1(c, m, d) AUTOBOOST_PP_ENUM_SHIFTED_1_I(c, m, d)
#    define AUTOBOOST_PP_ENUM_SHIFTED_2(c, m, d) AUTOBOOST_PP_ENUM_SHIFTED_1_2(c, m, d)
#    define AUTOBOOST_PP_ENUM_SHIFTED_3(c, m, d) AUTOBOOST_PP_ENUM_SHIFTED_1_3(c, m, d)
#    define AUTOBOOST_PP_ENUM_SHIFTED_1_I(c, m, d) AUTOBOOST_PP_REPEAT_1(AUTOBOOST_PP_DEC(c), AUTOBOOST_PP_ENUM_SHIFTED_M_1, (m, d))
#    define AUTOBOOST_PP_ENUM_SHIFTED_2_I(c, m, d) AUTOBOOST_PP_REPEAT_2(AUTOBOOST_PP_DEC(c), AUTOBOOST_PP_ENUM_SHIFTED_M_2, (m, d))
#    define AUTOBOOST_PP_ENUM_SHIFTED_3_I(c, m, d) AUTOBOOST_PP_REPEAT_3(AUTOBOOST_PP_DEC(c), AUTOBOOST_PP_ENUM_SHIFTED_M_3, (m, d))
# endif
#
# define AUTOBOOST_PP_ENUM_SHIFTED_4(c, m, d) AUTOBOOST_PP_ERROR(0x0003)
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_ENUM_SHIFTED_M_1(z, n, md) AUTOBOOST_PP_ENUM_SHIFTED_M_1_IM(z, n, AUTOBOOST_PP_TUPLE_REM_2 md)
#    define AUTOBOOST_PP_ENUM_SHIFTED_M_2(z, n, md) AUTOBOOST_PP_ENUM_SHIFTED_M_2_IM(z, n, AUTOBOOST_PP_TUPLE_REM_2 md)
#    define AUTOBOOST_PP_ENUM_SHIFTED_M_3(z, n, md) AUTOBOOST_PP_ENUM_SHIFTED_M_3_IM(z, n, AUTOBOOST_PP_TUPLE_REM_2 md)
#    define AUTOBOOST_PP_ENUM_SHIFTED_M_1_IM(z, n, im) AUTOBOOST_PP_ENUM_SHIFTED_M_1_I(z, n, im)
#    define AUTOBOOST_PP_ENUM_SHIFTED_M_2_IM(z, n, im) AUTOBOOST_PP_ENUM_SHIFTED_M_2_I(z, n, im)
#    define AUTOBOOST_PP_ENUM_SHIFTED_M_3_IM(z, n, im) AUTOBOOST_PP_ENUM_SHIFTED_M_3_I(z, n, im)
# else
#    define AUTOBOOST_PP_ENUM_SHIFTED_M_1(z, n, md) AUTOBOOST_PP_ENUM_SHIFTED_M_1_I(z, n, AUTOBOOST_PP_TUPLE_ELEM(2, 0, md), AUTOBOOST_PP_TUPLE_ELEM(2, 1, md))
#    define AUTOBOOST_PP_ENUM_SHIFTED_M_2(z, n, md) AUTOBOOST_PP_ENUM_SHIFTED_M_2_I(z, n, AUTOBOOST_PP_TUPLE_ELEM(2, 0, md), AUTOBOOST_PP_TUPLE_ELEM(2, 1, md))
#    define AUTOBOOST_PP_ENUM_SHIFTED_M_3(z, n, md) AUTOBOOST_PP_ENUM_SHIFTED_M_3_I(z, n, AUTOBOOST_PP_TUPLE_ELEM(2, 0, md), AUTOBOOST_PP_TUPLE_ELEM(2, 1, md))
# endif
#
# define AUTOBOOST_PP_ENUM_SHIFTED_M_1_I(z, n, m, d) AUTOBOOST_PP_COMMA_IF(n) m(z, AUTOBOOST_PP_INC(n), d)
# define AUTOBOOST_PP_ENUM_SHIFTED_M_2_I(z, n, m, d) AUTOBOOST_PP_COMMA_IF(n) m(z, AUTOBOOST_PP_INC(n), d)
# define AUTOBOOST_PP_ENUM_SHIFTED_M_3_I(z, n, m, d) AUTOBOOST_PP_COMMA_IF(n) m(z, AUTOBOOST_PP_INC(n), d)
#
# endif

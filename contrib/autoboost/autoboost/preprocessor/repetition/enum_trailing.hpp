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
# ifndef AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_TRAILING_HPP
# define AUTOBOOST_PREPROCESSOR_REPETITION_ENUM_TRAILING_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/debug/error.hpp>
# include <autoboost/preprocessor/detail/auto_rec.hpp>
# include <autoboost/preprocessor/repetition/repeat.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_ENUM_TRAILING */
#
# if 0
#    define AUTOBOOST_PP_ENUM_TRAILING(count, macro, data)
# endif
#
# define AUTOBOOST_PP_ENUM_TRAILING AUTOBOOST_PP_CAT(AUTOBOOST_PP_ENUM_TRAILING_, AUTOBOOST_PP_AUTO_REC(AUTOBOOST_PP_REPEAT_P, 4))
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ENUM_TRAILING_1(c, m, d) AUTOBOOST_PP_REPEAT_1(c, AUTOBOOST_PP_ENUM_TRAILING_M_1, (m, d))
#    define AUTOBOOST_PP_ENUM_TRAILING_2(c, m, d) AUTOBOOST_PP_REPEAT_2(c, AUTOBOOST_PP_ENUM_TRAILING_M_2, (m, d))
#    define AUTOBOOST_PP_ENUM_TRAILING_3(c, m, d) AUTOBOOST_PP_REPEAT_3(c, AUTOBOOST_PP_ENUM_TRAILING_M_3, (m, d))
# else
#    define AUTOBOOST_PP_ENUM_TRAILING_1(c, m, d) AUTOBOOST_PP_ENUM_TRAILING_1_I(c, m, d)
#    define AUTOBOOST_PP_ENUM_TRAILING_2(c, m, d) AUTOBOOST_PP_ENUM_TRAILING_2_I(c, m, d)
#    define AUTOBOOST_PP_ENUM_TRAILING_3(c, m, d) AUTOBOOST_PP_ENUM_TRAILING_3_I(c, m, d)
#    define AUTOBOOST_PP_ENUM_TRAILING_1_I(c, m, d) AUTOBOOST_PP_REPEAT_1(c, AUTOBOOST_PP_ENUM_TRAILING_M_1, (m, d))
#    define AUTOBOOST_PP_ENUM_TRAILING_2_I(c, m, d) AUTOBOOST_PP_REPEAT_2(c, AUTOBOOST_PP_ENUM_TRAILING_M_2, (m, d))
#    define AUTOBOOST_PP_ENUM_TRAILING_3_I(c, m, d) AUTOBOOST_PP_REPEAT_3(c, AUTOBOOST_PP_ENUM_TRAILING_M_3, (m, d))
# endif
#
# define AUTOBOOST_PP_ENUM_TRAILING_4(c, m, d) AUTOBOOST_PP_ERROR(0x0003)
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_ENUM_TRAILING_M_1(z, n, md) AUTOBOOST_PP_ENUM_TRAILING_M_1_IM(z, n, AUTOBOOST_PP_TUPLE_REM_2 md)
#    define AUTOBOOST_PP_ENUM_TRAILING_M_2(z, n, md) AUTOBOOST_PP_ENUM_TRAILING_M_2_IM(z, n, AUTOBOOST_PP_TUPLE_REM_2 md)
#    define AUTOBOOST_PP_ENUM_TRAILING_M_3(z, n, md) AUTOBOOST_PP_ENUM_TRAILING_M_3_IM(z, n, AUTOBOOST_PP_TUPLE_REM_2 md)
#    define AUTOBOOST_PP_ENUM_TRAILING_M_1_IM(z, n, im) AUTOBOOST_PP_ENUM_TRAILING_M_1_I(z, n, im)
#    define AUTOBOOST_PP_ENUM_TRAILING_M_2_IM(z, n, im) AUTOBOOST_PP_ENUM_TRAILING_M_2_I(z, n, im)
#    define AUTOBOOST_PP_ENUM_TRAILING_M_3_IM(z, n, im) AUTOBOOST_PP_ENUM_TRAILING_M_3_I(z, n, im)
# else
#    define AUTOBOOST_PP_ENUM_TRAILING_M_1(z, n, md) AUTOBOOST_PP_ENUM_TRAILING_M_1_I(z, n, AUTOBOOST_PP_TUPLE_ELEM(2, 0, md), AUTOBOOST_PP_TUPLE_ELEM(2, 1, md))
#    define AUTOBOOST_PP_ENUM_TRAILING_M_2(z, n, md) AUTOBOOST_PP_ENUM_TRAILING_M_2_I(z, n, AUTOBOOST_PP_TUPLE_ELEM(2, 0, md), AUTOBOOST_PP_TUPLE_ELEM(2, 1, md))
#    define AUTOBOOST_PP_ENUM_TRAILING_M_3(z, n, md) AUTOBOOST_PP_ENUM_TRAILING_M_3_I(z, n, AUTOBOOST_PP_TUPLE_ELEM(2, 0, md), AUTOBOOST_PP_TUPLE_ELEM(2, 1, md))
# endif
#
# define AUTOBOOST_PP_ENUM_TRAILING_M_1_I(z, n, m, d) , m(z, n, d)
# define AUTOBOOST_PP_ENUM_TRAILING_M_2_I(z, n, m, d) , m(z, n, d)
# define AUTOBOOST_PP_ENUM_TRAILING_M_3_I(z, n, m, d) , m(z, n, d)
#
# endif

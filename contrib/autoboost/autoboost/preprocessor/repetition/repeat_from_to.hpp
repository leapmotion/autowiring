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
# ifndef AUTOBOOST_PREPROCESSOR_REPETITION_REPEAT_FROM_TO_HPP
# define AUTOBOOST_PREPROCESSOR_REPETITION_REPEAT_FROM_TO_HPP
#
# include <autoboost/preprocessor/arithmetic/add.hpp>
# include <autoboost/preprocessor/arithmetic/sub.hpp>
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/debug/error.hpp>
# include <autoboost/preprocessor/detail/auto_rec.hpp>
# include <autoboost/preprocessor/repetition/repeat.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_REPEAT_FROM_TO */
#
# if 0
#    define AUTOBOOST_PP_REPEAT_FROM_TO(first, last, macro, data)
# endif
#
# define AUTOBOOST_PP_REPEAT_FROM_TO AUTOBOOST_PP_CAT(AUTOBOOST_PP_REPEAT_FROM_TO_, AUTOBOOST_PP_AUTO_REC(AUTOBOOST_PP_REPEAT_P, 4))
#
# define AUTOBOOST_PP_REPEAT_FROM_TO_1(f, l, m, dt) AUTOBOOST_PP_REPEAT_FROM_TO_D_1(AUTOBOOST_PP_AUTO_REC(AUTOBOOST_PP_WHILE_P, 256), f, l, m, dt)
# define AUTOBOOST_PP_REPEAT_FROM_TO_2(f, l, m, dt) AUTOBOOST_PP_REPEAT_FROM_TO_D_2(AUTOBOOST_PP_AUTO_REC(AUTOBOOST_PP_WHILE_P, 256), f, l, m, dt)
# define AUTOBOOST_PP_REPEAT_FROM_TO_3(f, l, m, dt) AUTOBOOST_PP_REPEAT_FROM_TO_D_3(AUTOBOOST_PP_AUTO_REC(AUTOBOOST_PP_WHILE_P, 256), f, l, m, dt)
# define AUTOBOOST_PP_REPEAT_FROM_TO_4(f, l, m, dt) AUTOBOOST_PP_ERROR(0x0003)
#
# define AUTOBOOST_PP_REPEAT_FROM_TO_1ST AUTOBOOST_PP_REPEAT_FROM_TO_1
# define AUTOBOOST_PP_REPEAT_FROM_TO_2ND AUTOBOOST_PP_REPEAT_FROM_TO_2
# define AUTOBOOST_PP_REPEAT_FROM_TO_3RD AUTOBOOST_PP_REPEAT_FROM_TO_3
#
# /* AUTOBOOST_PP_REPEAT_FROM_TO_D */
#
# if 0
#    define AUTOBOOST_PP_REPEAT_FROM_TO_D(d, first, last, macro, data)
# endif
#
# define AUTOBOOST_PP_REPEAT_FROM_TO_D AUTOBOOST_PP_CAT(AUTOBOOST_PP_REPEAT_FROM_TO_D_, AUTOBOOST_PP_AUTO_REC(AUTOBOOST_PP_REPEAT_P, 4))
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_REPEAT_FROM_TO_D_1(d, f, l, m, dt) AUTOBOOST_PP_REPEAT_1(AUTOBOOST_PP_SUB_D(d, l, f), AUTOBOOST_PP_REPEAT_FROM_TO_M_1, (d, f, m, dt))
#    define AUTOBOOST_PP_REPEAT_FROM_TO_D_2(d, f, l, m, dt) AUTOBOOST_PP_REPEAT_2(AUTOBOOST_PP_SUB_D(d, l, f), AUTOBOOST_PP_REPEAT_FROM_TO_M_2, (d, f, m, dt))
#    define AUTOBOOST_PP_REPEAT_FROM_TO_D_3(d, f, l, m, dt) AUTOBOOST_PP_REPEAT_3(AUTOBOOST_PP_SUB_D(d, l, f), AUTOBOOST_PP_REPEAT_FROM_TO_M_3, (d, f, m, dt))
# else
#    define AUTOBOOST_PP_REPEAT_FROM_TO_D_1(d, f, l, m, dt) AUTOBOOST_PP_REPEAT_FROM_TO_D_1_I(d, f, l, m, dt)
#    define AUTOBOOST_PP_REPEAT_FROM_TO_D_2(d, f, l, m, dt) AUTOBOOST_PP_REPEAT_FROM_TO_D_2_I(d, f, l, m, dt)
#    define AUTOBOOST_PP_REPEAT_FROM_TO_D_3(d, f, l, m, dt) AUTOBOOST_PP_REPEAT_FROM_TO_D_3_I(d, f, l, m, dt)
#    define AUTOBOOST_PP_REPEAT_FROM_TO_D_1_I(d, f, l, m, dt) AUTOBOOST_PP_REPEAT_1(AUTOBOOST_PP_SUB_D(d, l, f), AUTOBOOST_PP_REPEAT_FROM_TO_M_1, (d, f, m, dt))
#    define AUTOBOOST_PP_REPEAT_FROM_TO_D_2_I(d, f, l, m, dt) AUTOBOOST_PP_REPEAT_2(AUTOBOOST_PP_SUB_D(d, l, f), AUTOBOOST_PP_REPEAT_FROM_TO_M_2, (d, f, m, dt))
#    define AUTOBOOST_PP_REPEAT_FROM_TO_D_3_I(d, f, l, m, dt) AUTOBOOST_PP_REPEAT_3(AUTOBOOST_PP_SUB_D(d, l, f), AUTOBOOST_PP_REPEAT_FROM_TO_M_3, (d, f, m, dt))
# endif
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_REPEAT_FROM_TO_M_1(z, n, dfmd) AUTOBOOST_PP_REPEAT_FROM_TO_M_1_IM(z, n, AUTOBOOST_PP_TUPLE_REM_4 dfmd)
#    define AUTOBOOST_PP_REPEAT_FROM_TO_M_2(z, n, dfmd) AUTOBOOST_PP_REPEAT_FROM_TO_M_2_IM(z, n, AUTOBOOST_PP_TUPLE_REM_4 dfmd)
#    define AUTOBOOST_PP_REPEAT_FROM_TO_M_3(z, n, dfmd) AUTOBOOST_PP_REPEAT_FROM_TO_M_3_IM(z, n, AUTOBOOST_PP_TUPLE_REM_4 dfmd)
#    define AUTOBOOST_PP_REPEAT_FROM_TO_M_1_IM(z, n, im) AUTOBOOST_PP_REPEAT_FROM_TO_M_1_I(z, n, im)
#    define AUTOBOOST_PP_REPEAT_FROM_TO_M_2_IM(z, n, im) AUTOBOOST_PP_REPEAT_FROM_TO_M_2_I(z, n, im)
#    define AUTOBOOST_PP_REPEAT_FROM_TO_M_3_IM(z, n, im) AUTOBOOST_PP_REPEAT_FROM_TO_M_3_I(z, n, im)
# else
#    define AUTOBOOST_PP_REPEAT_FROM_TO_M_1(z, n, dfmd) AUTOBOOST_PP_REPEAT_FROM_TO_M_1_I(z, n, AUTOBOOST_PP_TUPLE_ELEM(4, 0, dfmd), AUTOBOOST_PP_TUPLE_ELEM(4, 1, dfmd), AUTOBOOST_PP_TUPLE_ELEM(4, 2, dfmd), AUTOBOOST_PP_TUPLE_ELEM(4, 3, dfmd))
#    define AUTOBOOST_PP_REPEAT_FROM_TO_M_2(z, n, dfmd) AUTOBOOST_PP_REPEAT_FROM_TO_M_2_I(z, n, AUTOBOOST_PP_TUPLE_ELEM(4, 0, dfmd), AUTOBOOST_PP_TUPLE_ELEM(4, 1, dfmd), AUTOBOOST_PP_TUPLE_ELEM(4, 2, dfmd), AUTOBOOST_PP_TUPLE_ELEM(4, 3, dfmd))
#    define AUTOBOOST_PP_REPEAT_FROM_TO_M_3(z, n, dfmd) AUTOBOOST_PP_REPEAT_FROM_TO_M_3_I(z, n, AUTOBOOST_PP_TUPLE_ELEM(4, 0, dfmd), AUTOBOOST_PP_TUPLE_ELEM(4, 1, dfmd), AUTOBOOST_PP_TUPLE_ELEM(4, 2, dfmd), AUTOBOOST_PP_TUPLE_ELEM(4, 3, dfmd))
# endif
#
# define AUTOBOOST_PP_REPEAT_FROM_TO_M_1_I(z, n, d, f, m, dt) AUTOBOOST_PP_REPEAT_FROM_TO_M_1_II(z, AUTOBOOST_PP_ADD_D(d, n, f), m, dt)
# define AUTOBOOST_PP_REPEAT_FROM_TO_M_2_I(z, n, d, f, m, dt) AUTOBOOST_PP_REPEAT_FROM_TO_M_2_II(z, AUTOBOOST_PP_ADD_D(d, n, f), m, dt)
# define AUTOBOOST_PP_REPEAT_FROM_TO_M_3_I(z, n, d, f, m, dt) AUTOBOOST_PP_REPEAT_FROM_TO_M_3_II(z, AUTOBOOST_PP_ADD_D(d, n, f), m, dt)
#
# define AUTOBOOST_PP_REPEAT_FROM_TO_M_1_II(z, n, m, dt) m(z, n, dt)
# define AUTOBOOST_PP_REPEAT_FROM_TO_M_2_II(z, n, m, dt) m(z, n, dt)
# define AUTOBOOST_PP_REPEAT_FROM_TO_M_3_II(z, n, m, dt) m(z, n, dt)
#
# endif

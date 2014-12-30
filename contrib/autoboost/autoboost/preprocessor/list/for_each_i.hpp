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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_LIST_FOR_EACH_I_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_LIST_FOR_EACH_I_HPP
#
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/list/adt.hpp>
# include <autoboost/preprocessor/repetition/for.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_LIST_FOR_EACH_I */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG() && ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_LIST_FOR_EACH_I(macro, data, list) AUTOBOOST_PP_FOR((macro, data, list, 0), AUTOBOOST_PP_LIST_FOR_EACH_I_P, AUTOBOOST_PP_LIST_FOR_EACH_I_O, AUTOBOOST_PP_LIST_FOR_EACH_I_M)
# else
#    define AUTOBOOST_PP_LIST_FOR_EACH_I(macro, data, list) AUTOBOOST_PP_LIST_FOR_EACH_I_I(macro, data, list)
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_I(macro, data, list) AUTOBOOST_PP_FOR((macro, data, list, 0), AUTOBOOST_PP_LIST_FOR_EACH_I_P, AUTOBOOST_PP_LIST_FOR_EACH_I_O, AUTOBOOST_PP_LIST_FOR_EACH_I_M)
# endif
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_P(r, x) AUTOBOOST_PP_LIST_FOR_EACH_I_P_D x
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_P_D(m, d, l, i) AUTOBOOST_PP_LIST_IS_CONS(l)
# else
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_P(r, x) AUTOBOOST_PP_LIST_IS_CONS(AUTOBOOST_PP_TUPLE_ELEM(4, 2, x))
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_O(r, x) AUTOBOOST_PP_LIST_FOR_EACH_I_O_D x
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_O_D(m, d, l, i) (m, d, AUTOBOOST_PP_LIST_REST(l), AUTOBOOST_PP_INC(i))
# else
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_O(r, x) (AUTOBOOST_PP_TUPLE_ELEM(4, 0, x), AUTOBOOST_PP_TUPLE_ELEM(4, 1, x), AUTOBOOST_PP_LIST_REST(AUTOBOOST_PP_TUPLE_ELEM(4, 2, x)), AUTOBOOST_PP_INC(AUTOBOOST_PP_TUPLE_ELEM(4, 3, x)))
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_M(r, x) AUTOBOOST_PP_LIST_FOR_EACH_I_M_D(r, AUTOBOOST_PP_TUPLE_ELEM(4, 0, x), AUTOBOOST_PP_TUPLE_ELEM(4, 1, x), AUTOBOOST_PP_TUPLE_ELEM(4, 2, x), AUTOBOOST_PP_TUPLE_ELEM(4, 3, x))
# else
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_M(r, x) AUTOBOOST_PP_LIST_FOR_EACH_I_M_I(r, AUTOBOOST_PP_TUPLE_REM_4 x)
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_M_I(r, x_e) AUTOBOOST_PP_LIST_FOR_EACH_I_M_D(r, x_e)
# endif
#
# define AUTOBOOST_PP_LIST_FOR_EACH_I_M_D(r, m, d, l, i) m(r, d, i, AUTOBOOST_PP_LIST_FIRST(l))
#
# /* AUTOBOOST_PP_LIST_FOR_EACH_I_R */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_R(r, macro, data, list) AUTOBOOST_PP_FOR_ ## r((macro, data, list, 0), AUTOBOOST_PP_LIST_FOR_EACH_I_P, AUTOBOOST_PP_LIST_FOR_EACH_I_O, AUTOBOOST_PP_LIST_FOR_EACH_I_M)
# else
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_R(r, macro, data, list) AUTOBOOST_PP_LIST_FOR_EACH_I_R_I(r, macro, data, list)
#    define AUTOBOOST_PP_LIST_FOR_EACH_I_R_I(r, macro, data, list) AUTOBOOST_PP_FOR_ ## r((macro, data, list, 0), AUTOBOOST_PP_LIST_FOR_EACH_I_P, AUTOBOOST_PP_LIST_FOR_EACH_I_O, AUTOBOOST_PP_LIST_FOR_EACH_I_M)
# endif
#
# endif

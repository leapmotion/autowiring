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
# ifndef AUTOBOOST_PREPROCESSOR_ARITHMETIC_MOD_HPP
# define AUTOBOOST_PREPROCESSOR_ARITHMETIC_MOD_HPP
#
# include <autoboost/preprocessor/arithmetic/detail/div_base.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_MOD */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_MOD(x, y) AUTOBOOST_PP_TUPLE_ELEM(3, 1, AUTOBOOST_PP_DIV_BASE(x, y))
# else
#    define AUTOBOOST_PP_MOD(x, y) AUTOBOOST_PP_MOD_I(x, y)
#    define AUTOBOOST_PP_MOD_I(x, y) AUTOBOOST_PP_TUPLE_ELEM(3, 1, AUTOBOOST_PP_DIV_BASE(x, y))
# endif
#
# /* AUTOBOOST_PP_MOD_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_MOD_D(d, x, y) AUTOBOOST_PP_TUPLE_ELEM(3, 1, AUTOBOOST_PP_DIV_BASE_D(d, x, y))
# else
#    define AUTOBOOST_PP_MOD_D(d, x, y) AUTOBOOST_PP_MOD_D_I(d, x, y)
#    define AUTOBOOST_PP_MOD_D_I(d, x, y) AUTOBOOST_PP_TUPLE_ELEM(3, 1, AUTOBOOST_PP_DIV_BASE_D(d, x, y))
# endif
#
# endif

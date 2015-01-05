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
# ifndef AUTOBOOST_PREPROCESSOR_COMPARISON_LESS_EQUAL_HPP
# define AUTOBOOST_PREPROCESSOR_COMPARISON_LESS_EQUAL_HPP
#
# include <autoboost/preprocessor/arithmetic/sub.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/logical/not.hpp>
#
# /* AUTOBOOST_PP_LESS_EQUAL */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LESS_EQUAL(x, y) AUTOBOOST_PP_NOT(AUTOBOOST_PP_SUB(x, y))
# else
#    define AUTOBOOST_PP_LESS_EQUAL(x, y) AUTOBOOST_PP_LESS_EQUAL_I(x, y)
#    define AUTOBOOST_PP_LESS_EQUAL_I(x, y) AUTOBOOST_PP_NOT(AUTOBOOST_PP_SUB(x, y))
# endif
#
# /* AUTOBOOST_PP_LESS_EQUAL_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LESS_EQUAL_D(d, x, y) AUTOBOOST_PP_NOT(AUTOBOOST_PP_SUB_D(d, x, y))
# else
#    define AUTOBOOST_PP_LESS_EQUAL_D(d, x, y) AUTOBOOST_PP_LESS_EQUAL_D_I(d, x, y)
#    define AUTOBOOST_PP_LESS_EQUAL_D_I(d, x, y) AUTOBOOST_PP_NOT(AUTOBOOST_PP_SUB_D(d, x, y))
# endif
#
# endif

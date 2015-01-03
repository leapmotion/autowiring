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
# ifndef AUTOBOOST_PREPROCESSOR_COMPARISON_EQUAL_HPP
# define AUTOBOOST_PREPROCESSOR_COMPARISON_EQUAL_HPP
#
# include <autoboost/preprocessor/comparison/not_equal.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/logical/compl.hpp>
#
# /* AUTOBOOST_PP_EQUAL */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_EQUAL(x, y) AUTOBOOST_PP_COMPL(AUTOBOOST_PP_NOT_EQUAL(x, y))
# else
#    define AUTOBOOST_PP_EQUAL(x, y) AUTOBOOST_PP_EQUAL_I(x, y)
#    define AUTOBOOST_PP_EQUAL_I(x, y) AUTOBOOST_PP_COMPL(AUTOBOOST_PP_NOT_EQUAL(x, y))
# endif
#
# /* AUTOBOOST_PP_EQUAL_D */
#
# define AUTOBOOST_PP_EQUAL_D(d, x, y) AUTOBOOST_PP_EQUAL(x, y)
#
# endif

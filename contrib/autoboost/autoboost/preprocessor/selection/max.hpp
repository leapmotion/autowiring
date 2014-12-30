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
# ifndef AUTOBOOST_PREPROCESSOR_SELECTION_MAX_HPP
# define AUTOBOOST_PREPROCESSOR_SELECTION_MAX_HPP
#
# include <autoboost/preprocessor/comparison/less_equal.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
#
# /* AUTOBOOST_PP_MAX */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_MAX(x, y) AUTOBOOST_PP_IIF(AUTOBOOST_PP_LESS_EQUAL(x, y), y, x)
# else
#    define AUTOBOOST_PP_MAX(x, y) AUTOBOOST_PP_MAX_I(x, y)
#    define AUTOBOOST_PP_MAX_I(x, y) AUTOBOOST_PP_IIF(AUTOBOOST_PP_LESS_EQUAL(x, y), y, x)
# endif
#
# /* AUTOBOOST_PP_MAX_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_MAX_D(d, x, y) AUTOBOOST_PP_IIF(AUTOBOOST_PP_LESS_EQUAL_D(d, x, y), y, x)
# else
#    define AUTOBOOST_PP_MAX_D(d, x, y) AUTOBOOST_PP_MAX_D_I(d, x, y)
#    define AUTOBOOST_PP_MAX_D_I(d, x, y) AUTOBOOST_PP_IIF(AUTOBOOST_PP_LESS_EQUAL_D(d, x, y), y, x)
# endif
#
# endif

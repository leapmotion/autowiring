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
# ifndef AUTOBOOST_PREPROCESSOR_COMPARISON_GREATER_HPP
# define AUTOBOOST_PREPROCESSOR_COMPARISON_GREATER_HPP
#
# include <autoboost/preprocessor/comparison/less.hpp>
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_GREATER */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_GREATER(x, y) AUTOBOOST_PP_LESS(y, x)
# else
#    define AUTOBOOST_PP_GREATER(x, y) AUTOBOOST_PP_GREATER_I(x, y)
#    define AUTOBOOST_PP_GREATER_I(x, y) AUTOBOOST_PP_LESS(y, x)
# endif
#
# /* AUTOBOOST_PP_GREATER_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_GREATER_D(d, x, y) AUTOBOOST_PP_LESS_D(d, y, x)
# else
#    define AUTOBOOST_PP_GREATER_D(d, x, y) AUTOBOOST_PP_GREATER_D_I(d, x, y)
#    define AUTOBOOST_PP_GREATER_D_I(d, x, y) AUTOBOOST_PP_LESS_D(d, y, x)
# endif
#
# endif

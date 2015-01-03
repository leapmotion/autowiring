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
# ifndef AUTOBOOST_PREPROCESSOR_FACILITIES_EXPAND_HPP
# define AUTOBOOST_PREPROCESSOR_FACILITIES_EXPAND_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC() && ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_DMC()
#    define AUTOBOOST_PP_EXPAND(x) AUTOBOOST_PP_EXPAND_I(x)
# else
#    define AUTOBOOST_PP_EXPAND(x) AUTOBOOST_PP_EXPAND_OO((x))
#    define AUTOBOOST_PP_EXPAND_OO(par) AUTOBOOST_PP_EXPAND_I ## par
# endif
#
# define AUTOBOOST_PP_EXPAND_I(x) x
#
# endif

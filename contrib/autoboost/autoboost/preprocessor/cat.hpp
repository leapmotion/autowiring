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
# ifndef AUTOBOOST_PREPROCESSOR_CAT_HPP
# define AUTOBOOST_PREPROCESSOR_CAT_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_CAT */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_CAT(a, b) AUTOBOOST_PP_CAT_I(a, b)
# else
#    define AUTOBOOST_PP_CAT(a, b) AUTOBOOST_PP_CAT_OO((a, b))
#    define AUTOBOOST_PP_CAT_OO(par) AUTOBOOST_PP_CAT_I ## par
# endif
#
# if (~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1700)
#    define AUTOBOOST_PP_CAT_I(a, b) a ## b
# else
#    define AUTOBOOST_PP_CAT_I(a, b) AUTOBOOST_PP_CAT_II(~, a ## b)
#    define AUTOBOOST_PP_CAT_II(p, res) res
# endif
#
# endif

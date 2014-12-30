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
# ifndef AUTOBOOST_PREPROCESSOR_LOGICAL_AND_HPP
# define AUTOBOOST_PREPROCESSOR_LOGICAL_AND_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/logical/bool.hpp>
# include <autoboost/preprocessor/logical/bitand.hpp>
#
# /* AUTOBOOST_PP_AND */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_AND(p, q) AUTOBOOST_PP_BITAND(AUTOBOOST_PP_BOOL(p), AUTOBOOST_PP_BOOL(q))
# else
#    define AUTOBOOST_PP_AND(p, q) AUTOBOOST_PP_AND_I(p, q)
#    define AUTOBOOST_PP_AND_I(p, q) AUTOBOOST_PP_BITAND(AUTOBOOST_PP_BOOL(p), AUTOBOOST_PP_BOOL(q))
# endif
#
# endif

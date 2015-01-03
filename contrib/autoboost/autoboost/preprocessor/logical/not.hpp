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
# ifndef AUTOBOOST_PREPROCESSOR_LOGICAL_NOT_HPP
# define AUTOBOOST_PREPROCESSOR_LOGICAL_NOT_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/logical/bool.hpp>
# include <autoboost/preprocessor/logical/compl.hpp>
#
# /* AUTOBOOST_PP_NOT */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_NOT(x) AUTOBOOST_PP_COMPL(AUTOBOOST_PP_BOOL(x))
# else
#    define AUTOBOOST_PP_NOT(x) AUTOBOOST_PP_NOT_I(x)
#    define AUTOBOOST_PP_NOT_I(x) AUTOBOOST_PP_COMPL(AUTOBOOST_PP_BOOL(x))
# endif
#
# endif

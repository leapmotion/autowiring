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
# ifndef AUTOBOOST_PREPROCESSOR_STRINGIZE_HPP
# define AUTOBOOST_PREPROCESSOR_STRINGIZE_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_STRINGIZE */
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_STRINGIZE(text) AUTOBOOST_PP_STRINGIZE_A((text))
#    define AUTOBOOST_PP_STRINGIZE_A(arg) AUTOBOOST_PP_STRINGIZE_I arg
# elif AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_STRINGIZE(text) AUTOBOOST_PP_STRINGIZE_OO((text))
#    define AUTOBOOST_PP_STRINGIZE_OO(par) AUTOBOOST_PP_STRINGIZE_I ## par
# else
#    define AUTOBOOST_PP_STRINGIZE(text) AUTOBOOST_PP_STRINGIZE_I(text)
# endif
#
# define AUTOBOOST_PP_STRINGIZE_I(text) #text
#
# endif

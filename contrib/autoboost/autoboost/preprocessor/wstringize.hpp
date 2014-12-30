# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_WSTRINGIZE_HPP
# define AUTOBOOST_PREPROCESSOR_WSTRINGIZE_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_WSTRINGIZE */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_WSTRINGIZE(text) AUTOBOOST_PP_WSTRINGIZE_I(text)
# else
#    define AUTOBOOST_PP_WSTRINGIZE(text) AUTOBOOST_PP_WSTRINGIZE_OO((text))
#    define AUTOBOOST_PP_WSTRINGIZE_OO(par) AUTOBOOST_PP_WSTRINGIZE_I ## par
# endif
#
# define AUTOBOOST_PP_WSTRINGIZE_I(text) AUTOBOOST_PP_WSTRINGIZE_II(#text)
# define AUTOBOOST_PP_WSTRINGIZE_II(str) L ## str
#
# endif

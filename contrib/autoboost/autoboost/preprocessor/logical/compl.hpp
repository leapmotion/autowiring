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
# ifndef AUTOBOOST_PREPROCESSOR_LOGICAL_COMPL_HPP
# define AUTOBOOST_PREPROCESSOR_LOGICAL_COMPL_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_COMPL */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_COMPL(x) AUTOBOOST_PP_COMPL_I(x)
# else
#    define AUTOBOOST_PP_COMPL(x) AUTOBOOST_PP_COMPL_OO((x))
#    define AUTOBOOST_PP_COMPL_OO(par) AUTOBOOST_PP_COMPL_I ## par
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_COMPL_I(x) AUTOBOOST_PP_COMPL_ ## x
# else
#    define AUTOBOOST_PP_COMPL_I(x) AUTOBOOST_PP_COMPL_ID(AUTOBOOST_PP_COMPL_ ## x)
#    define AUTOBOOST_PP_COMPL_ID(id) id
# endif
#
# define AUTOBOOST_PP_COMPL_0 1
# define AUTOBOOST_PP_COMPL_1 0
#
# endif

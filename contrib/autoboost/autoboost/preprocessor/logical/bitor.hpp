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
# ifndef AUTOBOOST_PREPROCESSOR_LOGICAL_BITOR_HPP
# define AUTOBOOST_PREPROCESSOR_LOGICAL_BITOR_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_BITOR */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_BITOR(x, y) AUTOBOOST_PP_BITOR_I(x, y)
# else
#    define AUTOBOOST_PP_BITOR(x, y) AUTOBOOST_PP_BITOR_OO((x, y))
#    define AUTOBOOST_PP_BITOR_OO(par) AUTOBOOST_PP_BITOR_I ## par
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_BITOR_I(x, y) AUTOBOOST_PP_BITOR_ ## x ## y
# else
#    define AUTOBOOST_PP_BITOR_I(x, y) AUTOBOOST_PP_BITOR_ID(AUTOBOOST_PP_BITOR_ ## x ## y)
#    define AUTOBOOST_PP_BITOR_ID(id) id
# endif
#
# define AUTOBOOST_PP_BITOR_00 0
# define AUTOBOOST_PP_BITOR_01 1
# define AUTOBOOST_PP_BITOR_10 1
# define AUTOBOOST_PP_BITOR_11 1
#
# endif

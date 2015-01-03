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
# ifndef AUTOBOOST_PREPROCESSOR_LOGICAL_BITAND_HPP
# define AUTOBOOST_PREPROCESSOR_LOGICAL_BITAND_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_BITAND */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_BITAND(x, y) AUTOBOOST_PP_BITAND_I(x, y)
# else
#    define AUTOBOOST_PP_BITAND(x, y) AUTOBOOST_PP_BITAND_OO((x, y))
#    define AUTOBOOST_PP_BITAND_OO(par) AUTOBOOST_PP_BITAND_I ## par
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_BITAND_I(x, y) AUTOBOOST_PP_BITAND_ ## x ## y
# else
#    define AUTOBOOST_PP_BITAND_I(x, y) AUTOBOOST_PP_BITAND_ID(AUTOBOOST_PP_BITAND_ ## x ## y)
#    define AUTOBOOST_PP_BITAND_ID(res) res
# endif
#
# define AUTOBOOST_PP_BITAND_00 0
# define AUTOBOOST_PP_BITAND_01 0
# define AUTOBOOST_PP_BITAND_10 0
# define AUTOBOOST_PP_BITAND_11 1
#
# endif

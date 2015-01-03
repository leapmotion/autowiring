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
# ifndef AUTOBOOST_PREPROCESSOR_LOGICAL_BITNOR_HPP
# define AUTOBOOST_PREPROCESSOR_LOGICAL_BITNOR_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_BITNOR */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_BITNOR(x, y) AUTOBOOST_PP_BITNOR_I(x, y)
# else
#    define AUTOBOOST_PP_BITNOR(x, y) AUTOBOOST_PP_BITNOR_OO((x, y))
#    define AUTOBOOST_PP_BITNOR_OO(par) AUTOBOOST_PP_BITNOR_I ## par
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_BITNOR_I(x, y) AUTOBOOST_PP_BITNOR_ ## x ## y
# else
#    define AUTOBOOST_PP_BITNOR_I(x, y) AUTOBOOST_PP_BITNOR_ID(AUTOBOOST_PP_BITNOR_ ## x ## y)
#    define AUTOBOOST_PP_BITNOR_ID(id) id
# endif
#
# define AUTOBOOST_PP_BITNOR_00 1
# define AUTOBOOST_PP_BITNOR_01 0
# define AUTOBOOST_PP_BITNOR_10 0
# define AUTOBOOST_PP_BITNOR_11 0
#
# endif

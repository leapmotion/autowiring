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
# ifndef AUTOBOOST_PREPROCESSOR_CONTROL_EXPR_IIF_HPP
# define AUTOBOOST_PREPROCESSOR_CONTROL_EXPR_IIF_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_EXPR_IIF */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_EXPR_IIF(bit, expr) AUTOBOOST_PP_EXPR_IIF_I(bit, expr)
# else
#    define AUTOBOOST_PP_EXPR_IIF(bit, expr) AUTOBOOST_PP_EXPR_IIF_OO((bit, expr))
#    define AUTOBOOST_PP_EXPR_IIF_OO(par) AUTOBOOST_PP_EXPR_IIF_I ## par
# endif
#
# define AUTOBOOST_PP_EXPR_IIF_I(bit, expr) AUTOBOOST_PP_EXPR_IIF_ ## bit(expr)
#
# define AUTOBOOST_PP_EXPR_IIF_0(expr)
# define AUTOBOOST_PP_EXPR_IIF_1(expr) expr
#
# endif

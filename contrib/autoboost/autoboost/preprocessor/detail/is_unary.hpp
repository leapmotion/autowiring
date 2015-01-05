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
# ifndef AUTOBOOST_PREPROCESSOR_DETAIL_IS_UNARY_HPP
# define AUTOBOOST_PREPROCESSOR_DETAIL_IS_UNARY_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/detail/check.hpp>
#
# /* AUTOBOOST_PP_IS_UNARY */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_IS_UNARY(x) AUTOBOOST_PP_CHECK(x, AUTOBOOST_PP_IS_UNARY_CHECK)
# else
#    define AUTOBOOST_PP_IS_UNARY(x) AUTOBOOST_PP_IS_UNARY_I(x)
#    define AUTOBOOST_PP_IS_UNARY_I(x) AUTOBOOST_PP_CHECK(x, AUTOBOOST_PP_IS_UNARY_CHECK)
# endif
#
# define AUTOBOOST_PP_IS_UNARY_CHECK(a) 1
# define AUTOBOOST_PP_CHECK_RESULT_AUTOBOOST_PP_IS_UNARY_CHECK 0, AUTOBOOST_PP_NIL
#
# endif

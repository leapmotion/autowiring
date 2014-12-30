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
# ifndef AUTOBOOST_PREPROCESSOR_PUNCTUATION_PAREN_IF_HPP
# define AUTOBOOST_PREPROCESSOR_PUNCTUATION_PAREN_IF_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/if.hpp>
# include <autoboost/preprocessor/facilities/empty.hpp>
# include <autoboost/preprocessor/punctuation/paren.hpp>
#
# /* AUTOBOOST_PP_LPAREN_IF */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LPAREN_IF(cond) AUTOBOOST_PP_IF(cond, AUTOBOOST_PP_LPAREN, AUTOBOOST_PP_EMPTY)()
# else
#    define AUTOBOOST_PP_LPAREN_IF(cond) AUTOBOOST_PP_LPAREN_IF_I(cond)
#    define AUTOBOOST_PP_LPAREN_IF_I(cond) AUTOBOOST_PP_IF(cond, AUTOBOOST_PP_LPAREN, AUTOBOOST_PP_EMPTY)()
# endif
#
# /* AUTOBOOST_PP_RPAREN_IF */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_RPAREN_IF(cond) AUTOBOOST_PP_IF(cond, AUTOBOOST_PP_RPAREN, AUTOBOOST_PP_EMPTY)()
# else
#    define AUTOBOOST_PP_RPAREN_IF(cond) AUTOBOOST_PP_RPAREN_IF_I(cond)
#    define AUTOBOOST_PP_RPAREN_IF_I(cond) AUTOBOOST_PP_IF(cond, AUTOBOOST_PP_RPAREN, AUTOBOOST_PP_EMPTY)()
# endif
#
# endif

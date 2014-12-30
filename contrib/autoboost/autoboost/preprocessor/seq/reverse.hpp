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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_REVERSE_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_REVERSE_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/facilities/empty.hpp>
# include <autoboost/preprocessor/seq/fold_left.hpp>
#
# /* AUTOBOOST_PP_SEQ_REVERSE */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_REVERSE(seq) AUTOBOOST_PP_SEQ_FOLD_LEFT(AUTOBOOST_PP_SEQ_REVERSE_O, AUTOBOOST_PP_EMPTY, seq)()
# else
#    define AUTOBOOST_PP_SEQ_REVERSE(seq) AUTOBOOST_PP_SEQ_REVERSE_I(seq)
#    define AUTOBOOST_PP_SEQ_REVERSE_I(seq) AUTOBOOST_PP_SEQ_FOLD_LEFT(AUTOBOOST_PP_SEQ_REVERSE_O, AUTOBOOST_PP_EMPTY, seq)()
# endif
#
# define AUTOBOOST_PP_SEQ_REVERSE_O(s, state, elem) (elem) state
#
# /* AUTOBOOST_PP_SEQ_REVERSE_S */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_REVERSE_S(s, seq) AUTOBOOST_PP_SEQ_FOLD_LEFT_ ## s(AUTOBOOST_PP_SEQ_REVERSE_O, AUTOBOOST_PP_EMPTY, seq)()
# else
#    define AUTOBOOST_PP_SEQ_REVERSE_S(s, seq) AUTOBOOST_PP_SEQ_REVERSE_S_I(s, seq)
#    define AUTOBOOST_PP_SEQ_REVERSE_S_I(s, seq) AUTOBOOST_PP_SEQ_FOLD_LEFT_ ## s(AUTOBOOST_PP_SEQ_REVERSE_O, AUTOBOOST_PP_EMPTY, seq)()
# endif
#
# endif

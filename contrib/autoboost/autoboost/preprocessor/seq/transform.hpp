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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_TRANSFORM_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_TRANSFORM_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/seq/fold_left.hpp>
# include <autoboost/preprocessor/seq/seq.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_SEQ_TRANSFORM */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_TRANSFORM(op, data, seq) AUTOBOOST_PP_SEQ_TAIL(AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_SEQ_FOLD_LEFT(AUTOBOOST_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# else
#    define AUTOBOOST_PP_SEQ_TRANSFORM(op, data, seq) AUTOBOOST_PP_SEQ_TRANSFORM_I(op, data, seq)
#    define AUTOBOOST_PP_SEQ_TRANSFORM_I(op, data, seq) AUTOBOOST_PP_SEQ_TAIL(AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_SEQ_FOLD_LEFT(AUTOBOOST_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# endif
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_SEQ_TRANSFORM_O(s, state, elem) AUTOBOOST_PP_SEQ_TRANSFORM_O_IM(s, AUTOBOOST_PP_TUPLE_REM_3 state, elem)
#    define AUTOBOOST_PP_SEQ_TRANSFORM_O_IM(s, im, elem) AUTOBOOST_PP_SEQ_TRANSFORM_O_I(s, im, elem)
# else
#    define AUTOBOOST_PP_SEQ_TRANSFORM_O(s, state, elem) AUTOBOOST_PP_SEQ_TRANSFORM_O_I(s, AUTOBOOST_PP_TUPLE_ELEM(3, 0, state), AUTOBOOST_PP_TUPLE_ELEM(3, 1, state), AUTOBOOST_PP_TUPLE_ELEM(3, 2, state), elem)
# endif
#
# define AUTOBOOST_PP_SEQ_TRANSFORM_O_I(s, op, data, res, elem) (op, data, res (op(s, data, elem)))
#
# /* AUTOBOOST_PP_SEQ_TRANSFORM_S */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_TRANSFORM_S(s, op, data, seq) AUTOBOOST_PP_SEQ_TAIL(AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_SEQ_FOLD_LEFT_ ## s(AUTOBOOST_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# else
#    define AUTOBOOST_PP_SEQ_TRANSFORM_S(s, op, data, seq) AUTOBOOST_PP_SEQ_TRANSFORM_S_I(s, op, data, seq)
#    define AUTOBOOST_PP_SEQ_TRANSFORM_S_I(s, op, data, seq) AUTOBOOST_PP_SEQ_TAIL(AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_SEQ_FOLD_LEFT_ ## s(AUTOBOOST_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# endif
#
# endif

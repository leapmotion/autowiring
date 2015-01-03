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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_FILTER_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_FILTER_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/expr_if.hpp>
# include <autoboost/preprocessor/facilities/empty.hpp>
# include <autoboost/preprocessor/seq/fold_left.hpp>
# include <autoboost/preprocessor/seq/seq.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_SEQ_FILTER */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_FILTER(pred, data, seq) AUTOBOOST_PP_SEQ_TAIL(AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_SEQ_FOLD_LEFT(AUTOBOOST_PP_SEQ_FILTER_O, (pred, data, (nil)), seq)))
# else
#    define AUTOBOOST_PP_SEQ_FILTER(pred, data, seq) AUTOBOOST_PP_SEQ_FILTER_I(pred, data, seq)
#    define AUTOBOOST_PP_SEQ_FILTER_I(pred, data, seq) AUTOBOOST_PP_SEQ_TAIL(AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_SEQ_FOLD_LEFT(AUTOBOOST_PP_SEQ_FILTER_O, (pred, data, (nil)), seq)))
# endif
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_SEQ_FILTER_O(s, st, elem) AUTOBOOST_PP_SEQ_FILTER_O_IM(s, AUTOBOOST_PP_TUPLE_REM_3 st, elem)
#    define AUTOBOOST_PP_SEQ_FILTER_O_IM(s, im, elem) AUTOBOOST_PP_SEQ_FILTER_O_I(s, im, elem)
# else
#    define AUTOBOOST_PP_SEQ_FILTER_O(s, st, elem) AUTOBOOST_PP_SEQ_FILTER_O_I(s, AUTOBOOST_PP_TUPLE_ELEM(3, 0, st), AUTOBOOST_PP_TUPLE_ELEM(3, 1, st), AUTOBOOST_PP_TUPLE_ELEM(3, 2, st), elem)
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_DMC()
#   define AUTOBOOST_PP_SEQ_FILTER_O_I(s, pred, data, res, elem) (pred, data, res AUTOBOOST_PP_EXPR_IF(pred(s, data, elem), (elem)))
# else
#   define AUTOBOOST_PP_SEQ_FILTER_O_I(s, pred, data, res, elem) (pred, data, res AUTOBOOST_PP_EXPR_IF(pred##(s, data, elem), (elem)))
# endif
#
# /* AUTOBOOST_PP_SEQ_FILTER_S */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_FILTER_S(s, pred, data, seq) AUTOBOOST_PP_SEQ_TAIL(AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_SEQ_FOLD_LEFT_ ## s(AUTOBOOST_PP_SEQ_FILTER_O, (pred, data, (nil)), seq)))
# else
#    define AUTOBOOST_PP_SEQ_FILTER_S(s, pred, data, seq) AUTOBOOST_PP_SEQ_FILTER_S_I(s, pred, data, seq)
#    define AUTOBOOST_PP_SEQ_FILTER_S_I(s, pred, data, seq) AUTOBOOST_PP_SEQ_TAIL(AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_SEQ_FOLD_LEFT_ ## s(AUTOBOOST_PP_SEQ_FILTER_O, (pred, data, (nil)), seq)))
# endif
#
# endif

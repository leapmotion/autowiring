# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_LIST_FILTER_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_FILTER_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/if.hpp>
# include <autoboost/preprocessor/list/fold_right.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_LIST_FILTER */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_FILTER(pred, data, list) AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_LIST_FOLD_RIGHT(AUTOBOOST_PP_LIST_FILTER_O, (pred, data, AUTOBOOST_PP_NIL), list))
# else
#    define AUTOBOOST_PP_LIST_FILTER(pred, data, list) AUTOBOOST_PP_LIST_FILTER_I(pred, data, list)
#    define AUTOBOOST_PP_LIST_FILTER_I(pred, data, list) AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_LIST_FOLD_RIGHT(AUTOBOOST_PP_LIST_FILTER_O, (pred, data, AUTOBOOST_PP_NIL), list))
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_FILTER_O(d, pdr, elem) AUTOBOOST_PP_LIST_FILTER_O_D(d, AUTOBOOST_PP_TUPLE_ELEM(3, 0, pdr), AUTOBOOST_PP_TUPLE_ELEM(3, 1, pdr), AUTOBOOST_PP_TUPLE_ELEM(3, 2, pdr), elem)
# else
#    define AUTOBOOST_PP_LIST_FILTER_O(d, pdr, elem) AUTOBOOST_PP_LIST_FILTER_O_I(d, AUTOBOOST_PP_TUPLE_REM_3 pdr, elem)
#    define AUTOBOOST_PP_LIST_FILTER_O_I(d, im, elem) AUTOBOOST_PP_LIST_FILTER_O_D(d, im, elem)
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_DMC()
#    define AUTOBOOST_PP_LIST_FILTER_O_D(d, pred, data, res, elem) (pred, data, AUTOBOOST_PP_IF(pred(d, data, elem), (elem, res), res))
# else
#    define AUTOBOOST_PP_LIST_FILTER_O_D(d, pred, data, res, elem) (pred, data, AUTOBOOST_PP_IF(pred##(d, data, elem), (elem, res), res))
# endif
#
# /* AUTOBOOST_PP_LIST_FILTER_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_FILTER_D(d, pred, data, list) AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_LIST_FOLD_RIGHT_ ## d(AUTOBOOST_PP_LIST_FILTER_O, (pred, data, AUTOBOOST_PP_NIL), list))
# else
#    define AUTOBOOST_PP_LIST_FILTER_D(d, pred, data, list) AUTOBOOST_PP_LIST_FILTER_D_I(d, pred, data, list)
#    define AUTOBOOST_PP_LIST_FILTER_D_I(d, pred, data, list) AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_LIST_FOLD_RIGHT_ ## d(AUTOBOOST_PP_LIST_FILTER_O, (pred, data, AUTOBOOST_PP_NIL), list))
# endif
#
# endif

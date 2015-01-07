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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_TRANSFORM_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_TRANSFORM_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/list/fold_right.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_LIST_TRANSFORM */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_TRANSFORM(op, data, list) AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_LIST_FOLD_RIGHT(AUTOBOOST_PP_LIST_TRANSFORM_O, (op, data, AUTOBOOST_PP_NIL), list))
# else
#    define AUTOBOOST_PP_LIST_TRANSFORM(op, data, list) AUTOBOOST_PP_LIST_TRANSFORM_I(op, data, list)
#    define AUTOBOOST_PP_LIST_TRANSFORM_I(op, data, list) AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_LIST_FOLD_RIGHT(AUTOBOOST_PP_LIST_TRANSFORM_O, (op, data, AUTOBOOST_PP_NIL), list))
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_TRANSFORM_O(d, odr, elem) AUTOBOOST_PP_LIST_TRANSFORM_O_D(d, AUTOBOOST_PP_TUPLE_ELEM(3, 0, odr), AUTOBOOST_PP_TUPLE_ELEM(3, 1, odr), AUTOBOOST_PP_TUPLE_ELEM(3, 2, odr), elem)
# else
#    define AUTOBOOST_PP_LIST_TRANSFORM_O(d, odr, elem) AUTOBOOST_PP_LIST_TRANSFORM_O_I(d, AUTOBOOST_PP_TUPLE_REM_3 odr, elem)
#    define AUTOBOOST_PP_LIST_TRANSFORM_O_I(d, im, elem) AUTOBOOST_PP_LIST_TRANSFORM_O_D(d, im, elem)
# endif
#
# define AUTOBOOST_PP_LIST_TRANSFORM_O_D(d, op, data, res, elem) (op, data, (op(d, data, elem), res))
#
# /* AUTOBOOST_PP_LIST_TRANSFORM_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_TRANSFORM_D(d, op, data, list) AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_LIST_FOLD_RIGHT_ ## d(AUTOBOOST_PP_LIST_TRANSFORM_O, (op, data, AUTOBOOST_PP_NIL), list))
# else
#    define AUTOBOOST_PP_LIST_TRANSFORM_D(d, op, data, list) AUTOBOOST_PP_LIST_TRANSFORM_D_I(d, op, data, list)
#    define AUTOBOOST_PP_LIST_TRANSFORM_D_I(d, op, data, list) AUTOBOOST_PP_TUPLE_ELEM(3, 2, AUTOBOOST_PP_LIST_FOLD_RIGHT_ ## d(AUTOBOOST_PP_LIST_TRANSFORM_O, (op, data, AUTOBOOST_PP_NIL), list))
# endif
#
# endif

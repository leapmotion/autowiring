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
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_INSERT_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_INSERT_HPP
#
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/array/elem.hpp>
# include <autoboost/preprocessor/array/push_back.hpp>
# include <autoboost/preprocessor/array/size.hpp>
# include <autoboost/preprocessor/comparison/not_equal.hpp>
# include <autoboost/preprocessor/control/deduce_d.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_ARRAY_INSERT */
#
# define AUTOBOOST_PP_ARRAY_INSERT(array, i, elem) AUTOBOOST_PP_ARRAY_INSERT_I(AUTOBOOST_PP_DEDUCE_D(), array, i, elem)
# define AUTOBOOST_PP_ARRAY_INSERT_I(d, array, i, elem) AUTOBOOST_PP_ARRAY_INSERT_D(d, array, i, elem)
#
# /* AUTOBOOST_PP_ARRAY_INSERT_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ARRAY_INSERT_D(d, array, i, elem) AUTOBOOST_PP_TUPLE_ELEM(5, 3, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_ARRAY_INSERT_P, AUTOBOOST_PP_ARRAY_INSERT_O, (0, i, elem, (0, ()), array)))
# else
#    define AUTOBOOST_PP_ARRAY_INSERT_D(d, array, i, elem) AUTOBOOST_PP_ARRAY_INSERT_D_I(d, array, i, elem)
#    define AUTOBOOST_PP_ARRAY_INSERT_D_I(d, array, i, elem) AUTOBOOST_PP_TUPLE_ELEM(5, 3, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_ARRAY_INSERT_P, AUTOBOOST_PP_ARRAY_INSERT_O, (0, i, elem, (0, ()), array)))
# endif
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_ARRAY_INSERT_P(d, state) AUTOBOOST_PP_ARRAY_INSERT_P_I state
# else
#    define AUTOBOOST_PP_ARRAY_INSERT_P(d, state) AUTOBOOST_PP_ARRAY_INSERT_P_I(nil, nil, nil, AUTOBOOST_PP_TUPLE_ELEM(5, 3, state), AUTOBOOST_PP_TUPLE_ELEM(5, 4, state))
# endif
#
# define AUTOBOOST_PP_ARRAY_INSERT_P_I(_i, _ii, _iii, res, arr) AUTOBOOST_PP_NOT_EQUAL(AUTOBOOST_PP_ARRAY_SIZE(res), AUTOBOOST_PP_INC(AUTOBOOST_PP_ARRAY_SIZE(arr)))
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_ARRAY_INSERT_O(d, state) AUTOBOOST_PP_ARRAY_INSERT_O_I state
# else
#    define AUTOBOOST_PP_ARRAY_INSERT_O(d, state) AUTOBOOST_PP_ARRAY_INSERT_O_I(AUTOBOOST_PP_TUPLE_ELEM(5, 0, state), AUTOBOOST_PP_TUPLE_ELEM(5, 1, state), AUTOBOOST_PP_TUPLE_ELEM(5, 2, state), AUTOBOOST_PP_TUPLE_ELEM(5, 3, state), AUTOBOOST_PP_TUPLE_ELEM(5, 4, state))
# endif
#
# define AUTOBOOST_PP_ARRAY_INSERT_O_I(n, i, elem, res, arr) (AUTOBOOST_PP_IIF(AUTOBOOST_PP_NOT_EQUAL(AUTOBOOST_PP_ARRAY_SIZE(res), i), AUTOBOOST_PP_INC(n), n), i, elem, AUTOBOOST_PP_ARRAY_PUSH_BACK(res, AUTOBOOST_PP_IIF(AUTOBOOST_PP_NOT_EQUAL(AUTOBOOST_PP_ARRAY_SIZE(res), i), AUTOBOOST_PP_ARRAY_ELEM(n, arr), elem)), arr)
#
# endif

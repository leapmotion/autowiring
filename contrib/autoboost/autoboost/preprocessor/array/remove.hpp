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
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_REMOVE_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_REMOVE_HPP
#
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/array/elem.hpp>
# include <autoboost/preprocessor/array/push_back.hpp>
# include <autoboost/preprocessor/array/size.hpp>
# include <autoboost/preprocessor/comparison/not_equal.hpp>
# include <autoboost/preprocessor/control/deduce_d.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/tuple/eat.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_ARRAY_REMOVE */
#
# define AUTOBOOST_PP_ARRAY_REMOVE(array, i) AUTOBOOST_PP_ARRAY_REMOVE_I(AUTOBOOST_PP_DEDUCE_D(), array, i)
# define AUTOBOOST_PP_ARRAY_REMOVE_I(d, array, i) AUTOBOOST_PP_ARRAY_REMOVE_D(d, array, i)
#
# /* AUTOBOOST_PP_ARRAY_REMOVE_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ARRAY_REMOVE_D(d, array, i) AUTOBOOST_PP_TUPLE_ELEM(4, 2, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_ARRAY_REMOVE_P, AUTOBOOST_PP_ARRAY_REMOVE_O, (0, i, (0, ()), array)))
# else
#    define AUTOBOOST_PP_ARRAY_REMOVE_D(d, array, i) AUTOBOOST_PP_ARRAY_REMOVE_D_I(d, array, i)
#    define AUTOBOOST_PP_ARRAY_REMOVE_D_I(d, array, i) AUTOBOOST_PP_TUPLE_ELEM(4, 2, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_ARRAY_REMOVE_P, AUTOBOOST_PP_ARRAY_REMOVE_O, (0, i, (0, ()), array)))
# endif
#
# define AUTOBOOST_PP_ARRAY_REMOVE_P(d, st) AUTOBOOST_PP_NOT_EQUAL(AUTOBOOST_PP_TUPLE_ELEM(4, 0, st), AUTOBOOST_PP_ARRAY_SIZE(AUTOBOOST_PP_TUPLE_ELEM(4, 3, st)))
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_ARRAY_REMOVE_O(d, st) AUTOBOOST_PP_ARRAY_REMOVE_O_I st
# else
#    define AUTOBOOST_PP_ARRAY_REMOVE_O(d, st) AUTOBOOST_PP_ARRAY_REMOVE_O_I(AUTOBOOST_PP_TUPLE_ELEM(4, 0, st), AUTOBOOST_PP_TUPLE_ELEM(4, 1, st), AUTOBOOST_PP_TUPLE_ELEM(4, 2, st), AUTOBOOST_PP_TUPLE_ELEM(4, 3, st))
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_DMC()
#    define AUTOBOOST_PP_ARRAY_REMOVE_O_I(n, i, res, arr) (AUTOBOOST_PP_INC(n), i, AUTOBOOST_PP_IIF(AUTOBOOST_PP_NOT_EQUAL(n, i), AUTOBOOST_PP_ARRAY_PUSH_BACK, res AUTOBOOST_PP_TUPLE_EAT_2)(res, AUTOBOOST_PP_ARRAY_ELEM(n, arr)), arr)
# else
#    define AUTOBOOST_PP_ARRAY_REMOVE_O_I(n, i, res, arr) (AUTOBOOST_PP_INC(n), i, AUTOBOOST_PP_IIF(AUTOBOOST_PP_NOT_EQUAL(n, i), AUTOBOOST_PP_ARRAY_PUSH_BACK, AUTOBOOST_PP_TUPLE_ELEM_2_0)(res, AUTOBOOST_PP_ARRAY_ELEM(n, arr)), arr)
# endif
#
# endif

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
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_REPLACE_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_REPLACE_HPP
#
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/array/elem.hpp>
# include <autoboost/preprocessor/array/push_back.hpp>
# include <autoboost/preprocessor/comparison/not_equal.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/deduce_d.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_ARRAY_REPLACE */
#
# define AUTOBOOST_PP_ARRAY_REPLACE(array, i, elem) AUTOBOOST_PP_ARRAY_REPLACE_I(AUTOBOOST_PP_DEDUCE_D(), array, i, elem)
# define AUTOBOOST_PP_ARRAY_REPLACE_I(d, array, i, elem) AUTOBOOST_PP_ARRAY_REPLACE_D(d, array, i, elem)
#
# /* AUTOBOOST_PP_ARRAY_REPLACE_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ARRAY_REPLACE_D(d, array, i, elem) AUTOBOOST_PP_TUPLE_ELEM(5, 3, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_ARRAY_REPLACE_P, AUTOBOOST_PP_ARRAY_REPLACE_O, (0, i, elem, (0, ()), array)))
# else
#    define AUTOBOOST_PP_ARRAY_REPLACE_D(d, array, i, elem) AUTOBOOST_PP_ARRAY_REPLACE_D_I(d, array, i, elem)
#    define AUTOBOOST_PP_ARRAY_REPLACE_D_I(d, array, i, elem) AUTOBOOST_PP_TUPLE_ELEM(5, 3, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_ARRAY_REPLACE_P, AUTOBOOST_PP_ARRAY_REPLACE_O, (0, i, elem, (0, ()), array)))
# endif
#
# define AUTOBOOST_PP_ARRAY_REPLACE_P(d, state) AUTOBOOST_PP_NOT_EQUAL(AUTOBOOST_PP_TUPLE_ELEM(5, 0, state), AUTOBOOST_PP_ARRAY_SIZE(AUTOBOOST_PP_TUPLE_ELEM(5, 4, state)))
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_ARRAY_REPLACE_O(d, state) AUTOBOOST_PP_ARRAY_REPLACE_O_I state
# else
#    define AUTOBOOST_PP_ARRAY_REPLACE_O(d, state) AUTOBOOST_PP_ARRAY_REPLACE_O_I(AUTOBOOST_PP_TUPLE_ELEM(5, 0, state), AUTOBOOST_PP_TUPLE_ELEM(5, 1, state), AUTOBOOST_PP_TUPLE_ELEM(5, 2, state), AUTOBOOST_PP_TUPLE_ELEM(5, 3, state), AUTOBOOST_PP_TUPLE_ELEM(5, 4, state))
# endif
#
# define AUTOBOOST_PP_ARRAY_REPLACE_O_I(n, i, elem, res, arr) (AUTOBOOST_PP_INC(n), i, elem, AUTOBOOST_PP_ARRAY_PUSH_BACK(res, AUTOBOOST_PP_IIF(AUTOBOOST_PP_NOT_EQUAL(n, i), AUTOBOOST_PP_ARRAY_ELEM(n, arr), elem)), arr)
#
# endif

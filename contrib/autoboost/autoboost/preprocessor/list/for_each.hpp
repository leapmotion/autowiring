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
# ifndef AUTOBOOST_PREPROCESSOR_LIST_FOR_EACH_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_FOR_EACH_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/list/for_each_i.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_LIST_FOR_EACH */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_FOR_EACH(macro, data, list) AUTOBOOST_PP_LIST_FOR_EACH_I(AUTOBOOST_PP_LIST_FOR_EACH_O, (macro, data), list)
# else
#    define AUTOBOOST_PP_LIST_FOR_EACH(macro, data, list) AUTOBOOST_PP_LIST_FOR_EACH_X(macro, data, list)
#    define AUTOBOOST_PP_LIST_FOR_EACH_X(macro, data, list) AUTOBOOST_PP_LIST_FOR_EACH_I(AUTOBOOST_PP_LIST_FOR_EACH_O, (macro, data), list)
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_FOR_EACH_O(r, md, i, elem) AUTOBOOST_PP_LIST_FOR_EACH_O_D(r, AUTOBOOST_PP_TUPLE_ELEM(2, 0, md), AUTOBOOST_PP_TUPLE_ELEM(2, 1, md), elem)
# else
#    define AUTOBOOST_PP_LIST_FOR_EACH_O(r, md, i, elem) AUTOBOOST_PP_LIST_FOR_EACH_O_I(r, AUTOBOOST_PP_TUPLE_REM_2 md, elem)
#    define AUTOBOOST_PP_LIST_FOR_EACH_O_I(r, im, elem) AUTOBOOST_PP_LIST_FOR_EACH_O_D(r, im, elem)
# endif
#
# define AUTOBOOST_PP_LIST_FOR_EACH_O_D(r, m, d, elem) m(r, d, elem)
#
# /* AUTOBOOST_PP_LIST_FOR_EACH_R */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_LIST_FOR_EACH_R(r, macro, data, list) AUTOBOOST_PP_LIST_FOR_EACH_I_R(r, AUTOBOOST_PP_LIST_FOR_EACH_O, (macro, data), list)
# else
#    define AUTOBOOST_PP_LIST_FOR_EACH_R(r, macro, data, list) AUTOBOOST_PP_LIST_FOR_EACH_R_X(r, macro, data, list)
#    define AUTOBOOST_PP_LIST_FOR_EACH_R_X(r, macro, data, list) AUTOBOOST_PP_LIST_FOR_EACH_I_R(r, AUTOBOOST_PP_LIST_FOR_EACH_O, (macro, data), list)
# endif
#
# endif

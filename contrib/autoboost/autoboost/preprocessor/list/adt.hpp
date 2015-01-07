# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  *
#  * See http://www.boost.org for most recent version.
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# ifndef AUTOBOOST_PREPROCESSOR_LIST_ADT_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_ADT_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/detail/is_binary.hpp>
# include <autoboost/preprocessor/logical/compl.hpp>
# include <autoboost/preprocessor/tuple/eat.hpp>
#
# /* AUTOBOOST_PP_LIST_CONS */
#
# define AUTOBOOST_PP_LIST_CONS(head, tail) (head, tail)
#
# /* AUTOBOOST_PP_LIST_NIL */
#
# define AUTOBOOST_PP_LIST_NIL AUTOBOOST_PP_NIL
#
# /* AUTOBOOST_PP_LIST_FIRST */
#
# define AUTOBOOST_PP_LIST_FIRST(list) AUTOBOOST_PP_LIST_FIRST_D(list)
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_LIST_FIRST_D(list) AUTOBOOST_PP_LIST_FIRST_I list
# else
#    define AUTOBOOST_PP_LIST_FIRST_D(list) AUTOBOOST_PP_LIST_FIRST_I ## list
# endif
#
# define AUTOBOOST_PP_LIST_FIRST_I(head, tail) head
#
# /* AUTOBOOST_PP_LIST_REST */
#
# define AUTOBOOST_PP_LIST_REST(list) AUTOBOOST_PP_LIST_REST_D(list)
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_LIST_REST_D(list) AUTOBOOST_PP_LIST_REST_I list
# else
#    define AUTOBOOST_PP_LIST_REST_D(list) AUTOBOOST_PP_LIST_REST_I ## list
# endif
#
# define AUTOBOOST_PP_LIST_REST_I(head, tail) tail
#
# /* AUTOBOOST_PP_LIST_IS_CONS */
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_BCC()
#    define AUTOBOOST_PP_LIST_IS_CONS(list) AUTOBOOST_PP_LIST_IS_CONS_D(list)
#    define AUTOBOOST_PP_LIST_IS_CONS_D(list) AUTOBOOST_PP_LIST_IS_CONS_ ## list
#    define AUTOBOOST_PP_LIST_IS_CONS_(head, tail) 1
#    define AUTOBOOST_PP_LIST_IS_CONS_AUTOBOOST_PP_NIL 0
# else
#    define AUTOBOOST_PP_LIST_IS_CONS(list) AUTOBOOST_PP_IS_BINARY(list)
# endif
#
# /* AUTOBOOST_PP_LIST_IS_NIL */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_BCC()
#    define AUTOBOOST_PP_LIST_IS_NIL(list) AUTOBOOST_PP_COMPL(AUTOBOOST_PP_IS_BINARY(list))
# else
#    define AUTOBOOST_PP_LIST_IS_NIL(list) AUTOBOOST_PP_COMPL(AUTOBOOST_PP_LIST_IS_CONS(list))
# endif
#
# endif

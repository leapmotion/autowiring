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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_SEQ_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_SEQ_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/seq/elem.hpp>
#
# /* AUTOBOOST_PP_SEQ_HEAD */
#
# define AUTOBOOST_PP_SEQ_HEAD(seq) AUTOBOOST_PP_SEQ_ELEM(0, seq)
#
# /* AUTOBOOST_PP_SEQ_TAIL */
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_SEQ_TAIL(seq) AUTOBOOST_PP_SEQ_TAIL_1((seq))
#    define AUTOBOOST_PP_SEQ_TAIL_1(par) AUTOBOOST_PP_SEQ_TAIL_2 ## par
#    define AUTOBOOST_PP_SEQ_TAIL_2(seq) AUTOBOOST_PP_SEQ_TAIL_I ## seq
# elif AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_SEQ_TAIL(seq) AUTOBOOST_PP_SEQ_TAIL_ID(AUTOBOOST_PP_SEQ_TAIL_I seq)
#    define AUTOBOOST_PP_SEQ_TAIL_ID(id) id
# elif AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_TAIL(seq) AUTOBOOST_PP_SEQ_TAIL_D(seq)
#    define AUTOBOOST_PP_SEQ_TAIL_D(seq) AUTOBOOST_PP_SEQ_TAIL_I seq
# else
#    define AUTOBOOST_PP_SEQ_TAIL(seq) AUTOBOOST_PP_SEQ_TAIL_I seq
# endif
#
# define AUTOBOOST_PP_SEQ_TAIL_I(x)
#
# /* AUTOBOOST_PP_SEQ_NIL */
#
# define AUTOBOOST_PP_SEQ_NIL(x) (x)
#
# endif

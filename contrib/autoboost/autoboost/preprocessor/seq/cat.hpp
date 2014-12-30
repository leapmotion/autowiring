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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_CAT_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_CAT_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/if.hpp>
# include <autoboost/preprocessor/seq/fold_left.hpp>
# include <autoboost/preprocessor/seq/seq.hpp>
# include <autoboost/preprocessor/seq/size.hpp>
# include <autoboost/preprocessor/tuple/eat.hpp>
#
# /* AUTOBOOST_PP_SEQ_CAT */
#
# define AUTOBOOST_PP_SEQ_CAT(seq) \
    AUTOBOOST_PP_IF( \
        AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_SIZE(seq)), \
        AUTOBOOST_PP_SEQ_CAT_I, \
        AUTOBOOST_PP_SEQ_HEAD \
    )(seq) \
    /**/
# define AUTOBOOST_PP_SEQ_CAT_I(seq) AUTOBOOST_PP_SEQ_FOLD_LEFT(AUTOBOOST_PP_SEQ_CAT_O, AUTOBOOST_PP_SEQ_HEAD(seq), AUTOBOOST_PP_SEQ_TAIL(seq))
#
# define AUTOBOOST_PP_SEQ_CAT_O(s, st, elem) AUTOBOOST_PP_SEQ_CAT_O_I(st, elem)
# define AUTOBOOST_PP_SEQ_CAT_O_I(a, b) a ## b
#
# /* AUTOBOOST_PP_SEQ_CAT_S */
#
# define AUTOBOOST_PP_SEQ_CAT_S(s, seq) \
    AUTOBOOST_PP_IF( \
        AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_SIZE(seq)), \
        AUTOBOOST_PP_SEQ_CAT_S_I_A, \
        AUTOBOOST_PP_SEQ_CAT_S_I_B \
    )(s, seq) \
    /**/
# define AUTOBOOST_PP_SEQ_CAT_S_I_A(s, seq) AUTOBOOST_PP_SEQ_FOLD_LEFT_ ## s(AUTOBOOST_PP_SEQ_CAT_O, AUTOBOOST_PP_SEQ_HEAD(seq), AUTOBOOST_PP_SEQ_TAIL(seq))
# define AUTOBOOST_PP_SEQ_CAT_S_I_B(s, seq) AUTOBOOST_PP_SEQ_HEAD(seq)
#
# endif

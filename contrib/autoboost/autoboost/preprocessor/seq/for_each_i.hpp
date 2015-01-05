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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_FOR_EACH_I_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_FOR_EACH_I_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/repetition/for.hpp>
# include <autoboost/preprocessor/seq/seq.hpp>
# include <autoboost/preprocessor/seq/size.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_SEQ_FOR_EACH_I */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_FOR_EACH_I(macro, data, seq) AUTOBOOST_PP_FOR((macro, data, seq (nil), 0), AUTOBOOST_PP_SEQ_FOR_EACH_I_P, AUTOBOOST_PP_SEQ_FOR_EACH_I_O, AUTOBOOST_PP_SEQ_FOR_EACH_I_M)
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH_I(macro, data, seq) AUTOBOOST_PP_SEQ_FOR_EACH_I_I(macro, data, seq)
#    define AUTOBOOST_PP_SEQ_FOR_EACH_I_I(macro, data, seq) AUTOBOOST_PP_FOR((macro, data, seq (nil), 0), AUTOBOOST_PP_SEQ_FOR_EACH_I_P, AUTOBOOST_PP_SEQ_FOR_EACH_I_O, AUTOBOOST_PP_SEQ_FOR_EACH_I_M)
# endif
#
# define AUTOBOOST_PP_SEQ_FOR_EACH_I_P(r, x) AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_SIZE(AUTOBOOST_PP_TUPLE_ELEM(4, 2, x)))
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_SEQ_FOR_EACH_I_O(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_I_O_I x
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH_I_O(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_I_O_I(AUTOBOOST_PP_TUPLE_ELEM(4, 0, x), AUTOBOOST_PP_TUPLE_ELEM(4, 1, x), AUTOBOOST_PP_TUPLE_ELEM(4, 2, x), AUTOBOOST_PP_TUPLE_ELEM(4, 3, x))
# endif
#
# define AUTOBOOST_PP_SEQ_FOR_EACH_I_O_I(macro, data, seq, i) (macro, data, AUTOBOOST_PP_SEQ_TAIL(seq), AUTOBOOST_PP_INC(i))
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_SEQ_FOR_EACH_I_M(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_I_M_IM(r, AUTOBOOST_PP_TUPLE_REM_4 x)
#    define AUTOBOOST_PP_SEQ_FOR_EACH_I_M_IM(r, im) AUTOBOOST_PP_SEQ_FOR_EACH_I_M_I(r, im)
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH_I_M(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_I_M_I(r, AUTOBOOST_PP_TUPLE_ELEM(4, 0, x), AUTOBOOST_PP_TUPLE_ELEM(4, 1, x), AUTOBOOST_PP_TUPLE_ELEM(4, 2, x), AUTOBOOST_PP_TUPLE_ELEM(4, 3, x))
# endif
#
# define AUTOBOOST_PP_SEQ_FOR_EACH_I_M_I(r, macro, data, seq, i) macro(r, data, i, AUTOBOOST_PP_SEQ_HEAD(seq))
#
# /* AUTOBOOST_PP_SEQ_FOR_EACH_I_R */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_FOR_EACH_I_R(r, macro, data, seq) AUTOBOOST_PP_FOR_ ## r((macro, data, seq (nil), 0), AUTOBOOST_PP_SEQ_FOR_EACH_I_P, AUTOBOOST_PP_SEQ_FOR_EACH_I_O, AUTOBOOST_PP_SEQ_FOR_EACH_I_M)
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH_I_R(r, macro, data, seq) AUTOBOOST_PP_SEQ_FOR_EACH_I_R_I(r, macro, data, seq)
#    define AUTOBOOST_PP_SEQ_FOR_EACH_I_R_I(r, macro, data, seq) AUTOBOOST_PP_FOR_ ## r((macro, data, seq (nil), 0), AUTOBOOST_PP_SEQ_FOR_EACH_I_P, AUTOBOOST_PP_SEQ_FOR_EACH_I_O, AUTOBOOST_PP_SEQ_FOR_EACH_I_M)
# endif
#
# endif

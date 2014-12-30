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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_FOR_EACH_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_FOR_EACH_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/repetition/for.hpp>
# include <autoboost/preprocessor/seq/seq.hpp>
# include <autoboost/preprocessor/seq/size.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_SEQ_FOR_EACH */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_FOR_EACH(macro, data, seq) AUTOBOOST_PP_FOR((macro, data, seq (nil)), AUTOBOOST_PP_SEQ_FOR_EACH_P, AUTOBOOST_PP_SEQ_FOR_EACH_O, AUTOBOOST_PP_SEQ_FOR_EACH_M)
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH(macro, data, seq) AUTOBOOST_PP_SEQ_FOR_EACH_D(macro, data, seq)
#    define AUTOBOOST_PP_SEQ_FOR_EACH_D(macro, data, seq) AUTOBOOST_PP_FOR((macro, data, seq (nil)), AUTOBOOST_PP_SEQ_FOR_EACH_P, AUTOBOOST_PP_SEQ_FOR_EACH_O, AUTOBOOST_PP_SEQ_FOR_EACH_M)
# endif
#
# define AUTOBOOST_PP_SEQ_FOR_EACH_P(r, x) AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_SIZE(AUTOBOOST_PP_TUPLE_ELEM(3, 2, x)))
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_SEQ_FOR_EACH_O(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_O_I x
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH_O(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_O_I(AUTOBOOST_PP_TUPLE_ELEM(3, 0, x), AUTOBOOST_PP_TUPLE_ELEM(3, 1, x), AUTOBOOST_PP_TUPLE_ELEM(3, 2, x))
# endif
#
# define AUTOBOOST_PP_SEQ_FOR_EACH_O_I(macro, data, seq) (macro, data, AUTOBOOST_PP_SEQ_TAIL(seq))
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_SEQ_FOR_EACH_M(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_M_IM(r, AUTOBOOST_PP_TUPLE_REM_3 x)
#    define AUTOBOOST_PP_SEQ_FOR_EACH_M_IM(r, im) AUTOBOOST_PP_SEQ_FOR_EACH_M_I(r, im)
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH_M(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_M_I(r, AUTOBOOST_PP_TUPLE_ELEM(3, 0, x), AUTOBOOST_PP_TUPLE_ELEM(3, 1, x), AUTOBOOST_PP_TUPLE_ELEM(3, 2, x))
# endif
#
# define AUTOBOOST_PP_SEQ_FOR_EACH_M_I(r, macro, data, seq) macro(r, data, AUTOBOOST_PP_SEQ_HEAD(seq))
#
# /* AUTOBOOST_PP_SEQ_FOR_EACH_R */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_FOR_EACH_R(r, macro, data, seq) AUTOBOOST_PP_FOR_ ## r((macro, data, seq (nil)), AUTOBOOST_PP_SEQ_FOR_EACH_P, AUTOBOOST_PP_SEQ_FOR_EACH_O, AUTOBOOST_PP_SEQ_FOR_EACH_M)
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH_R(r, macro, data, seq) AUTOBOOST_PP_SEQ_FOR_EACH_R_I(r, macro, data, seq)
#    define AUTOBOOST_PP_SEQ_FOR_EACH_R_I(r, macro, data, seq) AUTOBOOST_PP_FOR_ ## r((macro, data, seq (nil)), AUTOBOOST_PP_SEQ_FOR_EACH_P, AUTOBOOST_PP_SEQ_FOR_EACH_O, AUTOBOOST_PP_SEQ_FOR_EACH_M)
# endif
#
# endif

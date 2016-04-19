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
# include <autoboost/preprocessor/control/if.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/repetition/for.hpp>
# include <autoboost/preprocessor/seq/seq.hpp>
# include <autoboost/preprocessor/seq/size.hpp>
# include <autoboost/preprocessor/seq/detail/is_empty.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_SEQ_FOR_EACH */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_FOR_EACH(macro, data, seq) AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK(macro, data, seq)
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH(macro, data, seq) AUTOBOOST_PP_SEQ_FOR_EACH_D(macro, data, seq)
#    define AUTOBOOST_PP_SEQ_FOR_EACH_D(macro, data, seq) AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK(macro, data, seq)
# endif
#
#    define AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK_EXEC(macro, data, seq) AUTOBOOST_PP_FOR((macro, data, seq, AUTOBOOST_PP_SEQ_SIZE(seq)), AUTOBOOST_PP_SEQ_FOR_EACH_P, AUTOBOOST_PP_SEQ_FOR_EACH_O, AUTOBOOST_PP_SEQ_FOR_EACH_M)
#    define AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK_EMPTY(macro, data, seq)
#
#    define AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK(macro, data, seq) \
		AUTOBOOST_PP_IIF \
			( \
			AUTOBOOST_PP_SEQ_DETAIL_IS_NOT_EMPTY(seq), \
			AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK_EXEC, \
			AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK_EMPTY \
			) \
		(macro, data, seq) \
/**/
#
# define AUTOBOOST_PP_SEQ_FOR_EACH_P(r, x) AUTOBOOST_PP_TUPLE_ELEM(4, 3, x)
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_SEQ_FOR_EACH_O(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_O_I x
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH_O(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_O_I(AUTOBOOST_PP_TUPLE_ELEM(4, 0, x), AUTOBOOST_PP_TUPLE_ELEM(4, 1, x), AUTOBOOST_PP_TUPLE_ELEM(4, 2, x), AUTOBOOST_PP_TUPLE_ELEM(4, 3, x))
# endif
#
# define AUTOBOOST_PP_SEQ_FOR_EACH_O_I(macro, data, seq, sz) \
	AUTOBOOST_PP_SEQ_FOR_EACH_O_I_DEC(macro, data, seq, AUTOBOOST_PP_DEC(sz)) \
/**/
# define AUTOBOOST_PP_SEQ_FOR_EACH_O_I_DEC(macro, data, seq, sz) \
	( \
	macro, \
	data, \
	AUTOBOOST_PP_IF \
		( \
		sz, \
		AUTOBOOST_PP_SEQ_FOR_EACH_O_I_TAIL, \
		AUTOBOOST_PP_SEQ_FOR_EACH_O_I_NIL \
		) \
	(seq), \
	sz \
	) \
/**/
# define AUTOBOOST_PP_SEQ_FOR_EACH_O_I_TAIL(seq) AUTOBOOST_PP_SEQ_TAIL(seq)
# define AUTOBOOST_PP_SEQ_FOR_EACH_O_I_NIL(seq) AUTOBOOST_PP_NIL
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_STRICT()
#    define AUTOBOOST_PP_SEQ_FOR_EACH_M(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_M_IM(r, AUTOBOOST_PP_TUPLE_REM_4 x)
#    define AUTOBOOST_PP_SEQ_FOR_EACH_M_IM(r, im) AUTOBOOST_PP_SEQ_FOR_EACH_M_I(r, im)
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH_M(r, x) AUTOBOOST_PP_SEQ_FOR_EACH_M_I(r, AUTOBOOST_PP_TUPLE_ELEM(4, 0, x), AUTOBOOST_PP_TUPLE_ELEM(4, 1, x), AUTOBOOST_PP_TUPLE_ELEM(4, 2, x), AUTOBOOST_PP_TUPLE_ELEM(4, 3, x))
# endif
#
# define AUTOBOOST_PP_SEQ_FOR_EACH_M_I(r, macro, data, seq, sz) macro(r, data, AUTOBOOST_PP_SEQ_HEAD(seq))
#
# /* AUTOBOOST_PP_SEQ_FOR_EACH_R */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_FOR_EACH_R(r, macro, data, seq) AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK_R(r, macro, data, seq)
# else
#    define AUTOBOOST_PP_SEQ_FOR_EACH_R(r, macro, data, seq) AUTOBOOST_PP_SEQ_FOR_EACH_R_I(r, macro, data, seq)
#    define AUTOBOOST_PP_SEQ_FOR_EACH_R_I(r, macro, data, seq) AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK_R(r, macro, data, seq)
# endif
#
#    define AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK_EXEC_R(r, macro, data, seq) AUTOBOOST_PP_FOR_ ## r((macro, data, seq, AUTOBOOST_PP_SEQ_SIZE(seq)), AUTOBOOST_PP_SEQ_FOR_EACH_P, AUTOBOOST_PP_SEQ_FOR_EACH_O, AUTOBOOST_PP_SEQ_FOR_EACH_M)
#    define AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK_EMPTY_R(r, macro, data, seq)
#
#    define AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK_R(r, macro, data, seq) \
		AUTOBOOST_PP_IIF \
			( \
			AUTOBOOST_PP_SEQ_DETAIL_IS_NOT_EMPTY(seq), \
			AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK_EXEC_R, \
			AUTOBOOST_PP_SEQ_FOR_EACH_DETAIL_CHECK_EMPTY_R \
			) \
		(r, macro, data, seq) \
/**/
#
# endif

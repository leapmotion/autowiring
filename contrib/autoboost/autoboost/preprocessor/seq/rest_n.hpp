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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_REST_N_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_REST_N_HPP
#
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/comparison/not_equal.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/expr_iif.hpp>
# include <autoboost/preprocessor/facilities/identity.hpp>
# include <autoboost/preprocessor/logical/bitand.hpp>
# include <autoboost/preprocessor/seq/detail/is_empty.hpp>
# include <autoboost/preprocessor/seq/detail/split.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_SEQ_REST_N */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_REST_N(n, seq) AUTOBOOST_PP_SEQ_REST_N_DETAIL_EXEC(n, seq, AUTOBOOST_PP_SEQ_DETAIL_EMPTY_SIZE(seq))
# else
#    define AUTOBOOST_PP_SEQ_REST_N(n, seq) AUTOBOOST_PP_SEQ_REST_N_I(n, seq)
#    define AUTOBOOST_PP_SEQ_REST_N_I(n, seq) AUTOBOOST_PP_SEQ_REST_N_DETAIL_EXEC(n, seq, AUTOBOOST_PP_SEQ_DETAIL_EMPTY_SIZE(seq))
# endif
#
#    define AUTOBOOST_PP_SEQ_REST_N_DETAIL_EXEC(n, seq, size) \
		AUTOBOOST_PP_EXPR_IIF \
			( \
			AUTOBOOST_PP_BITAND \
				( \
				AUTOBOOST_PP_SEQ_DETAIL_IS_NOT_EMPTY_SIZE(size), \
				AUTOBOOST_PP_NOT_EQUAL(n,size) \
				), \
			AUTOBOOST_PP_TUPLE_ELEM(2, 1, AUTOBOOST_PP_SEQ_SPLIT(AUTOBOOST_PP_INC(n), AUTOBOOST_PP_IDENTITY( (nil) seq )))() \
			) \
/**/
#
# endif

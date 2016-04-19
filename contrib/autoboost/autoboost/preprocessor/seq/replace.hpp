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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_REPLACE_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_REPLACE_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/comparison/equal.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/seq/first_n.hpp>
# include <autoboost/preprocessor/seq/rest_n.hpp>
# include <autoboost/preprocessor/seq/size.hpp>
#
# /* AUTOBOOST_PP_SEQ_REPLACE */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_REPLACE(seq, i, elem) AUTOBOOST_PP_SEQ_FIRST_N(i, seq) (elem) AUTOBOOST_PP_SEQ_REPLACE_DETAIL_REST(seq, i)
# else
#    define AUTOBOOST_PP_SEQ_REPLACE(seq, i, elem) AUTOBOOST_PP_SEQ_REPLACE_I(seq, i, elem)
#    define AUTOBOOST_PP_SEQ_REPLACE_I(seq, i, elem) AUTOBOOST_PP_SEQ_FIRST_N(i, seq) (elem) AUTOBOOST_PP_SEQ_REPLACE_DETAIL_REST(seq, i)
# endif
#
#    define AUTOBOOST_PP_SEQ_REPLACE_DETAIL_REST_EMPTY(seq, i)
#    define AUTOBOOST_PP_SEQ_REPLACE_DETAIL_REST_VALID(seq, i) AUTOBOOST_PP_SEQ_REST_N(AUTOBOOST_PP_INC(i), seq)
#    define AUTOBOOST_PP_SEQ_REPLACE_DETAIL_REST(seq, i) \
		AUTOBOOST_PP_IIF \
			( \
			AUTOBOOST_PP_EQUAL(i,AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_SIZE(seq))), \
			AUTOBOOST_PP_SEQ_REPLACE_DETAIL_REST_EMPTY, \
			AUTOBOOST_PP_SEQ_REPLACE_DETAIL_REST_VALID \
			) \
		(seq, i) \
/**/
#
# endif

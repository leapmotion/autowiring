# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2015.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_DETAIL_IS_EMPTY_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_DETAIL_IS_EMPTY_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/logical/bool.hpp>
# include <autoboost/preprocessor/logical/compl.hpp>
# include <autoboost/preprocessor/seq/size.hpp>
#
/* An empty seq is one that is just AUTOBOOST_PP_SEQ_NIL */
#
# define AUTOBOOST_PP_SEQ_DETAIL_IS_EMPTY(seq) \
	AUTOBOOST_PP_COMPL \
		( \
		AUTOBOOST_PP_SEQ_DETAIL_IS_NOT_EMPTY(seq) \
		) \
/**/
#
# define AUTOBOOST_PP_SEQ_DETAIL_IS_EMPTY_SIZE(size) \
	AUTOBOOST_PP_COMPL \
		( \
		AUTOBOOST_PP_SEQ_DETAIL_IS_NOT_EMPTY_SIZE(size) \
		) \
/**/
#
# define AUTOBOOST_PP_SEQ_DETAIL_IS_NOT_EMPTY(seq) \
	AUTOBOOST_PP_SEQ_DETAIL_IS_NOT_EMPTY_SIZE(AUTOBOOST_PP_SEQ_DETAIL_EMPTY_SIZE(seq)) \
/**/
#
# define AUTOBOOST_PP_SEQ_DETAIL_IS_NOT_EMPTY_SIZE(size) \
	AUTOBOOST_PP_BOOL(size) \
/**/
#
# define AUTOBOOST_PP_SEQ_DETAIL_EMPTY_SIZE(seq) \
	AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_SIZE(seq (nil))) \
/**/
#
# endif

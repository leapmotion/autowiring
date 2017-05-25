# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_TO_LIST_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_TO_LIST_HPP
#
# include <autoboost/preprocessor/punctuation/comma.hpp>
# include <autoboost/preprocessor/punctuation/paren.hpp>
# include <autoboost/preprocessor/seq/detail/binary_transform.hpp>
#
# /* AUTOBOOST_PP_SEQ_TO_LIST */
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
# include <autoboost/preprocessor/seq/size.hpp>
# include <autoboost/preprocessor/seq/detail/to_list_msvc.hpp>
# define AUTOBOOST_PP_SEQ_TO_LIST(seq) \
    AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC \
        ( \
        AUTOBOOST_PP_SEQ_TO_LIST_I(AUTOBOOST_PP_SEQ_BINARY_TRANSFORM(seq)), \
        AUTOBOOST_PP_SEQ_SIZE(seq) \
        ) \
/**/
# else
# define AUTOBOOST_PP_SEQ_TO_LIST(seq) AUTOBOOST_PP_SEQ_TO_LIST_I(AUTOBOOST_PP_SEQ_BINARY_TRANSFORM(seq))
# endif
# define AUTOBOOST_PP_SEQ_TO_LIST_I(bseq) AUTOBOOST_PP_SEQ_TO_LIST_A bseq AUTOBOOST_PP_NIL AUTOBOOST_PP_SEQ_TO_LIST_B bseq
# define AUTOBOOST_PP_SEQ_TO_LIST_A(m, e) m(AUTOBOOST_PP_LPAREN() e AUTOBOOST_PP_COMMA() AUTOBOOST_PP_SEQ_TO_LIST_A_ID)
# define AUTOBOOST_PP_SEQ_TO_LIST_A_ID() AUTOBOOST_PP_SEQ_TO_LIST_A
# define AUTOBOOST_PP_SEQ_TO_LIST_B(m, e) m(AUTOBOOST_PP_RPAREN() AUTOBOOST_PP_SEQ_TO_LIST_B_ID)
# define AUTOBOOST_PP_SEQ_TO_LIST_B_ID() AUTOBOOST_PP_SEQ_TO_LIST_B
#
# endif

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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_SUBSEQ_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_SUBSEQ_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/seq/first_n.hpp>
# include <autoboost/preprocessor/seq/rest_n.hpp>
#
# /* AUTOBOOST_PP_SEQ_SUBSEQ */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_SUBSEQ(seq, i, len) AUTOBOOST_PP_SEQ_FIRST_N(len, AUTOBOOST_PP_SEQ_REST_N(i, seq))
# else
#    define AUTOBOOST_PP_SEQ_SUBSEQ(seq, i, len) AUTOBOOST_PP_SEQ_SUBSEQ_I(seq, i, len)
#    define AUTOBOOST_PP_SEQ_SUBSEQ_I(seq, i, len) AUTOBOOST_PP_SEQ_FIRST_N(len, AUTOBOOST_PP_SEQ_REST_N(i, seq))
# endif
#
# endif

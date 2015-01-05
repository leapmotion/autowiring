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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_INSERT_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_INSERT_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/seq/first_n.hpp>
# include <autoboost/preprocessor/seq/rest_n.hpp>
#
# /* AUTOBOOST_PP_SEQ_INSERT */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_INSERT(seq, i, elem) AUTOBOOST_PP_SEQ_FIRST_N(i, seq) (elem) AUTOBOOST_PP_SEQ_REST_N(i, seq)
# else
#    define AUTOBOOST_PP_SEQ_INSERT(seq, i, elem) AUTOBOOST_PP_SEQ_INSERT_I(seq, i, elem)
#    define AUTOBOOST_PP_SEQ_INSERT_I(seq, i, elem) AUTOBOOST_PP_SEQ_FIRST_N(i, seq) (elem) AUTOBOOST_PP_SEQ_REST_N(i, seq)
# endif
#
# endif

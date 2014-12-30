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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_REMOVE_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_REMOVE_HPP
#
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/seq/first_n.hpp>
# include <autoboost/preprocessor/seq/rest_n.hpp>
#
# /* AUTOBOOST_PP_SEQ_REMOVE */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_REMOVE(seq, i) AUTOBOOST_PP_SEQ_FIRST_N(i, seq) AUTOBOOST_PP_SEQ_REST_N(AUTOBOOST_PP_INC(i), seq)
# else
#    define AUTOBOOST_PP_SEQ_REMOVE(seq, i) AUTOBOOST_PP_SEQ_REMOVE_I(seq, i)
#    define AUTOBOOST_PP_SEQ_REMOVE_I(seq, i) AUTOBOOST_PP_SEQ_FIRST_N(i, seq) AUTOBOOST_PP_SEQ_REST_N(AUTOBOOST_PP_INC(i), seq)
# endif
#
# endif

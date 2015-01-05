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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_POP_BACK_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_POP_BACK_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/seq/first_n.hpp>
# include <autoboost/preprocessor/seq/size.hpp>
#
# /* AUTOBOOST_PP_SEQ_POP_BACK */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_POP_BACK(seq) AUTOBOOST_PP_SEQ_FIRST_N(AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_SIZE(seq)), seq)
# else
#    define AUTOBOOST_PP_SEQ_POP_BACK(seq) AUTOBOOST_PP_SEQ_POP_BACK_I(seq)
#    define AUTOBOOST_PP_SEQ_POP_BACK_I(seq) AUTOBOOST_PP_SEQ_FIRST_N(AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_SIZE(seq)), seq)
# endif
#
# endif

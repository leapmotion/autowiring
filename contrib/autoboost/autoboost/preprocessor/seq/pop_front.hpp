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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_POP_FRONT_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_POP_FRONT_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/seq/seq.hpp>
#
# /* AUTOBOOST_PP_SEQ_POP_FRONT */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_POP_FRONT(seq) AUTOBOOST_PP_SEQ_TAIL(seq)
# else
#    define AUTOBOOST_PP_SEQ_POP_FRONT(seq) AUTOBOOST_PP_SEQ_POP_FRONT_I(seq)
#    define AUTOBOOST_PP_SEQ_POP_FRONT_I(seq) AUTOBOOST_PP_SEQ_TAIL(seq)
# endif
#
# endif

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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_TO_TUPLE_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_TO_TUPLE_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/seq/enum.hpp>
#
# /* AUTOBOOST_PP_SEQ_TO_TUPLE */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_TO_TUPLE(seq) (AUTOBOOST_PP_SEQ_ENUM(seq))
# else
#    define AUTOBOOST_PP_SEQ_TO_TUPLE(seq) AUTOBOOST_PP_SEQ_TO_TUPLE_I(seq)
#    define AUTOBOOST_PP_SEQ_TO_TUPLE_I(seq) (AUTOBOOST_PP_SEQ_ENUM(seq))
# endif
#
# endif

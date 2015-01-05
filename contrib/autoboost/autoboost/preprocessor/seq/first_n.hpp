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
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_FIRST_N_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_FIRST_N_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/if.hpp>
# include <autoboost/preprocessor/seq/detail/split.hpp>
# include <autoboost/preprocessor/tuple/eat.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_SEQ_FIRST_N */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_SEQ_FIRST_N(n, seq) AUTOBOOST_PP_IF(n, AUTOBOOST_PP_TUPLE_ELEM, AUTOBOOST_PP_TUPLE_EAT_3)(2, 0, AUTOBOOST_PP_SEQ_SPLIT(n, seq (nil)))
# else
#    define AUTOBOOST_PP_SEQ_FIRST_N(n, seq) AUTOBOOST_PP_SEQ_FIRST_N_I(n, seq)
#    define AUTOBOOST_PP_SEQ_FIRST_N_I(n, seq) AUTOBOOST_PP_IF(n, AUTOBOOST_PP_TUPLE_ELEM, AUTOBOOST_PP_TUPLE_EAT_3)(2, 0, AUTOBOOST_PP_SEQ_SPLIT(n, seq (nil)))
# endif
#
# endif

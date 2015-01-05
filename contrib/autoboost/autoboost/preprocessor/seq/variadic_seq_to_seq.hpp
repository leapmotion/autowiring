# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2012.                                  *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_VARIADIC_SEQ_TO_SEQ_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_VARIADIC_SEQ_TO_SEQ_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_VARIADIC_SEQ_TO_SEQ */
#
# if AUTOBOOST_PP_VARIADICS
#    define AUTOBOOST_PP_VARIADIC_SEQ_TO_SEQ(vseq) AUTOBOOST_PP_CAT(AUTOBOOST_PP_VARIADIC_SEQ_TO_SEQ_A vseq, 0)
#    define AUTOBOOST_PP_VARIADIC_SEQ_TO_SEQ_A(...) ((__VA_ARGS__)) AUTOBOOST_PP_VARIADIC_SEQ_TO_SEQ_B
#    define AUTOBOOST_PP_VARIADIC_SEQ_TO_SEQ_B(...) ((__VA_ARGS__)) AUTOBOOST_PP_VARIADIC_SEQ_TO_SEQ_A
#    define AUTOBOOST_PP_VARIADIC_SEQ_TO_SEQ_A0
#    define AUTOBOOST_PP_VARIADIC_SEQ_TO_SEQ_B0
# endif
#
# endif

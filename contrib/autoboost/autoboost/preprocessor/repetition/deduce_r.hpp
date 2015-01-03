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
# ifndef AUTOBOOST_PREPROCESSOR_REPETITION_DEDUCE_R_HPP
# define AUTOBOOST_PREPROCESSOR_REPETITION_DEDUCE_R_HPP
#
# include <autoboost/preprocessor/detail/auto_rec.hpp>
# include <autoboost/preprocessor/repetition/for.hpp>
#
# /* AUTOBOOST_PP_DEDUCE_R */
#
# define AUTOBOOST_PP_DEDUCE_R() AUTOBOOST_PP_AUTO_REC(AUTOBOOST_PP_FOR_P, 256)
#
# endif

# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2011.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* Revised by Paul Mensonides (2011) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_LIST_TO_SEQ_HPP
# define AUTOBOOST_PREPROCESSOR_LIST_TO_SEQ_HPP
#
# include <autoboost/preprocessor/list/for_each.hpp>
#
# /* AUTOBOOST_PP_LIST_TO_SEQ */
#
# define AUTOBOOST_PP_LIST_TO_SEQ(list) \
    AUTOBOOST_PP_LIST_FOR_EACH(AUTOBOOST_PP_LIST_TO_SEQ_MACRO, ~, list) \
    /**/
# define AUTOBOOST_PP_LIST_TO_SEQ_MACRO(r, data, elem) (elem)
#
# /* AUTOBOOST_PP_LIST_TO_SEQ_R */
#
# define AUTOBOOST_PP_LIST_TO_SEQ_R(r, list) \
    AUTOBOOST_PP_LIST_FOR_EACH_R(r, AUTOBOOST_PP_LIST_TO_SEQ_MACRO, ~, list) \
    /**/
#
# endif /* AUTOBOOST_PREPROCESSOR_LIST_TO_SEQ_HPP */

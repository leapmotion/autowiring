# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2003.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_FACILITIES_IS_1_HPP
# define AUTOBOOST_PREPROCESSOR_FACILITIES_IS_1_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/facilities/is_empty.hpp>
#
# /* AUTOBOOST_PP_IS_1 */
#
# define AUTOBOOST_PP_IS_1(x) AUTOBOOST_PP_IS_EMPTY(AUTOBOOST_PP_CAT(AUTOBOOST_PP_IS_1_HELPER_, x))
# define AUTOBOOST_PP_IS_1_HELPER_1
#
# endif

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
# ifndef AUTOBOOST_PREPROCESSOR_ITERATION_LOCAL_HPP
# define AUTOBOOST_PREPROCESSOR_ITERATION_LOCAL_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/slot/slot.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_LOCAL_ITERATE */
#
# define AUTOBOOST_PP_LOCAL_ITERATE() <autoboost/preprocessor/iteration/detail/local.hpp>
#
# define AUTOBOOST_PP_LOCAL_C(n) (AUTOBOOST_PP_LOCAL_S) <= n && (AUTOBOOST_PP_LOCAL_F) >= n
# define AUTOBOOST_PP_LOCAL_R(n) (AUTOBOOST_PP_LOCAL_F) <= n && (AUTOBOOST_PP_LOCAL_S) >= n
#
# endif

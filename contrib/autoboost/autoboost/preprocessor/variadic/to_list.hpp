# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_VARIADIC_TO_LIST_HPP
# define AUTOBOOST_PREPROCESSOR_VARIADIC_TO_LIST_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/tuple/to_list.hpp>
#
# /* AUTOBOOST_PP_VARIADIC_TO_LIST */
#
# if AUTOBOOST_PP_VARIADICS
#    define AUTOBOOST_PP_VARIADIC_TO_LIST(...) AUTOBOOST_PP_TUPLE_TO_LIST((__VA_ARGS__))
# endif
#
# endif

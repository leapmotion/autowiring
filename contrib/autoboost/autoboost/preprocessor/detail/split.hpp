# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# ifndef AUTOBOOST_PREPROCESSOR_DETAIL_SPLIT_HPP
# define AUTOBOOST_PREPROCESSOR_DETAIL_SPLIT_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_SPLIT */
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_SPLIT(n, im) AUTOBOOST_PP_SPLIT_I((n, im))
#    define AUTOBOOST_PP_SPLIT_I(par) AUTOBOOST_PP_SPLIT_II ## par
#    define AUTOBOOST_PP_SPLIT_II(n, a, b) AUTOBOOST_PP_SPLIT_ ## n(a, b)
# elif AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_SPLIT(n, im) AUTOBOOST_PP_SPLIT_I(n((im)))
#    define AUTOBOOST_PP_SPLIT_I(n) AUTOBOOST_PP_SPLIT_ID(AUTOBOOST_PP_SPLIT_II_ ## n)
#    define AUTOBOOST_PP_SPLIT_II_0(s) AUTOBOOST_PP_SPLIT_ID(AUTOBOOST_PP_SPLIT_0 s)
#    define AUTOBOOST_PP_SPLIT_II_1(s) AUTOBOOST_PP_SPLIT_ID(AUTOBOOST_PP_SPLIT_1 s)
#    define AUTOBOOST_PP_SPLIT_ID(id) id
# else
#    define AUTOBOOST_PP_SPLIT(n, im) AUTOBOOST_PP_SPLIT_I(n)(im)
#    define AUTOBOOST_PP_SPLIT_I(n) AUTOBOOST_PP_SPLIT_ ## n
# endif
#
# define AUTOBOOST_PP_SPLIT_0(a, b) a
# define AUTOBOOST_PP_SPLIT_1(a, b) b
#
# endif

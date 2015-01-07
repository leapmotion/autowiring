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
# ifndef AUTOBOOST_PREPROCESSOR_CONTROL_IIF_HPP
# define AUTOBOOST_PREPROCESSOR_CONTROL_IIF_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_IIF(bit, t, f) AUTOBOOST_PP_IIF_I(bit, t, f)
# else
#    define AUTOBOOST_PP_IIF(bit, t, f) AUTOBOOST_PP_IIF_OO((bit, t, f))
#    define AUTOBOOST_PP_IIF_OO(par) AUTOBOOST_PP_IIF_I ## par
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_IIF_I(bit, t, f) AUTOBOOST_PP_IIF_ ## bit(t, f)
# else
#    define AUTOBOOST_PP_IIF_I(bit, t, f) AUTOBOOST_PP_IIF_II(AUTOBOOST_PP_IIF_ ## bit(t, f))
#    define AUTOBOOST_PP_IIF_II(id) id
# endif
#
# define AUTOBOOST_PP_IIF_0(t, f) f
# define AUTOBOOST_PP_IIF_1(t, f) t
#
# endif

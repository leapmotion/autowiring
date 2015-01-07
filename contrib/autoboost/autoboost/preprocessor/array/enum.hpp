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
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_ENUM_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_ENUM_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
#
# /* AUTOBOOST_PP_ARRAY_ENUM */
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_ARRAY_ENUM(array) AUTOBOOST_PP_ARRAY_ENUM_I(AUTOBOOST_PP_TUPLE_REM_CTOR, array)
#    define AUTOBOOST_PP_ARRAY_ENUM_I(m, args) AUTOBOOST_PP_ARRAY_ENUM_II(m, args)
#    define AUTOBOOST_PP_ARRAY_ENUM_II(m, args) AUTOBOOST_PP_CAT(m ## args,)
# elif AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_ARRAY_ENUM(array) AUTOBOOST_PP_ARRAY_ENUM_I(array)
#    define AUTOBOOST_PP_ARRAY_ENUM_I(array) AUTOBOOST_PP_TUPLE_REM_CTOR ## array
# else
#    define AUTOBOOST_PP_ARRAY_ENUM(array) AUTOBOOST_PP_TUPLE_REM_CTOR array
# endif
#
# endif

# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_DETAIL_BINARY_TRANSFORM_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_DETAIL_BINARY_TRANSFORM_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/tuple/eat.hpp>
# include <autoboost/preprocessor/tuple/rem.hpp>
# include <autoboost/preprocessor/variadic/detail/is_single_return.hpp>
#
# /* AUTOBOOST_PP_SEQ_BINARY_TRANSFORM */
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM(seq) AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_I(, seq)
#    define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_I(p, seq) AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_II(p ## seq)
#    define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_II(seq) AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_III(seq)
#    define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_III(seq) AUTOBOOST_PP_CAT(AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_A seq, 0)
# else
#    define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM(seq) AUTOBOOST_PP_CAT(AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_A seq, 0)
# endif
# if AUTOBOOST_PP_VARIADICS
#    if AUTOBOOST_PP_VARIADICS_MSVC
#		define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_REM(data) data
#       define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_A(...) (AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_REM, __VA_ARGS__)() AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_B
#       define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_B(...) (AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_REM, __VA_ARGS__)() AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_A
#	 else
#       define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_A(...) (AUTOBOOST_PP_REM, __VA_ARGS__)() AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_B
#       define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_B(...) (AUTOBOOST_PP_REM, __VA_ARGS__)() AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_A
#	 endif
# else
#    define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_A(e) (AUTOBOOST_PP_REM, e)() AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_B
#    define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_B(e) (AUTOBOOST_PP_REM, e)() AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_A
# endif
# define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_A0 (AUTOBOOST_PP_EAT, ?)
# define AUTOBOOST_PP_SEQ_BINARY_TRANSFORM_B0 (AUTOBOOST_PP_EAT, ?)
#
# endif

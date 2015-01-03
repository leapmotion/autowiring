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
# ifndef AUTOBOOST_PREPROCESSOR_DEBUG_ERROR_HPP
# define AUTOBOOST_PREPROCESSOR_DEBUG_ERROR_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_ERROR */
#
# if AUTOBOOST_PP_CONFIG_ERRORS
#    define AUTOBOOST_PP_ERROR(code) AUTOBOOST_PP_CAT(AUTOBOOST_PP_ERROR_, code)
# endif
#
# define AUTOBOOST_PP_ERROR_0x0000 AUTOBOOST_PP_ERROR(0x0000, AUTOBOOST_PP_INDEX_OUT_OF_BOUNDS)
# define AUTOBOOST_PP_ERROR_0x0001 AUTOBOOST_PP_ERROR(0x0001, AUTOBOOST_PP_WHILE_OVERFLOW)
# define AUTOBOOST_PP_ERROR_0x0002 AUTOBOOST_PP_ERROR(0x0002, AUTOBOOST_PP_FOR_OVERFLOW)
# define AUTOBOOST_PP_ERROR_0x0003 AUTOBOOST_PP_ERROR(0x0003, AUTOBOOST_PP_REPEAT_OVERFLOW)
# define AUTOBOOST_PP_ERROR_0x0004 AUTOBOOST_PP_ERROR(0x0004, AUTOBOOST_PP_LIST_FOLD_OVERFLOW)
# define AUTOBOOST_PP_ERROR_0x0005 AUTOBOOST_PP_ERROR(0x0005, AUTOBOOST_PP_SEQ_FOLD_OVERFLOW)
# define AUTOBOOST_PP_ERROR_0x0006 AUTOBOOST_PP_ERROR(0x0006, AUTOBOOST_PP_ARITHMETIC_OVERFLOW)
# define AUTOBOOST_PP_ERROR_0x0007 AUTOBOOST_PP_ERROR(0x0007, AUTOBOOST_PP_DIVISION_BY_ZERO)
#
# endif

# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2014.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
#ifndef AUTOBOOST_PREPROCESSOR_DETAIL_IS_EMPTY_HPP
#define AUTOBOOST_PREPROCESSOR_DETAIL_IS_EMPTY_HPP

#include <autoboost/preprocessor/punctuation/is_begin_parens.hpp>

#if AUTOBOOST_PP_VARIADICS_MSVC

# pragma warning(once:4002)

#define AUTOBOOST_PP_DETAIL_IS_EMPTY_IIF_0(t, b) b
#define AUTOBOOST_PP_DETAIL_IS_EMPTY_IIF_1(t, b) t

#else

#define AUTOBOOST_PP_DETAIL_IS_EMPTY_IIF_0(t, ...) __VA_ARGS__
#define AUTOBOOST_PP_DETAIL_IS_EMPTY_IIF_1(t, ...) t

#endif

#if AUTOBOOST_PP_VARIADICS_MSVC && _MSC_VER <= 1400

#define AUTOBOOST_PP_DETAIL_IS_EMPTY_PROCESS(param) \
	AUTOBOOST_PP_IS_BEGIN_PARENS \
    	( \
        AUTOBOOST_PP_DETAIL_IS_EMPTY_NON_FUNCTION_C param () \
        ) \
/**/

#else

#define AUTOBOOST_PP_DETAIL_IS_EMPTY_PROCESS(...) \
	AUTOBOOST_PP_IS_BEGIN_PARENS \
        ( \
        AUTOBOOST_PP_DETAIL_IS_EMPTY_NON_FUNCTION_C __VA_ARGS__ () \
        ) \
/**/

#endif

#define AUTOBOOST_PP_DETAIL_IS_EMPTY_PRIMITIVE_CAT(a, b) a ## b
#define AUTOBOOST_PP_DETAIL_IS_EMPTY_IIF(bit) AUTOBOOST_PP_DETAIL_IS_EMPTY_PRIMITIVE_CAT(AUTOBOOST_PP_DETAIL_IS_EMPTY_IIF_,bit)
#define AUTOBOOST_PP_DETAIL_IS_EMPTY_NON_FUNCTION_C(...) ()

#endif /* AUTOBOOST_PREPROCESSOR_DETAIL_IS_EMPTY_HPP */

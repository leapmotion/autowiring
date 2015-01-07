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
# ifndef AUTOBOOST_PREPROCESSOR_FACILITIES_IS_EMPTY_VARIADIC_HPP
# define AUTOBOOST_PREPROCESSOR_FACILITIES_IS_EMPTY_VARIADIC_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# if AUTOBOOST_PP_VARIADICS
#
# include <autoboost/preprocessor/punctuation/is_begin_parens.hpp>
# include <autoboost/preprocessor/facilities/detail/is_empty.hpp>
#
#if AUTOBOOST_PP_VARIADICS_MSVC && _MSC_VER <= 1400
#
#define AUTOBOOST_PP_IS_EMPTY(param) \
    AUTOBOOST_PP_DETAIL_IS_EMPTY_IIF \
      ( \
      AUTOBOOST_PP_IS_BEGIN_PARENS \
        ( \
        param \
        ) \
      ) \
      ( \
      AUTOBOOST_PP_IS_EMPTY_ZERO, \
      AUTOBOOST_PP_DETAIL_IS_EMPTY_PROCESS \
      ) \
    (param) \
/**/
#define AUTOBOOST_PP_IS_EMPTY_ZERO(param) 0
# else
#define AUTOBOOST_PP_IS_EMPTY(...) \
    AUTOBOOST_PP_DETAIL_IS_EMPTY_IIF \
      ( \
      AUTOBOOST_PP_IS_BEGIN_PARENS \
        ( \
        __VA_ARGS__ \
        ) \
      ) \
      ( \
      AUTOBOOST_PP_IS_EMPTY_ZERO, \
      AUTOBOOST_PP_DETAIL_IS_EMPTY_PROCESS \
      ) \
    (__VA_ARGS__) \
/**/
#define AUTOBOOST_PP_IS_EMPTY_ZERO(...) 0
# endif /* AUTOBOOST_PP_VARIADICS_MSVC && _MSC_VER <= 1400 */
# endif /* AUTOBOOST_PP_VARIADICS */
# endif /* AUTOBOOST_PREPROCESSOR_FACILITIES_IS_EMPTY_VARIADIC_HPP */

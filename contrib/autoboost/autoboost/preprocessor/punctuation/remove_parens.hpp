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
#ifndef AUTOBOOST_PREPROCESSOR_REMOVE_PARENS_HPP
#define AUTOBOOST_PREPROCESSOR_REMOVE_PARENS_HPP

#include <autoboost/preprocessor/config/config.hpp>

#if AUTOBOOST_PP_VARIADICS

#include <autoboost/preprocessor/control/iif.hpp>
#include <autoboost/preprocessor/facilities/identity.hpp>
#include <autoboost/preprocessor/punctuation/is_begin_parens.hpp>
#include <autoboost/preprocessor/tuple/enum.hpp>

#define AUTOBOOST_PP_REMOVE_PARENS(param) \
    AUTOBOOST_PP_IIF \
      ( \
      AUTOBOOST_PP_IS_BEGIN_PARENS(param), \
      AUTOBOOST_PP_REMOVE_PARENS_DO, \
      AUTOBOOST_PP_IDENTITY \
      ) \
    (param)() \
/**/

#define AUTOBOOST_PP_REMOVE_PARENS_DO(param) \
  AUTOBOOST_PP_IDENTITY(AUTOBOOST_PP_TUPLE_ENUM(param)) \
/**/

#endif /* AUTOBOOST_PP_VARIADICS */
#endif /* AUTOBOOST_PREPROCESSOR_REMOVE_PARENS_HPP */

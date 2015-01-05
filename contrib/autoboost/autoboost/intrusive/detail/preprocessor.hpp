//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTRUSIVE_DETAIL_PREPROCESSOR_HPP
#define AUTOBOOST_INTRUSIVE_DETAIL_PREPROCESSOR_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/intrusive/detail/config_begin.hpp>
#include <autoboost/intrusive/detail/workaround.hpp>

#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/repetition/enum.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing.hpp>
#include <autoboost/preprocessor/arithmetic/sub.hpp>


#define AUTOBOOST_INTRUSIVE_MAX_CONSTRUCTOR_PARAMETERS 10

#define AUTOBOOST_INTRUSIVE_PP_IDENTITY(z, n, data) data

#define AUTOBOOST_INTRUSIVE_PP_DECLVAL(z, n, data) \
autoboost::move_detail::declval< AUTOBOOST_PP_CAT(P, n) >() \
//!

#define AUTOBOOST_INTRUSIVE_PP_TEMPLATE_PARAM_VOID_DEFAULT(z, n, data)   \
  AUTOBOOST_PP_CAT(class P, n) = void                                      \
//!

#include <autoboost/intrusive/detail/config_end.hpp>

#endif //#ifndef AUTOBOOST_INTRUSIVE_DETAIL_PREPROCESSOR_HPP

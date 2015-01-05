// Copyright (C) 2003, Fernando Luis Cacciola Carballal.
// Copyright (C) 2007, Tobias Schwinger.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/optional for documentation.
//
// You are welcome to contact the author at:
//  fernando_cacciola@hotmail.com
//
#ifndef AUTOBOOST_UTILITY_DETAIL_INPLACE_FACTORY_PREFIX_04APR2007_HPP
#define AUTOBOOST_UTILITY_DETAIL_INPLACE_FACTORY_PREFIX_04APR2007_HPP

#include <new>
#include <cstddef>
#include <autoboost/config.hpp>
#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/punctuation/paren.hpp>
#include <autoboost/preprocessor/iteration/iterate.hpp>
#include <autoboost/preprocessor/repetition/repeat.hpp>
#include <autoboost/preprocessor/repetition/enum.hpp>
#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/preprocessor/repetition/enum_binary_params.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing_params.hpp>

#define AUTOBOOST_DEFINE_INPLACE_FACTORY_CLASS_MEMBER_INIT(z,n,_) AUTOBOOST_PP_CAT(m_a,n) AUTOBOOST_PP_LPAREN() AUTOBOOST_PP_CAT(a,n) AUTOBOOST_PP_RPAREN()
#define AUTOBOOST_DEFINE_INPLACE_FACTORY_CLASS_MEMBER_DECL(z,n,_) AUTOBOOST_PP_CAT(A,n) const& AUTOBOOST_PP_CAT(m_a,n);

#define AUTOBOOST_MAX_INPLACE_FACTORY_ARITY 10

#undef AUTOBOOST_UTILITY_DETAIL_INPLACE_FACTORY_SUFFIX_04APR2007_HPP

#endif


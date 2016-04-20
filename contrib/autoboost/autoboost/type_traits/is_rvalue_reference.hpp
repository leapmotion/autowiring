
//  (C) John Maddock 2010.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_RVALUE_REFERENCE_HPP_INCLUDED
#define AUTOBOOST_TT_IS_RVALUE_REFERENCE_HPP_INCLUDED

#include <autoboost/type_traits/config.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_rvalue_reference,T,false)
#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_rvalue_reference,T&&,true)
#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_REFERENCE_HPP_INCLUDED


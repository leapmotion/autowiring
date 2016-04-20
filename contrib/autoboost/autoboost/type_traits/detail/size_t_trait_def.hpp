
// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// $Source$
// $Date$
// $Revision$

#include <autoboost/type_traits/detail/template_arity_spec.hpp>
#include <autoboost/type_traits/integral_constant.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>
#include <autoboost/mpl/size_t.hpp>

#include <cstddef>

// Obsolete. Remove.
#define AUTOBOOST_TT_AUX_SIZE_T_BASE(C) public ::autoboost::integral_constant<std::size_t,C>
#define AUTOBOOST_TT_AUX_SIZE_T_TRAIT_VALUE_DECL(C) /**/


#define AUTOBOOST_TT_AUX_SIZE_T_TRAIT_DEF1(trait,T,C) \
template< typename T > struct trait \
    : public ::autoboost::integral_constant<std::size_t,C> \
{ \
public:\
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,trait,(T)) \
}; \
\
AUTOBOOST_TT_AUX_TEMPLATE_ARITY_SPEC(1,trait) \
/**/

#define AUTOBOOST_TT_AUX_SIZE_T_TRAIT_SPEC1(trait,spec,C) \
template<> struct trait<spec> \
    : public ::autoboost::integral_constant<std::size_t,C> \
{ \
public:\
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(1,trait,(spec)) \
}; \
/**/

#define AUTOBOOST_TT_AUX_SIZE_T_TRAIT_PARTIAL_SPEC1_1(param,trait,spec,C) \
template< param > struct trait<spec> \
    : public ::autoboost::integral_constant<std::size_t,C> \
{ \
}; \
/**/

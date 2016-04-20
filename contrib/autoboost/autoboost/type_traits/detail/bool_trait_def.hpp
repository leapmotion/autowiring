
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
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>
#include <autoboost/config.hpp>

//
// Unfortunately some libraries have started using this header without
// cleaning up afterwards: so we'd better undef the macros just in case
// they've been defined already....
//
#ifdef AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL
#undef AUTOBOOST_TT_AUX_BOOL_C_BASE
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF2
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC2
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_2
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_1
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1
#undef AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1
#endif

#if defined(__SUNPRO_CC) && (__SUNPRO_CC < 0x570)
#   define AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL(C) \
    typedef ::autoboost::integral_constant<bool,C> type; \
    enum { value = type::value }; \
    /**/
#   define AUTOBOOST_TT_AUX_BOOL_C_BASE(C)
#endif

#ifndef AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL
#   define AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL(C) /**/
#endif

#ifndef AUTOBOOST_TT_AUX_BOOL_C_BASE
#   define AUTOBOOST_TT_AUX_BOOL_C_BASE(C) : public ::autoboost::integral_constant<bool,C>
#endif


#define AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(trait,T,C) \
template< typename T > struct trait \
    AUTOBOOST_TT_AUX_BOOL_C_BASE(C) \
{ \
public:\
    AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL(C) \
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,trait,(T)) \
}; \
\
AUTOBOOST_TT_AUX_TEMPLATE_ARITY_SPEC(1,trait) \
/**/


#define AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF2(trait,T1,T2,C) \
template< typename T1, typename T2 > struct trait \
    AUTOBOOST_TT_AUX_BOOL_C_BASE(C) \
{ \
public:\
    AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL(C) \
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(2,trait,(T1,T2)) \
}; \
\
AUTOBOOST_TT_AUX_TEMPLATE_ARITY_SPEC(2,trait) \
/**/

#define AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF3(trait,T1,T2,T3,C) \
template< typename T1, typename T2, typename T3 > struct trait \
    AUTOBOOST_TT_AUX_BOOL_C_BASE(C) \
{ \
public:\
    AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL(C) \
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(3,trait,(T1,T2,T3)) \
}; \
\
AUTOBOOST_TT_AUX_TEMPLATE_ARITY_SPEC(3,trait) \
/**/

#define AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(trait,sp,C) \
template<> struct trait< sp > \
    AUTOBOOST_TT_AUX_BOOL_C_BASE(C) \
{ \
public:\
    AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL(C) \
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(1,trait,(sp)) \
}; \
/**/

#define AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC2(trait,sp1,sp2,C) \
template<> struct trait< sp1,sp2 > \
    AUTOBOOST_TT_AUX_BOOL_C_BASE(C) \
{ \
public:\
    AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL(C) \
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(2,trait,(sp1,sp2)) \
}; \
/**/

#define AUTOBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(trait,sp,C) \
template<> struct trait##_impl< sp > \
{ \
public:\
    AUTOBOOST_STATIC_CONSTANT(bool, value = (C)); \
}; \
/**/

#define AUTOBOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,sp1,sp2,C) \
template<> struct trait##_impl< sp1,sp2 > \
{ \
public:\
    AUTOBOOST_STATIC_CONSTANT(bool, value = (C)); \
}; \
/**/

#define AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(param,trait,sp,C) \
template< param > struct trait< sp > \
    AUTOBOOST_TT_AUX_BOOL_C_BASE(C) \
{ \
public:\
    AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL(C) \
}; \
/**/

#define AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_2(param1,param2,trait,sp,C) \
template< param1, param2 > struct trait< sp > \
    AUTOBOOST_TT_AUX_BOOL_C_BASE(C) \
{ \
public:\
    AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL(C) \
}; \
/**/

#define AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_1(param,trait,sp1,sp2,C) \
template< param > struct trait< sp1,sp2 > \
    AUTOBOOST_TT_AUX_BOOL_C_BASE(C) \
{ \
public:\
    AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL(C) \
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(2,trait,(sp1,sp2)) \
}; \
/**/

#define AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(param1,param2,trait,sp1,sp2,C) \
template< param1, param2 > struct trait< sp1,sp2 > \
    AUTOBOOST_TT_AUX_BOOL_C_BASE(C) \
{ \
public:\
    AUTOBOOST_TT_AUX_BOOL_TRAIT_VALUE_DECL(C) \
}; \
/**/

#define AUTOBOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(param,trait,sp1,sp2,C) \
template< param > struct trait##_impl< sp1,sp2 > \
{ \
public:\
    AUTOBOOST_STATIC_CONSTANT(bool, value = (C)); \
}; \
/**/

#ifndef AUTOBOOST_NO_CV_SPECIALIZATIONS
#   define AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(trait,sp,value) \
    AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(trait,sp,value) \
    AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(trait,sp const,value) \
    AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(trait,sp volatile,value) \
    AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(trait,sp const volatile,value) \
    /**/
#else
#   define AUTOBOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(trait,sp,value) \
    AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(trait,sp,value) \
    /**/
#endif

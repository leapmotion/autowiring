
#ifndef AUTOBOOST_MPL_HAS_XXX_HPP_INCLUDED
#define AUTOBOOST_MPL_HAS_XXX_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2006
// Copyright David Abrahams 2002-2003
// Copyright Daniel Walker 2007
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/type_wrapper.hpp>
#include <autoboost/mpl/aux_/yes_no.hpp>
#include <autoboost/mpl/aux_/config/gcc.hpp>
#include <autoboost/mpl/aux_/config/has_xxx.hpp>
#include <autoboost/mpl/aux_/config/msvc_typename.hpp>
#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/static_constant.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#include <autoboost/preprocessor/array/elem.hpp>
#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/control/if.hpp>
#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing_params.hpp>

#if AUTOBOOST_WORKAROUND( __BORLANDC__, AUTOBOOST_TESTED_AT(0x590) )
# include <autoboost/type_traits/is_class.hpp>
#endif

#if !defined(AUTOBOOST_MPL_CFG_NO_HAS_XXX)

#   if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300)

// agurt, 11/sep/02: MSVC-specific version (< 7.1), based on a USENET
// newsgroup's posting by John Madsen (comp.lang.c++.moderated,
// 1999-11-12 19:17:06 GMT); the code is _not_ standard-conforming, but
// it works way more reliably than the SFINAE-based implementation

// Modified dwa 8/Oct/02 to handle reference types.

#   include <autoboost/mpl/if.hpp>
#   include <autoboost/mpl/bool.hpp>

namespace autoboost { namespace mpl { namespace aux {

struct has_xxx_tag;

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, == 1300)
template< typename U > struct msvc_incomplete_array
{
    typedef char (&type)[sizeof(U) + 1];
};
#endif

template< typename T >
struct msvc_is_incomplete
{
    // MSVC is capable of some kinds of SFINAE.  If U is an incomplete
    // type, it won't pick the second overload
    static char tester(...);

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, == 1300)
    template< typename U >
    static typename msvc_incomplete_array<U>::type tester(type_wrapper<U>);
#else
    template< typename U >
    static char (& tester(type_wrapper<U>) )[sizeof(U)+1];
#endif

    AUTOBOOST_STATIC_CONSTANT(bool, value =
          sizeof(tester(type_wrapper<T>())) == 1
        );
};

template<>
struct msvc_is_incomplete<int>
{
    AUTOBOOST_STATIC_CONSTANT(bool, value = false);
};

}}}

#   define AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF_(trait, name, default_) \
template< typename T, typename name = ::autoboost::mpl::aux::has_xxx_tag > \
struct AUTOBOOST_PP_CAT(trait,_impl) : T \
{ \
    static autoboost::mpl::aux::no_tag \
    test(void(*)(::autoboost::mpl::aux::has_xxx_tag)); \
    \
    static autoboost::mpl::aux::yes_tag test(...); \
    \
    AUTOBOOST_STATIC_CONSTANT(bool, value = \
          sizeof(test(static_cast<void(*)(name)>(0))) \
            != sizeof(autoboost::mpl::aux::no_tag) \
        ); \
    typedef autoboost::mpl::bool_<value> type; \
}; \
\
template< typename T, typename fallback_ = autoboost::mpl::bool_<default_> > \
struct trait \
    : autoboost::mpl::if_c< \
          autoboost::mpl::aux::msvc_is_incomplete<T>::value \
        , autoboost::mpl::bool_<false> \
        , AUTOBOOST_PP_CAT(trait,_impl)<T> \
        >::type \
{ \
}; \
\
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, void) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, bool) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, char) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, signed char) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, unsigned char) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, signed short) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, unsigned short) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, signed int) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, unsigned int) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, signed long) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, unsigned long) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, float) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, double) \
AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, long double) \
/**/

#   define AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, T) \
template<> struct trait<T> \
{ \
    AUTOBOOST_STATIC_CONSTANT(bool, value = false); \
    typedef autoboost::mpl::bool_<false> type; \
}; \
/**/

#if !defined(AUTOBOOST_NO_INTRINSIC_WCHAR_T)
#   define AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, unused) \
    AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF_(trait, name, unused) \
    AUTOBOOST_MPL_AUX_HAS_XXX_TRAIT_SPEC(trait, wchar_t) \
/**/
#else
#   define AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, unused) \
    AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF_(trait, name, unused) \
/**/
#endif


// SFINAE-based implementations below are derived from a USENET newsgroup's
// posting by Rani Sharoni (comp.lang.c++.moderated, 2002-03-17 07:45:09 PST)

#   elif AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1400) \
      || (AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, AUTOBOOST_TESTED_AT(1800)) && defined(__CUDACC__)) \
      || AUTOBOOST_WORKAROUND(__IBMCPP__, <= 700)

// MSVC 7.1 & MSVC 8.0 & VACPP

// agurt, 15/jun/05: replace overload-based SFINAE implementation with SFINAE
// applied to partial specialization to fix some apparently random failures
// (thanks to Daniel Wallin for researching this!)

#   define AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, default_) \
template< typename T > \
struct AUTOBOOST_PP_CAT(trait, _msvc_sfinae_helper) \
{ \
    typedef void type; \
};\
\
template< typename T, typename U = void > \
struct AUTOBOOST_PP_CAT(trait,_impl_) \
{ \
    AUTOBOOST_STATIC_CONSTANT(bool, value = false); \
    typedef autoboost::mpl::bool_<value> type; \
}; \
\
template< typename T > \
struct AUTOBOOST_PP_CAT(trait,_impl_)< \
      T \
    , typename AUTOBOOST_PP_CAT(trait, _msvc_sfinae_helper)< typename T::name >::type \
    > \
{ \
    AUTOBOOST_STATIC_CONSTANT(bool, value = true); \
    typedef autoboost::mpl::bool_<value> type; \
}; \
\
template< typename T, typename fallback_ = autoboost::mpl::bool_<default_> > \
struct trait \
    : AUTOBOOST_PP_CAT(trait,_impl_)<T> \
{ \
}; \
/**/

#   elif AUTOBOOST_WORKAROUND( __BORLANDC__, AUTOBOOST_TESTED_AT(0x590) )

#   define AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_BCB_DEF(trait, trait_tester, name, default_) \
template< typename T, bool IS_CLASS > \
struct trait_tester \
{ \
    AUTOBOOST_STATIC_CONSTANT( bool,  value = false ); \
}; \
template< typename T > \
struct trait_tester< T, true > \
{ \
    struct trait_tester_impl \
    { \
        template < class U > \
        static int  resolve( autoboost::mpl::aux::type_wrapper<U> const volatile * \
                           , autoboost::mpl::aux::type_wrapper<typename U::name >* = 0 ); \
        static char resolve( ... ); \
    }; \
    typedef autoboost::mpl::aux::type_wrapper<T> t_; \
    AUTOBOOST_STATIC_CONSTANT( bool, value = ( sizeof( trait_tester_impl::resolve( static_cast< t_ * >(0) ) ) == sizeof(int) ) ); \
}; \
template< typename T, typename fallback_ = autoboost::mpl::bool_<default_> > \
struct trait           \
{                      \
    AUTOBOOST_STATIC_CONSTANT( bool, value = (trait_tester< T, autoboost::is_class< T >::value >::value) );     \
    typedef autoboost::mpl::bool_< trait< T, fallback_ >::value > type; \
};

#   define AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, default_) \
    AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_BCB_DEF( trait \
                                         , AUTOBOOST_PP_CAT(trait,_tester)      \
                                         , name       \
                                         , default_ ) \
/**/

#   else // other SFINAE-capable compilers

#   define AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, default_) \
template< typename T, typename fallback_ = autoboost::mpl::bool_<default_> > \
struct trait \
{ \
    struct gcc_3_2_wknd \
    { \
        template< typename U > \
        static autoboost::mpl::aux::yes_tag test( \
              autoboost::mpl::aux::type_wrapper<U> const volatile* \
            , autoboost::mpl::aux::type_wrapper<AUTOBOOST_MSVC_TYPENAME U::name>* = 0 \
            ); \
    \
        static autoboost::mpl::aux::no_tag test(...); \
    }; \
    \
    typedef autoboost::mpl::aux::type_wrapper<T> t_; \
    AUTOBOOST_STATIC_CONSTANT(bool, value = \
          sizeof(gcc_3_2_wknd::test(static_cast<t_*>(0))) \
            == sizeof(autoboost::mpl::aux::yes_tag) \
        ); \
    typedef autoboost::mpl::bool_<value> type; \
}; \
/**/

#   endif // AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300)


#else // AUTOBOOST_MPL_CFG_NO_HAS_XXX

// placeholder implementation

#   define AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, default_) \
template< typename T, typename fallback_ = autoboost::mpl::bool_<default_> > \
struct trait \
{ \
    AUTOBOOST_STATIC_CONSTANT(bool, value = fallback_::value); \
    typedef fallback_ type; \
}; \
/**/

#endif

#define AUTOBOOST_MPL_HAS_XXX_TRAIT_DEF(name) \
    AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(AUTOBOOST_PP_CAT(has_,name), name, false) \
/**/


#if !defined(AUTOBOOST_MPL_CFG_NO_HAS_XXX_TEMPLATE)

// Create a boolean Metafunction to detect a nested template
// member. This implementation is based on a USENET newsgroup's
// posting by Aleksey Gurtovoy (comp.lang.c++.moderated, 2002-03-19),
// Rani Sharoni's USENET posting cited above, the non-template has_xxx
// implementations above, and discussion on the Boost mailing list.

#   if !defined(AUTOBOOST_MPL_HAS_XXX_NO_WRAPPED_TYPES)
#     if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1400)
#       define AUTOBOOST_MPL_HAS_XXX_NO_WRAPPED_TYPES 1
#     else
#       define AUTOBOOST_MPL_HAS_XXX_NO_WRAPPED_TYPES 0
#     endif
#   endif

#   if !defined(AUTOBOOST_MPL_HAS_XXX_NO_EXPLICIT_TEST_FUNCTION)
#     if (defined(AUTOBOOST_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS))
#       define AUTOBOOST_MPL_HAS_XXX_NO_EXPLICIT_TEST_FUNCTION 1
#     else
#       define AUTOBOOST_MPL_HAS_XXX_NO_EXPLICIT_TEST_FUNCTION 0
#     endif
#   endif

#   if !defined(AUTOBOOST_MPL_HAS_XXX_NEEDS_TEMPLATE_SFINAE)
#     if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1400)
#       define AUTOBOOST_MPL_HAS_XXX_NEEDS_TEMPLATE_SFINAE 1
#     else
#       define AUTOBOOST_MPL_HAS_XXX_NEEDS_TEMPLATE_SFINAE 0
#     endif
#   endif

// NOTE: Many internal implementation macros take a Boost.Preprocessor
// array argument called args which is of the following form.
//           ( 4, ( trait, name, max_arity, default_ ) )

#   define AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args) \
      AUTOBOOST_PP_CAT(AUTOBOOST_PP_ARRAY_ELEM(0, args) , _introspect) \
    /**/

#   define AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) \
      AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(AUTOBOOST_PP_ARRAY_ELEM(0, args) , _substitute), n) \
    /**/

#   define AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args) \
      AUTOBOOST_PP_CAT(AUTOBOOST_PP_ARRAY_ELEM(0, args) , _test) \
    /**/

// Thanks to Guillaume Melquiond for pointing out the need for the
// "substitute" template as an argument to the overloaded test
// functions to get SFINAE to work for member templates with the
// correct name but different number of arguments.
#   define AUTOBOOST_MPL_HAS_MEMBER_MULTI_SUBSTITUTE(z, n, args) \
      template< \
          template< AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_INC(n), typename V) > class V \
       > \
      struct AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) { \
      }; \
    /**/

#   define AUTOBOOST_MPL_HAS_MEMBER_SUBSTITUTE(args, substitute_macro) \
      AUTOBOOST_PP_REPEAT( \
          AUTOBOOST_PP_ARRAY_ELEM(2, args) \
        , AUTOBOOST_MPL_HAS_MEMBER_MULTI_SUBSTITUTE \
        , args \
      ) \
    /**/

#   if !AUTOBOOST_MPL_HAS_XXX_NO_EXPLICIT_TEST_FUNCTION
#     define AUTOBOOST_MPL_HAS_MEMBER_REJECT(args, member_macro) \
        template< typename V > \
        static autoboost::mpl::aux::no_tag \
        AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)(...); \
      /**/
#   else
#     define AUTOBOOST_MPL_HAS_MEMBER_REJECT(args, member_macro) \
        static autoboost::mpl::aux::no_tag \
        AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)(...); \
      /**/
#   endif

#   if !AUTOBOOST_MPL_HAS_XXX_NO_WRAPPED_TYPES
#     define AUTOBOOST_MPL_HAS_MEMBER_MULTI_ACCEPT(z, n, args) \
        template< typename V > \
        static autoboost::mpl::aux::yes_tag \
        AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)( \
            autoboost::mpl::aux::type_wrapper< V > const volatile* \
          , AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) < \
                V::template AUTOBOOST_PP_ARRAY_ELEM(1, args) \
            >* = 0 \
        ); \
      /**/
#     define AUTOBOOST_MPL_HAS_MEMBER_ACCEPT(args, member_macro) \
        AUTOBOOST_PP_REPEAT( \
            AUTOBOOST_PP_ARRAY_ELEM(2, args) \
          , AUTOBOOST_MPL_HAS_MEMBER_MULTI_ACCEPT \
          , args \
        ) \
      /**/
#   else
#     define AUTOBOOST_MPL_HAS_MEMBER_ACCEPT(args, member_macro) \
        template< typename V > \
        static autoboost::mpl::aux::yes_tag \
        AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)( \
            V const volatile* \
          , member_macro(args, V, T)* = 0 \
        ); \
      /**/
#   endif

#   if !AUTOBOOST_MPL_HAS_XXX_NO_EXPLICIT_TEST_FUNCTION
#     define AUTOBOOST_MPL_HAS_MEMBER_TEST(args) \
          sizeof(AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)< U >(0)) \
              == sizeof(autoboost::mpl::aux::yes_tag) \
      /**/
#   else
#     if !AUTOBOOST_MPL_HAS_XXX_NO_WRAPPED_TYPES
#       define AUTOBOOST_MPL_HAS_MEMBER_TEST(args) \
          sizeof( \
              AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)( \
                  static_cast< autoboost::mpl::aux::type_wrapper< U >* >(0) \
              ) \
          ) == sizeof(autoboost::mpl::aux::yes_tag) \
        /**/
#     else
#       define AUTOBOOST_MPL_HAS_MEMBER_TEST(args) \
          sizeof( \
              AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)( \
                  static_cast< U* >(0) \
              ) \
          ) == sizeof(autoboost::mpl::aux::yes_tag) \
        /**/
#     endif
#   endif

#   define AUTOBOOST_MPL_HAS_MEMBER_INTROSPECT( \
               args, substitute_macro, member_macro \
           ) \
      template< typename U > \
      struct AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args) { \
          AUTOBOOST_MPL_HAS_MEMBER_SUBSTITUTE(args, substitute_macro) \
          AUTOBOOST_MPL_HAS_MEMBER_REJECT(args, member_macro) \
          AUTOBOOST_MPL_HAS_MEMBER_ACCEPT(args, member_macro) \
          AUTOBOOST_STATIC_CONSTANT( \
              bool, value = AUTOBOOST_MPL_HAS_MEMBER_TEST(args) \
          ); \
          typedef autoboost::mpl::bool_< value > type; \
      }; \
    /**/

#   define AUTOBOOST_MPL_HAS_MEMBER_IMPLEMENTATION( \
               args, introspect_macro, substitute_macro, member_macro \
           ) \
      template< \
          typename T \
        , typename fallback_ \
              = autoboost::mpl::bool_< AUTOBOOST_PP_ARRAY_ELEM(3, args) > \
      > \
      class AUTOBOOST_PP_ARRAY_ELEM(0, args) { \
          introspect_macro(args, substitute_macro, member_macro) \
      public: \
          static const bool value \
              = AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args)< T >::value; \
          typedef typename AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args)< \
              T \
          >::type type; \
      }; \
    /**/

// AUTOBOOST_MPL_HAS_MEMBER_WITH_FUNCTION_SFINAE expands to the full
// implementation of the function-based metafunction. Compile with -E
// to see the preprocessor output for this macro.
#   define AUTOBOOST_MPL_HAS_MEMBER_WITH_FUNCTION_SFINAE( \
               args, substitute_macro, member_macro \
           ) \
      AUTOBOOST_MPL_HAS_MEMBER_IMPLEMENTATION( \
          args \
        , AUTOBOOST_MPL_HAS_MEMBER_INTROSPECT \
        , substitute_macro \
        , member_macro \
      ) \
    /**/

#   if AUTOBOOST_MPL_HAS_XXX_NEEDS_TEMPLATE_SFINAE

#     if !defined(AUTOBOOST_MPL_HAS_XXX_NEEDS_NAMESPACE_LEVEL_SUBSTITUTE)
#       if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1400)
#         define AUTOBOOST_MPL_HAS_XXX_NEEDS_NAMESPACE_LEVEL_SUBSTITUTE 1
#       endif
#     endif

#     if !AUTOBOOST_MPL_HAS_XXX_NEEDS_NAMESPACE_LEVEL_SUBSTITUTE
#       define AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME_WITH_TEMPLATE_SFINAE( \
                   args, n \
               ) \
          AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) \
        /**/
#     else
#       define AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME_WITH_TEMPLATE_SFINAE( \
                   args, n \
               ) \
          AUTOBOOST_PP_CAT( \
              autoboost_mpl_has_xxx_ \
            , AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) \
          ) \
        /**/
#     endif

#     define AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_TAG_NAME( \
                 args \
             ) \
        AUTOBOOST_PP_CAT( \
            AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME_WITH_TEMPLATE_SFINAE( \
                args, 0 \
            ) \
          , _tag \
        ) \
      /**/

#     define AUTOBOOST_MPL_HAS_MEMBER_MULTI_SUBSTITUTE_WITH_TEMPLATE_SFINAE( \
                 z, n, args \
             ) \
        template< \
             template< AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_INC(n), typename U) > class U \
        > \
        struct AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME_WITH_TEMPLATE_SFINAE( \
                args, n \
               ) { \
            typedef \
                AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_TAG_NAME(args) \
                type; \
        }; \
      /**/

#     define AUTOBOOST_MPL_HAS_MEMBER_SUBSTITUTE_WITH_TEMPLATE_SFINAE( \
                 args, substitute_macro \
             ) \
        typedef void \
            AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_TAG_NAME(args); \
        AUTOBOOST_PP_REPEAT( \
            AUTOBOOST_PP_ARRAY_ELEM(2, args) \
          , AUTOBOOST_MPL_HAS_MEMBER_MULTI_SUBSTITUTE_WITH_TEMPLATE_SFINAE \
          , args \
        ) \
      /**/

#     define AUTOBOOST_MPL_HAS_MEMBER_REJECT_WITH_TEMPLATE_SFINAE( \
                 args, member_macro \
             ) \
        template< \
            typename U \
          , typename V \
                = AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_TAG_NAME(args) \
        > \
        struct AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args) { \
            AUTOBOOST_STATIC_CONSTANT(bool, value = false); \
            typedef autoboost::mpl::bool_< value > type; \
        }; \
      /**/

#     define AUTOBOOST_MPL_HAS_MEMBER_MULTI_ACCEPT_WITH_TEMPLATE_SFINAE( \
                 z, n, args \
             ) \
        template< typename U > \
        struct AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)< \
            U \
          , typename \
                AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME_WITH_TEMPLATE_SFINAE( \
                    args, n \
                )< \
                    AUTOBOOST_MSVC_TYPENAME U::AUTOBOOST_PP_ARRAY_ELEM(1, args)< > \
                >::type \
        > { \
            AUTOBOOST_STATIC_CONSTANT(bool, value = true); \
            typedef autoboost::mpl::bool_< value > type; \
        }; \
      /**/

#     define AUTOBOOST_MPL_HAS_MEMBER_ACCEPT_WITH_TEMPLATE_SFINAE( \
                 args, member_macro \
             ) \
        AUTOBOOST_PP_REPEAT( \
            AUTOBOOST_PP_ARRAY_ELEM(2, args) \
          , AUTOBOOST_MPL_HAS_MEMBER_MULTI_ACCEPT_WITH_TEMPLATE_SFINAE \
          , args \
        ) \
      /**/

#     define AUTOBOOST_MPL_HAS_MEMBER_INTROSPECT_WITH_TEMPLATE_SFINAE( \
                 args, substitute_macro, member_macro \
             ) \
        AUTOBOOST_MPL_HAS_MEMBER_REJECT_WITH_TEMPLATE_SFINAE(args, member_macro) \
        AUTOBOOST_MPL_HAS_MEMBER_ACCEPT_WITH_TEMPLATE_SFINAE(args, member_macro) \
        template< typename U > \
        struct AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args) \
            : AUTOBOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)< U > { \
        }; \
      /**/

// AUTOBOOST_MPL_HAS_MEMBER_WITH_TEMPLATE_SFINAE expands to the full
// implementation of the template-based metafunction. Compile with -E
// to see the preprocessor output for this macro.
//
// Note that if AUTOBOOST_MPL_HAS_XXX_NEEDS_NAMESPACE_LEVEL_SUBSTITUTE is
// defined AUTOBOOST_MPL_HAS_MEMBER_SUBSTITUTE_WITH_TEMPLATE_SFINAE needs
// to be expanded at namespace level before
// AUTOBOOST_MPL_HAS_MEMBER_WITH_TEMPLATE_SFINAE can be used.
#     define AUTOBOOST_MPL_HAS_MEMBER_WITH_TEMPLATE_SFINAE( \
                 args, substitute_macro, member_macro \
             ) \
        AUTOBOOST_MPL_HAS_MEMBER_SUBSTITUTE_WITH_TEMPLATE_SFINAE( \
            args, substitute_macro \
        ) \
        AUTOBOOST_MPL_HAS_MEMBER_IMPLEMENTATION( \
            args \
          , AUTOBOOST_MPL_HAS_MEMBER_INTROSPECT_WITH_TEMPLATE_SFINAE \
          , substitute_macro \
          , member_macro \
        ) \
      /**/

#   endif // AUTOBOOST_MPL_HAS_XXX_NEEDS_TEMPLATE_SFINAE

// Note: In the current implementation the parameter and access macros
// are no longer expanded.
#   if !AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1400)
#     define AUTOBOOST_MPL_HAS_XXX_TEMPLATE_NAMED_DEF(trait, name, default_) \
        AUTOBOOST_MPL_HAS_MEMBER_WITH_FUNCTION_SFINAE( \
            ( 4, ( trait, name, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, default_ ) ) \
          , AUTOBOOST_MPL_HAS_MEMBER_TEMPLATE_SUBSTITUTE_PARAMETER \
          , AUTOBOOST_MPL_HAS_MEMBER_TEMPLATE_ACCESS \
        ) \
      /**/
#   else
#     define AUTOBOOST_MPL_HAS_XXX_TEMPLATE_NAMED_DEF(trait, name, default_) \
        AUTOBOOST_MPL_HAS_MEMBER_WITH_TEMPLATE_SFINAE( \
            ( 4, ( trait, name, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, default_ ) ) \
          , AUTOBOOST_MPL_HAS_MEMBER_TEMPLATE_SUBSTITUTE_PARAMETER \
          , AUTOBOOST_MPL_HAS_MEMBER_TEMPLATE_ACCESS \
        ) \
      /**/
#   endif

#else // AUTOBOOST_MPL_CFG_NO_HAS_XXX_TEMPLATE

// placeholder implementation

#   define AUTOBOOST_MPL_HAS_XXX_TEMPLATE_NAMED_DEF(trait, name, default_) \
      template< typename T \
              , typename fallback_ = autoboost::mpl::bool_< default_ > > \
      struct trait { \
          AUTOBOOST_STATIC_CONSTANT(bool, value = fallback_::value); \
          typedef fallback_ type; \
      }; \
    /**/

#endif // AUTOBOOST_MPL_CFG_NO_HAS_XXX_TEMPLATE

#   define AUTOBOOST_MPL_HAS_XXX_TEMPLATE_DEF(name) \
      AUTOBOOST_MPL_HAS_XXX_TEMPLATE_NAMED_DEF( \
          AUTOBOOST_PP_CAT(has_, name), name, false \
      ) \
    /**/

#endif // AUTOBOOST_MPL_HAS_XXX_HPP_INCLUDED

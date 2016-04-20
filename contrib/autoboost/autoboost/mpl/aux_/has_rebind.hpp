
#ifndef AUTOBOOST_MPL_AUX_HAS_REBIND_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_HAS_REBIND_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/intel.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if AUTOBOOST_WORKAROUND(__EDG_VERSION__, <= 244) && !defined(AUTOBOOST_INTEL_CXX_VERSION)
#   include <autoboost/mpl/has_xxx.hpp>
#elif AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
#   include <autoboost/mpl/has_xxx.hpp>
#   include <autoboost/mpl/if.hpp>
#   include <autoboost/mpl/bool.hpp>
#   include <autoboost/mpl/aux_/msvc_is_class.hpp>
#elif AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610))
#   include <autoboost/mpl/if.hpp>
#   include <autoboost/mpl/bool.hpp>
#   include <autoboost/mpl/aux_/yes_no.hpp>
#   include <autoboost/mpl/aux_/config/static_constant.hpp>
#   include <autoboost/type_traits/is_class.hpp>
#else
#   include <autoboost/mpl/aux_/type_wrapper.hpp>
#   include <autoboost/mpl/aux_/yes_no.hpp>
#   include <autoboost/mpl/aux_/config/static_constant.hpp>
#endif

namespace autoboost { namespace mpl { namespace aux {

#if AUTOBOOST_WORKAROUND(__EDG_VERSION__, <= 244) && !defined(AUTOBOOST_INTEL_CXX_VERSION)

AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_rebind, rebind, false)

#elif AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)

AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_rebind_impl, rebind, false)

template< typename T >
struct has_rebind
    : if_<
          msvc_is_class<T>
        , has_rebind_impl<T>
        , bool_<false>
        >::type
{
};

#else // the rest

template< typename T > struct has_rebind_tag {};
no_tag operator|(has_rebind_tag<int>, void const volatile*);

#   if !AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610))
template< typename T >
struct has_rebind
{
    static has_rebind_tag<T>* get();
    AUTOBOOST_STATIC_CONSTANT(bool, value =
          sizeof(has_rebind_tag<int>() | get()) == sizeof(yes_tag)
        );
};
#   else // __BORLANDC__
template< typename T >
struct has_rebind_impl
{
    static T* get();
    AUTOBOOST_STATIC_CONSTANT(bool, value =
          sizeof(has_rebind_tag<int>() | get()) == sizeof(yes_tag)
        );
};

template< typename T >
struct has_rebind
    : if_<
          is_class<T>
        , has_rebind_impl<T>
        , bool_<false>
        >::type
{
};
#   endif // __BORLANDC__

#endif

}}}

#endif // AUTOBOOST_MPL_AUX_HAS_REBIND_HPP_INCLUDED

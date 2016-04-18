
#ifndef AUTOBOOST_MPL_VOID_HPP_INCLUDED
#define AUTOBOOST_MPL_VOID_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/void_fwd.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN

//  [JDG Feb-4-2003] made void_ a complete type to allow it to be
//  instantiated so that it can be passed in as an object that can be
//  used to select an overloaded function. Possible use includes signaling
//  a zero arity functor evaluation call.
struct void_ { typedef void_ type; };

AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE

namespace autoboost { namespace mpl {

template< typename T >
struct is_void_
    : false_
{
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
    using false_::value;
#endif
};

template<>
struct is_void_<void_>
    : true_
{
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
    using true_::value;
#endif
};

template< typename T >
struct is_not_void_
    : true_
{
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
    using true_::value;
#endif
};

template<>
struct is_not_void_<void_>
    : false_
{
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
    using false_::value;
#endif
};

AUTOBOOST_MPL_AUX_NA_SPEC(1, is_void_)
AUTOBOOST_MPL_AUX_NA_SPEC(1, is_not_void_)

}}

#endif // AUTOBOOST_MPL_VOID_HPP_INCLUDED

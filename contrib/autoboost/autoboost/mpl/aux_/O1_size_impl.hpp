
#ifndef AUTOBOOST_MPL_O1_SIZE_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_O1_SIZE_IMPL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/O1_size_fwd.hpp>
#include <autoboost/mpl/long.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/aux_/has_size.hpp>
#include <autoboost/mpl/aux_/config/forwarding.hpp>
#include <autoboost/mpl/aux_/config/static_constant.hpp>
#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

namespace autoboost { namespace mpl {

// default implementation - returns 'Sequence::size' if sequence has a 'size'
// member, and -1 otherwise; conrete sequences might override it by
// specializing either the 'O1_size_impl' or the primary 'O1_size' template

#   if !AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300) \
    && !AUTOBOOST_WORKAROUND(__MWERKS__, AUTOBOOST_TESTED_AT(0x3003))

namespace aux {
template< typename Sequence > struct O1_size_impl
    : Sequence::size
{
};
}

template< typename Tag >
struct O1_size_impl
{
    template< typename Sequence > struct apply
#if !defined(AUTOBOOST_MPL_CFG_NO_NESTED_FORWARDING)
        : if_<
              aux::has_size<Sequence>
            , aux::O1_size_impl<Sequence>
            , long_<-1>
            >::type
    {
#else
    {
        typedef typename if_<
              aux::has_size<Sequence>
            , aux::O1_size_impl<Sequence>
            , long_<-1>
            >::type type;

        AUTOBOOST_STATIC_CONSTANT(long, value =
              (if_<
                  aux::has_size<Sequence>
                , aux::O1_size_impl<Sequence>
                , long_<-1>
                >::type::value)
            );
#endif
    };
};

#   else // AUTOBOOST_MSVC

template< typename Tag >
struct O1_size_impl
{
    template< typename Sequence > struct apply
        : long_<-1>
        {
        };
};

#   endif

}}

#endif // AUTOBOOST_MPL_O1_SIZE_IMPL_HPP_INCLUDED


#ifndef AUTOBOOST_MPL_TAG_HPP_INCLUDED
#define AUTOBOOST_MPL_TAG_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/eval_if.hpp>
#include <autoboost/mpl/void.hpp>
#include <autoboost/mpl/aux_/has_tag.hpp>
#include <autoboost/mpl/aux_/config/eti.hpp>

namespace autoboost { namespace mpl {

namespace aux {
template< typename T > struct tag_impl
{
    typedef typename T::tag type;
};
}

template< typename T, typename Default = void_ > struct tag
#if !defined(AUTOBOOST_MPL_CFG_MSVC_ETI_BUG)
    : if_<
          aux::has_tag<T>
        , aux::tag_impl<T>
        , Default
        >::type
{
#else
{
    typedef typename eval_if<
          aux::has_tag<T>
        , aux::tag_impl<T>
        , Default
        >::type type;

#endif
};

}}

#endif // AUTOBOOST_MPL_TAG_HPP_INCLUDED

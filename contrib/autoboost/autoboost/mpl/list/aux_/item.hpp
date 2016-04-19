
#ifndef AUTOBOOST_MPL_LIST_AUX_NODE_HPP_INCLUDED
#define AUTOBOOST_MPL_LIST_AUX_NODE_HPP_INCLUDED

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

#include <autoboost/mpl/long.hpp>
#include <autoboost/mpl/list/aux_/tag.hpp>
#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

namespace autoboost { namespace mpl {

template<
      typename Size
    , typename T
    , typename Next
    >
struct l_item
{
// agurt, 17/jul/03: to facilitate the deficient 'is_sequence' implementation
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300)
    typedef int begin;
#endif
    typedef aux::list_tag tag;
    typedef l_item type;

    typedef Size size;
    typedef T item;
    typedef Next next;
};

struct l_end
{
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300)
    typedef int begin;
#endif
    typedef aux::list_tag tag;
    typedef l_end type;
    typedef long_<0> size;
};

}}

#endif // AUTOBOOST_MPL_LIST_AUX_NODE_HPP_INCLUDED

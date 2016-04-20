
#ifndef AUTOBOOST_MPL_LIST_LIST0_HPP_INCLUDED
#define AUTOBOOST_MPL_LIST_LIST0_HPP_INCLUDED

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
#include <autoboost/mpl/aux_/na.hpp>
#include <autoboost/mpl/list/aux_/push_front.hpp>
#include <autoboost/mpl/list/aux_/pop_front.hpp>
#include <autoboost/mpl/list/aux_/push_back.hpp>
#include <autoboost/mpl/list/aux_/front.hpp>
#include <autoboost/mpl/list/aux_/clear.hpp>
#include <autoboost/mpl/list/aux_/O1_size.hpp>
#include <autoboost/mpl/list/aux_/size.hpp>
#include <autoboost/mpl/list/aux_/empty.hpp>
#include <autoboost/mpl/list/aux_/begin_end.hpp>
#include <autoboost/mpl/list/aux_/item.hpp>

namespace autoboost { namespace mpl {

template< typename Dummy = na > struct list0;

template<> struct list0<na>
    : l_end
{
    typedef l_end type;
};

}}

#endif // AUTOBOOST_MPL_LIST_LIST0_HPP_INCLUDED

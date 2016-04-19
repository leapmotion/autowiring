
#ifndef AUTOBOOST_MPL_RANGE_C_HPP_INCLUDED
#define AUTOBOOST_MPL_RANGE_C_HPP_INCLUDED

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

#include <autoboost/mpl/integral_c.hpp>
#include <autoboost/mpl/aux_/range_c/front.hpp>
#include <autoboost/mpl/aux_/range_c/back.hpp>
#include <autoboost/mpl/aux_/range_c/size.hpp>
#include <autoboost/mpl/aux_/range_c/O1_size.hpp>
#include <autoboost/mpl/aux_/range_c/empty.hpp>
#include <autoboost/mpl/aux_/range_c/iterator.hpp>
#include <autoboost/mpl/aux_/range_c/tag.hpp>

namespace autoboost { namespace mpl {

template<
      typename T
    , T Start
    , T Finish
    >
struct range_c
{
    typedef aux::half_open_range_tag tag;
    typedef T value_type;
    typedef range_c type;

    typedef integral_c<T,Start> start;
    typedef integral_c<T,Finish> finish;

    typedef r_iter<start> begin;
    typedef r_iter<finish> end;
};

}}

#endif // AUTOBOOST_MPL_RANGE_C_HPP_INCLUDED

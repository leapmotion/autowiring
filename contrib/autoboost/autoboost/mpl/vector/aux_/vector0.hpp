
#ifndef AUTOBOOST_MPL_VECTOR_AUX_VECTOR0_HPP_INCLUDED
#define AUTOBOOST_MPL_VECTOR_AUX_VECTOR0_HPP_INCLUDED

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
#include <autoboost/mpl/void.hpp>
#include <autoboost/mpl/aux_/na.hpp>
#include <autoboost/mpl/aux_/type_wrapper.hpp>

#include <autoboost/mpl/vector/aux_/iterator.hpp>
#include <autoboost/mpl/vector/aux_/tag.hpp>
#include <autoboost/mpl/aux_/config/typeof.hpp>

namespace autoboost { namespace mpl {

template< typename Dummy = na > struct vector0;

template<> struct vector0<na>
{
#if defined(AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES)
    typedef aux::vector_tag tag;
    typedef vector0         type;
    typedef long_<32768>    lower_bound_;
    typedef lower_bound_    upper_bound_;
    typedef long_<0>        size;

    static aux::type_wrapper<void_> item_(...);
#else
    typedef aux::vector_tag<0> tag;
    typedef vector0 type;
    typedef void_ item0;

    typedef v_iter<vector0<>,0> begin;
    typedef v_iter<vector0<>,0> end;
#endif
};

}}

#endif // AUTOBOOST_MPL_VECTOR_AUX_VECTOR0_HPP_INCLUDED


#ifndef AUTOBOOST_MPL_VECTOR_AUX_BEGIN_END_HPP_INCLUDED
#define AUTOBOOST_MPL_VECTOR_AUX_BEGIN_END_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/typeof.hpp>

#if defined(AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES)

#   include <autoboost/mpl/begin_end_fwd.hpp>
#   include <autoboost/mpl/vector/aux_/iterator.hpp>
#   include <autoboost/mpl/vector/aux_/tag.hpp>

namespace autoboost { namespace mpl {

template<>
struct begin_impl< aux::vector_tag >
{
    template< typename Vector > struct apply
    {
        typedef v_iter<Vector,0> type;
    };
};

template<>
struct end_impl< aux::vector_tag >
{
    template< typename Vector > struct apply
    {
        typedef v_iter<Vector,Vector::size::value> type;
    };
};

}}

#endif // AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES

#endif // AUTOBOOST_MPL_VECTOR_AUX_BEGIN_END_HPP_INCLUDED

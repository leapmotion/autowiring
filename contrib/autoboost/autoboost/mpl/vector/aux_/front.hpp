
#ifndef AUTOBOOST_MPL_VECTOR_AUX_FRONT_HPP_INCLUDED
#define AUTOBOOST_MPL_VECTOR_AUX_FRONT_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2008
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/front_fwd.hpp>
#include <autoboost/mpl/vector/aux_/at.hpp>
#include <autoboost/mpl/vector/aux_/tag.hpp>
#include <autoboost/mpl/aux_/nttp_decl.hpp>
#include <autoboost/mpl/aux_/config/typeof.hpp>
#include <autoboost/mpl/aux_/config/ctps.hpp>

namespace autoboost { namespace mpl {

#if defined(AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES)

template<>
struct front_impl< aux::vector_tag >
{
    template< typename Vector > struct apply
        : v_at<Vector,0>
    {
    };
};

#else

#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template< AUTOBOOST_MPL_AUX_NTTP_DECL(long, n_) >
struct front_impl< aux::vector_tag<n_> >
{
    template< typename Vector > struct apply
    {
        typedef typename Vector::item0 type;
    };
};

#endif // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

#endif // AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES

}}

#endif // AUTOBOOST_MPL_VECTOR_AUX_FRONT_HPP_INCLUDED

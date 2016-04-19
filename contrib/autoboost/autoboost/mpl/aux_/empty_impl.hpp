
#ifndef AUTOBOOST_MPL_AUX_EMPTY_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_EMPTY_IMPL_HPP_INCLUDED

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

#include <autoboost/mpl/empty_fwd.hpp>
#include <autoboost/mpl/begin_end.hpp>
#include <autoboost/mpl/aux_/traits_lambda_spec.hpp>
#include <autoboost/type_traits/is_same.hpp>

namespace autoboost { namespace mpl {

// default implementation; conrete sequences might override it by
// specializing either the 'empty_impl' or the primary 'empty' template

template< typename Tag >
struct empty_impl
{
    template< typename Sequence > struct apply
        : is_same<
              typename begin<Sequence>::type
            , typename end<Sequence>::type
            >
    {
    };
};

AUTOBOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(1,empty_impl)

}}

#endif // AUTOBOOST_MPL_AUX_EMPTY_IMPL_HPP_INCLUDED

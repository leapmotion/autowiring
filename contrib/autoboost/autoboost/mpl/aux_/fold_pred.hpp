
#ifndef AUTOBOOST_MPL_AUX_FOLD_PRED_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_FOLD_PRED_HPP_INCLUDED

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

#include <autoboost/mpl/same_as.hpp>
#include <autoboost/mpl/apply.hpp>

namespace autoboost { namespace mpl { namespace aux {

template< typename Last >
struct fold_pred
{
    template<
          typename State
        , typename Iterator
        >
    struct apply
        : not_same_as<Last>::template apply<Iterator>
    {
    };
};

}}}

#endif // AUTOBOOST_MPL_AUX_FOLD_PRED_HPP_INCLUDED

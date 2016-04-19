
#ifndef AUTOBOOST_MPL_COPY_HPP_INCLUDED
#define AUTOBOOST_MPL_COPY_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
// Copyright David Abrahams 2003-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/fold.hpp>
#include <autoboost/mpl/reverse_fold.hpp>
#include <autoboost/mpl/aux_/inserter_algorithm.hpp>

namespace autoboost { namespace mpl {

namespace aux {

template<
      typename Sequence
    , typename Inserter
    >
struct copy_impl
    : fold<
          Sequence
        , typename Inserter::state
        , typename Inserter::operation
        >
{
};

template<
      typename Sequence
    , typename Inserter
    >
struct reverse_copy_impl
    : reverse_fold<
          Sequence
        , typename Inserter::state
        , typename Inserter::operation
        >
{
};

} // namespace aux

AUTOBOOST_MPL_AUX_INSERTER_ALGORITHM_DEF(2, copy)

}}

#endif // AUTOBOOST_MPL_COPY_HPP_INCLUDED

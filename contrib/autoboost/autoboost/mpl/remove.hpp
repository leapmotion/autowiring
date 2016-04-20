
#ifndef AUTOBOOST_MPL_REMOVE_HPP_INCLUDED
#define AUTOBOOST_MPL_REMOVE_HPP_INCLUDED

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

#include <autoboost/mpl/remove_if.hpp>
#include <autoboost/mpl/same_as.hpp>
#include <autoboost/mpl/aux_/inserter_algorithm.hpp>

namespace autoboost { namespace mpl {

namespace aux {

template<
      typename Sequence
    , typename T
    , typename Inserter
    >
struct remove_impl
    : remove_if_impl< Sequence, same_as<T>, Inserter >
{
};

template<
      typename Sequence
    , typename T
    , typename Inserter
    >
struct reverse_remove_impl
    : reverse_remove_if_impl< Sequence, same_as<T>, Inserter >
{
};

} // namespace aux

AUTOBOOST_MPL_AUX_INSERTER_ALGORITHM_DEF(3, remove)

}}

#endif // AUTOBOOST_MPL_REMOVE_HPP_INCLUDED

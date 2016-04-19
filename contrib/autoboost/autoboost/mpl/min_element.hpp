
#ifndef AUTOBOOST_MPL_MIN_ELEMENT_HPP_INCLUDED
#define AUTOBOOST_MPL_MIN_ELEMENT_HPP_INCLUDED

// Copyright David Abrahams 2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/max_element.hpp>
#include <autoboost/mpl/not.hpp>

namespace autoboost { namespace mpl {

AUTOBOOST_MPL_AUX_COMMON_NAME_WKND(min_element)

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence)
    , typename Predicate = less<_,_>
    >
struct min_element
    : max_element<
          Sequence
        , mpl::not_<Predicate>
        >
{
};

AUTOBOOST_MPL_AUX_NA_SPEC(1, min_element)

}}

#endif // AUTOBOOST_MPL_MIN_ELEMENT_HPP_INCLUDED

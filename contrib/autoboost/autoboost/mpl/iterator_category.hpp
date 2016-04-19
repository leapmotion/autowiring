
#ifndef AUTOBOOST_MPL_ITERATOR_CATEGORY_HPP_INCLUDED
#define AUTOBOOST_MPL_ITERATOR_CATEGORY_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

namespace autoboost {  namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Iterator)
    >
struct iterator_category
{
    typedef typename Iterator::category type;
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,iterator_category,(Iterator))
};

AUTOBOOST_MPL_AUX_NA_SPEC(1, iterator_category)

}}

#endif // AUTOBOOST_MPL_ITERATOR_CATEGORY_HPP_INCLUDED


#ifndef AUTOBOOST_MPL_DEREF_HPP_INCLUDED
#define AUTOBOOST_MPL_DEREF_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/msvc_type.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>
#include <autoboost/mpl/aux_/config/eti.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Iterator)
    >
struct deref
{
#if !defined(AUTOBOOST_MPL_CFG_MSVC_70_ETI_BUG)
    typedef typename Iterator::type type;
#else
    typedef typename aux::msvc_type<Iterator>::type type;
#endif
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,deref,(Iterator))
};

AUTOBOOST_MPL_AUX_NA_SPEC(1, deref)

}}

#endif // AUTOBOOST_MPL_DEREF_HPP_INCLUDED

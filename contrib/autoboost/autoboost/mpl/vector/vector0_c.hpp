
#ifndef AUTOBOOST_MPL_VECTOR_VECTOR0_C_HPP_INCLUDED
#define AUTOBOOST_MPL_VECTOR_VECTOR0_C_HPP_INCLUDED

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

#include <autoboost/mpl/vector/vector0.hpp>
#include <autoboost/mpl/integral_c.hpp>

namespace autoboost { namespace mpl {

template< typename T > struct vector0_c
    : vector0<>
{
    typedef vector0_c type;
    typedef T value_type;
};

}}

#endif // AUTOBOOST_MPL_VECTOR_VECTOR0_C_HPP_INCLUDED

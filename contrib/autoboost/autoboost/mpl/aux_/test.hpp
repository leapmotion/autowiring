
#ifndef AUTOBOOST_MPL_AUX_TEST_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_TEST_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/test/test_case.hpp>
#include <autoboost/mpl/aux_/test/data.hpp>
#include <autoboost/mpl/aux_/test/assert.hpp>
#include <autoboost/detail/lightweight_test.hpp>

#include <autoboost/type_traits/is_same.hpp>

int main()
{
    return autoboost::report_errors();
}

using namespace autoboost;
using namespace mpl;

#endif // AUTOBOOST_MPL_AUX_TEST_HPP_INCLUDED


#ifndef AUTOBOOST_MPL_NUMERIC_CAST_HPP_INCLUDED
#define AUTOBOOST_MPL_NUMERIC_CAST_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

// agurt 21/sep/04: portability macro for the sake of MSVC 6.x-7.0;
// resolves conflicts with 'autoboost::numeric_cast' function template.
// use it in your own code _only_ if you care about compatibility with
// these outdated compilers!
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300) || AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x570) )
#   define AUTOBOOST_MPL_AUX_NUMERIC_CAST numeric_cast_
#else
#   define AUTOBOOST_MPL_AUX_NUMERIC_CAST numeric_cast
#endif

namespace autoboost { namespace mpl {

// no default implementation; the definition is needed to make MSVC happy

template< typename SourceTag, typename TargetTag > struct AUTOBOOST_MPL_AUX_NUMERIC_CAST
{
    template< typename N > struct apply;
};

}}

#endif // AUTOBOOST_MPL_NUMERIC_CAST_HPP_INCLUDED

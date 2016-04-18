
#ifndef AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/gcc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MPL_CFG_GCC, AUTOBOOST_TESTED_AT(0x0302)) \
    || AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x561)) \
    || AUTOBOOST_WORKAROUND(__SUNPRO_CC, AUTOBOOST_TESTED_AT(0x530)) \
    || AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840))

namespace autoboost { namespace mpl { namespace aux {
template< typename T > struct nested_type_wknd
    : T::type
{
};
}}}

#if AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840))
#   define AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND(T) \
    aux::nested_type_wknd<T> \
/**/
#else
#   define AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND(T) \
    ::autoboost::mpl::aux::nested_type_wknd<T> \
/**/
#endif

#else // !AUTOBOOST_MPL_CFG_GCC et al.

#   define AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND(T) T::type

#endif

#endif // AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND_HPP_INCLUDED

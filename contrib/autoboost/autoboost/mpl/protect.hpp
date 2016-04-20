
#ifndef AUTOBOOST_MPL_PROTECT_HPP_INCLUDED
#define AUTOBOOST_MPL_PROTECT_HPP_INCLUDED

// Copyright Peter Dimov 2001
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

#include <autoboost/mpl/aux_/arity.hpp>
#include <autoboost/mpl/aux_/config/dtp.hpp>
#include <autoboost/mpl/aux_/nttp_decl.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(T)
    , int not_le_ = 0
    >
struct protect : T
{
#if AUTOBOOST_WORKAROUND(__EDG_VERSION__, == 238)
    typedef mpl::protect type;
#else
    typedef protect type;
#endif
};

#if defined(AUTOBOOST_MPL_CFG_BROKEN_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
namespace aux {
template< AUTOBOOST_MPL_AUX_NTTP_DECL(int, N), typename T >
struct arity< protect<T>, N >
    : arity<T,N>
{
};
} // namespace aux
#endif

AUTOBOOST_MPL_AUX_NA_SPEC_MAIN(1, protect)
#if !defined(AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)
AUTOBOOST_MPL_AUX_NA_SPEC_TEMPLATE_ARITY(1, 1, protect)
#endif

}}

#endif // AUTOBOOST_MPL_PROTECT_HPP_INCLUDED

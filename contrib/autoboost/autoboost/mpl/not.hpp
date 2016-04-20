
#ifndef AUTOBOOST_MPL_NOT_HPP_INCLUDED
#define AUTOBOOST_MPL_NOT_HPP_INCLUDED

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

#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/aux_/nttp_decl.hpp>
#include <autoboost/mpl/aux_/nested_type_wknd.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

namespace autoboost { namespace mpl {

namespace aux {

template< AUTOBOOST_MPL_AUX_NTTP_DECL(long, C_) > // 'long' is intentional here
struct not_impl
    : bool_<!C_>
{
};

} // namespace aux


template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(T)
    >
struct not_
    : aux::not_impl<
          AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND(T)::value
        >
{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,not_,(T))
};

AUTOBOOST_MPL_AUX_NA_SPEC(1,not_)

}}

#endif // AUTOBOOST_MPL_NOT_HPP_INCLUDED

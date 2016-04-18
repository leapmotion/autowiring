
#ifndef AUTOBOOST_MPL_SET_AUX_HAS_KEY_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_SET_AUX_HAS_KEY_IMPL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2004
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

#include <autoboost/mpl/set/aux_/tag.hpp>
#include <autoboost/mpl/has_key_fwd.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/aux_/overload_names.hpp>
#include <autoboost/mpl/aux_/static_cast.hpp>
#include <autoboost/mpl/aux_/yes_no.hpp>
#include <autoboost/mpl/aux_/type_wrapper.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>
#include <autoboost/mpl/aux_/config/static_constant.hpp>

namespace autoboost { namespace mpl {

template<>
struct has_key_impl< aux::set_tag >
{
    template< typename Set, typename T > struct apply
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, AUTOBOOST_TESTED_AT(1400)) \
    || AUTOBOOST_WORKAROUND(__EDG_VERSION__, <= 245)
    {
        AUTOBOOST_STATIC_CONSTANT(bool, value =
              ( sizeof( AUTOBOOST_MPL_AUX_OVERLOAD_CALL_IS_MASKED(
                    Set
                  , AUTOBOOST_MPL_AUX_STATIC_CAST(aux::type_wrapper<T>*, 0)
                  ) ) == sizeof(aux::no_tag) )
            );

        typedef bool_<value> type;

#else // ISO98 C++
        : bool_<
              ( sizeof( AUTOBOOST_MPL_AUX_OVERLOAD_CALL_IS_MASKED(
                    Set
                  , AUTOBOOST_MPL_AUX_STATIC_CAST(aux::type_wrapper<T>*, 0)
                  ) ) == sizeof(aux::no_tag) )
            >
    {
#endif
    };
};

}}

#endif // AUTOBOOST_MPL_SET_AUX_HAS_KEY_IMPL_HPP_INCLUDED

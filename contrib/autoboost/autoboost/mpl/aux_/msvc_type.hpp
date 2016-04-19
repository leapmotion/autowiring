
#ifndef AUTOBOOST_MPL_AUX_MSVC_TYPE_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_MSVC_TYPE_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/config/eti.hpp>
#include <autoboost/mpl/aux_/is_msvc_eti_arg.hpp>

namespace autoboost { namespace mpl { namespace aux {

#if defined(AUTOBOOST_MPL_CFG_MSVC_70_ETI_BUG)

template< bool > struct msvc_type_impl
{
    template< typename T > struct result_
    {
        typedef typename T::type type;
    };
};

template<> struct msvc_type_impl<true>
{
    template< typename T > struct result_
    {
        typedef result_ type;
    };
};

template< typename T > struct msvc_type
    : msvc_type_impl< is_msvc_eti_arg<T>::value >
        ::template result_<T>
{
};

#else // AUTOBOOST_MPL_CFG_MSVC_70_ETI_BUG

template< typename T > struct msvc_type
{
    typedef typename T::type type;
};

template<> struct msvc_type<int>
{
    typedef int type;
};

#endif

}}}

#endif // AUTOBOOST_MPL_AUX_MSVC_TYPE_HPP_INCLUDED

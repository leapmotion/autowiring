
#ifndef AUTOBOOST_MPL_AUX_CONFIG_NTTP_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_NTTP_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

// MSVC 6.5 ICE-s on the code as simple as this (see "aux_/nttp_decl.hpp"
// for a workaround):
//
//    namespace std {
//    template< typename Char > struct string;
//    }
//
//    void foo(std::string<char>);
//
//    namespace autoboost { namespace mpl {
//    template< int > struct arg;
//    }}

#if    !defined(AUTOBOOST_MPL_CFG_NTTP_BUG) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE) \
    && AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)

#   define AUTOBOOST_MPL_CFG_NTTP_BUG

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_NTTP_HPP_INCLUDED

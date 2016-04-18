
#ifndef AUTOBOOST_MPL_AUX_ADL_BARRIER_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_ADL_BARRIER_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/config/adl.hpp>
#include <autoboost/mpl/aux_/config/gcc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE)

#   define AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE ampl_
#   define AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN namespace ampl_ {
#   define AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE }
#   define AUTOBOOST_MPL_AUX_ADL_BARRIER_DECL(type) \
    namespace autoboost { namespace ampl { \
    using ::AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::type; \
    } } \
/**/

#if !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)
namespace AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE { namespace aux {} }
namespace autoboost { namespace mpl { using namespace AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE;
namespace aux { using namespace AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::aux; }
}}
#endif

#else // AUTOBOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE

#   define AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE autoboost::mpl
#   define AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN namespace autoboost { namespace mpl {
#   define AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE }}
#   define AUTOBOOST_MPL_AUX_ADL_BARRIER_DECL(type) /**/

#endif

#endif // AUTOBOOST_MPL_AUX_ADL_BARRIER_HPP_INCLUDED

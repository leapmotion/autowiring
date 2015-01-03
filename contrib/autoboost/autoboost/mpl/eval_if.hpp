
#ifndef AUTOBOOST_MPL_EVAL_IF_HPP_INCLUDED
#define AUTOBOOST_MPL_EVAL_IF_HPP_INCLUDED

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

#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>
#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/gcc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(C)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(F1)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(F2)
    >
struct eval_if
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300) \
     || ( AUTOBOOST_WORKAROUND(AUTOBOOST_MPL_CFG_GCC, >= 0x0300) \
        && AUTOBOOST_WORKAROUND(AUTOBOOST_MPL_CFG_GCC, AUTOBOOST_TESTED_AT(0x0304)) \
        )
{
    typedef typename if_<C,F1,F2>::type f_;
    typedef typename f_::type type;
#else
    : if_<C,F1,F2>::type
{
#endif
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(3,eval_if,(C,F1,F2))
};

// (almost) copy & paste in order to save one more
// recursively nested template instantiation to user
template<
      bool C
    , typename F1
    , typename F2
    >
struct eval_if_c
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300) \
     || ( AUTOBOOST_WORKAROUND(AUTOBOOST_MPL_CFG_GCC, >= 0x0300) \
        && AUTOBOOST_WORKAROUND(AUTOBOOST_MPL_CFG_GCC, AUTOBOOST_TESTED_AT(0x0304)) \
        )
{
    typedef typename if_c<C,F1,F2>::type f_;
    typedef typename f_::type type;
#else
    : if_c<C,F1,F2>::type
{
#endif
};

AUTOBOOST_MPL_AUX_NA_SPEC(3, eval_if)

}}

#endif // AUTOBOOST_MPL_EVAL_IF_HPP_INCLUDED

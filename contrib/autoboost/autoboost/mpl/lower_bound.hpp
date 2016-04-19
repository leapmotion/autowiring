
#ifndef AUTOBOOST_MPL_LOWER_BOUND_HPP_INCLUDED
#define AUTOBOOST_MPL_LOWER_BOUND_HPP_INCLUDED

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

#include <autoboost/mpl/less.hpp>
#include <autoboost/mpl/lambda.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610))
#   define AUTOBOOST_MPL_CFG_STRIPPED_DOWN_LOWER_BOUND_IMPL
#endif

#if !defined(AUTOBOOST_MPL_CFG_STRIPPED_DOWN_LOWER_BOUND_IMPL)
#   include <autoboost/mpl/minus.hpp>
#   include <autoboost/mpl/divides.hpp>
#   include <autoboost/mpl/size.hpp>
#   include <autoboost/mpl/advance.hpp>
#   include <autoboost/mpl/begin_end.hpp>
#   include <autoboost/mpl/long.hpp>
#   include <autoboost/mpl/eval_if.hpp>
#   include <autoboost/mpl/prior.hpp>
#   include <autoboost/mpl/deref.hpp>
#   include <autoboost/mpl/apply.hpp>
#   include <autoboost/mpl/aux_/value_wknd.hpp>
#else
#   include <autoboost/mpl/not.hpp>
#   include <autoboost/mpl/find.hpp>
#   include <autoboost/mpl/bind.hpp>
#endif

#include <autoboost/config.hpp>

namespace autoboost { namespace mpl {

#if defined(AUTOBOOST_MPL_CFG_STRIPPED_DOWN_LOWER_BOUND_IMPL)

// agurt 23/oct/02: has a wrong complexity etc., but at least it works
// feel free to contribute a better implementation!
template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(T)
    , typename Predicate = less<>
    , typename pred_ = typename lambda<Predicate>::type
    >
struct lower_bound
    : find_if< Sequence, bind1< not_<>, bind2<pred_,_,T> > >
{
};

#else

namespace aux {

template<
      typename Distance
    , typename Predicate
    , typename T
    , typename DeferredIterator
    >
struct lower_bound_step_impl;

template<
      typename Distance
    , typename Predicate
    , typename T
    , typename DeferredIterator
    >
struct lower_bound_step
{
    typedef typename eval_if<
          Distance
        , lower_bound_step_impl<Distance,Predicate,T,DeferredIterator>
        , DeferredIterator
        >::type type;
};

template<
      typename Distance
    , typename Predicate
    , typename T
    , typename DeferredIterator
    >
struct lower_bound_step_impl
{
    typedef typename divides< Distance, long_<2> >::type offset_;
    typedef typename DeferredIterator::type iter_;
    typedef typename advance< iter_,offset_ >::type middle_;
    typedef typename apply2<
              Predicate
            , typename deref<middle_>::type
            , T
            >::type cond_;

    typedef typename prior< minus< Distance, offset_> >::type step_;
    typedef lower_bound_step< offset_,Predicate,T,DeferredIterator > step_forward_;
    typedef lower_bound_step< step_,Predicate,T,next<middle_> > step_backward_;
    typedef typename eval_if<
          cond_
        , step_backward_
        , step_forward_
        >::type type;
};


} // namespace aux

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(T)
    , typename Predicate = less<>
    >
struct lower_bound
{
 private:
    typedef typename lambda<Predicate>::type pred_;
    typedef typename size<Sequence>::type size_;

 public:
    typedef typename aux::lower_bound_step<
        size_,pred_,T,begin<Sequence>
        >::type type;
};

#endif // AUTOBOOST_MPL_CFG_STRIPPED_DOWN_LOWER_BOUND_IMPL

AUTOBOOST_MPL_AUX_NA_SPEC(2, lower_bound)

}}

#endif // AUTOBOOST_MPL_LOWER_BOUND_HPP_INCLUDED

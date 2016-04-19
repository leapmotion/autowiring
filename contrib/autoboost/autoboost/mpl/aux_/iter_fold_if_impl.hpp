
#ifndef AUTOBOOST_MPL_AUX_ITER_FOLD_IF_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_ITER_FOLD_IF_IMPL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
// Copyright David Abrahams 2001-2002
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#if !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)
#   include <autoboost/mpl/identity.hpp>
#   include <autoboost/mpl/next.hpp>
#   include <autoboost/mpl/if.hpp>
#   include <autoboost/mpl/apply.hpp>
#   include <autoboost/mpl/aux_/value_wknd.hpp>
#endif

#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER iter_fold_if_impl.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/limits/unrolling.hpp>
#   include <autoboost/preprocessor/arithmetic/sub.hpp>
#   include <autoboost/preprocessor/repeat.hpp>
#   include <autoboost/preprocessor/inc.hpp>
#   include <autoboost/preprocessor/dec.hpp>
#   include <autoboost/preprocessor/cat.hpp>

namespace autoboost { namespace mpl { namespace aux {

template< typename Iterator, typename State >
struct iter_fold_if_null_step
{
    typedef State state;
    typedef Iterator iterator;
};

template< bool >
struct iter_fold_if_step_impl
{
    template<
          typename Iterator
        , typename State
        , typename StateOp
        , typename IteratorOp
        >
    struct result_
    {
        typedef typename apply2<StateOp,State,Iterator>::type state;
        typedef typename IteratorOp::type iterator;
    };
};

template<>
struct iter_fold_if_step_impl<false>
{
    template<
          typename Iterator
        , typename State
        , typename StateOp
        , typename IteratorOp
        >
    struct result_
    {
        typedef State state;
        typedef Iterator iterator;
    };
};

// agurt, 25/jun/02: MSVC 6.5 workaround, had to get rid of inheritance
// here and in 'iter_fold_if_backward_step', because sometimes it interfered
// with the "early template instantiation bug" in _really_ ugly ways
template<
      typename Iterator
    , typename State
    , typename ForwardOp
    , typename Predicate
    >
struct iter_fold_if_forward_step
{
    typedef typename apply2<Predicate,State,Iterator>::type not_last;
    typedef typename iter_fold_if_step_impl<
          AUTOBOOST_MPL_AUX_MSVC_VALUE_WKND(not_last)::value
        >::template result_< Iterator,State,ForwardOp,mpl::next<Iterator> > impl_;

    typedef typename impl_::state state;
    typedef typename impl_::iterator iterator;
};

template<
      typename Iterator
    , typename State
    , typename BackwardOp
    , typename Predicate
    >
struct iter_fold_if_backward_step
{
    typedef typename apply2<Predicate,State,Iterator>::type not_last;
    typedef typename iter_fold_if_step_impl<
          AUTOBOOST_MPL_AUX_MSVC_VALUE_WKND(not_last)::value
        >::template result_< Iterator,State,BackwardOp,identity<Iterator> > impl_;

    typedef typename impl_::state state;
    typedef typename impl_::iterator iterator;
};


// local macros, #undef-ined at the end of the header

#   define AUX_ITER_FOLD_FORWARD_STEP(unused, i, unused2) \
    typedef iter_fold_if_forward_step< \
          typename AUTOBOOST_PP_CAT(forward_step,i)::iterator \
        , typename AUTOBOOST_PP_CAT(forward_step,i)::state \
        , ForwardOp \
        , ForwardPredicate \
        > AUTOBOOST_PP_CAT(forward_step, AUTOBOOST_PP_INC(i)); \
    /**/

#   define AUX_ITER_FOLD_BACKWARD_STEP_FUNC(i) \
    typedef iter_fold_if_backward_step< \
          typename AUTOBOOST_PP_CAT(forward_step,AUTOBOOST_PP_DEC(i))::iterator \
        , typename AUTOBOOST_PP_CAT(backward_step,i)::state \
        , BackwardOp \
        , BackwardPredicate \
        > AUTOBOOST_PP_CAT(backward_step,AUTOBOOST_PP_DEC(i)); \
    /**/

#   define AUX_ITER_FOLD_BACKWARD_STEP(unused, i, unused2) \
    AUX_ITER_FOLD_BACKWARD_STEP_FUNC( \
        AUTOBOOST_PP_SUB_D(1,AUTOBOOST_MPL_LIMIT_UNROLLING,i) \
        ) \
    /**/

#   define AUX_LAST_FORWARD_STEP \
    AUTOBOOST_PP_CAT(forward_step, AUTOBOOST_MPL_LIMIT_UNROLLING) \
    /**/

#   define AUX_LAST_BACKWARD_STEP \
    AUTOBOOST_PP_CAT(backward_step, AUTOBOOST_MPL_LIMIT_UNROLLING) \
    /**/

template<
      typename Iterator
    , typename State
    , typename ForwardOp
    , typename ForwardPredicate
    , typename BackwardOp
    , typename BackwardPredicate
    >
struct iter_fold_if_impl
{
 private:
    typedef iter_fold_if_null_step<Iterator,State> forward_step0;
    AUTOBOOST_PP_REPEAT(
          AUTOBOOST_MPL_LIMIT_UNROLLING
        , AUX_ITER_FOLD_FORWARD_STEP
        , unused
        )

    typedef typename if_<
          typename AUX_LAST_FORWARD_STEP::not_last
        , iter_fold_if_impl<
              typename AUX_LAST_FORWARD_STEP::iterator
            , typename AUX_LAST_FORWARD_STEP::state
            , ForwardOp
            , ForwardPredicate
            , BackwardOp
            , BackwardPredicate
            >
        , iter_fold_if_null_step<
              typename AUX_LAST_FORWARD_STEP::iterator
            , typename AUX_LAST_FORWARD_STEP::state
            >
        >::type AUX_LAST_BACKWARD_STEP;

    AUTOBOOST_PP_REPEAT(
          AUTOBOOST_MPL_LIMIT_UNROLLING
        , AUX_ITER_FOLD_BACKWARD_STEP
        , unused
        )

 public:
    typedef typename backward_step0::state state;
    typedef typename AUX_LAST_BACKWARD_STEP::iterator iterator;
};

#   undef AUX_LAST_BACKWARD_STEP
#   undef AUX_LAST_FORWARD_STEP
#   undef AUX_ITER_FOLD_BACKWARD_STEP
#   undef AUX_ITER_FOLD_BACKWARD_STEP_FUNC
#   undef AUX_ITER_FOLD_FORWARD_STEP

}}}

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_AUX_ITER_FOLD_IF_IMPL_HPP_INCLUDED


#if !defined(AUTOBOOST_PP_IS_ITERATING)

///// header body

#ifndef AUTOBOOST_MPL_AUX778076_ADVANCE_BACKWARD_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX778076_ADVANCE_BACKWARD_HPP_INCLUDED

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

#if !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)
#   include <autoboost/mpl/prior.hpp>
#   include <autoboost/mpl/apply_wrap.hpp>
#endif

#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if    !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   define AUTOBOOST_MPL_PREPROCESSED_HEADER advance_backward.hpp
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/limits/unrolling.hpp>
#   include <autoboost/mpl/aux_/nttp_decl.hpp>
#   include <autoboost/mpl/aux_/config/eti.hpp>

#   include <autoboost/preprocessor/iterate.hpp>
#   include <autoboost/preprocessor/cat.hpp>
#   include <autoboost/preprocessor/inc.hpp>

namespace autoboost { namespace mpl { namespace aux {

// forward declaration
template< AUTOBOOST_MPL_AUX_NTTP_DECL(long, N) > struct advance_backward;

#   define AUTOBOOST_PP_ITERATION_PARAMS_1 \
    (3,(0, AUTOBOOST_MPL_LIMIT_UNROLLING, <autoboost/mpl/aux_/advance_backward.hpp>))
#   include AUTOBOOST_PP_ITERATE()

// implementation for N that exceeds AUTOBOOST_MPL_LIMIT_UNROLLING
template< AUTOBOOST_MPL_AUX_NTTP_DECL(long, N) >
struct advance_backward
{
    template< typename Iterator > struct apply
    {
        typedef typename apply_wrap1<
              advance_backward<AUTOBOOST_MPL_LIMIT_UNROLLING>
            , Iterator
            >::type chunk_result_;

        typedef typename apply_wrap1<
              advance_backward<(
                (N - AUTOBOOST_MPL_LIMIT_UNROLLING) < 0
                    ? 0
                    : N - AUTOBOOST_MPL_LIMIT_UNROLLING
                    )>
            , chunk_result_
            >::type type;
    };
};

}}}

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_AUX778076_ADVANCE_BACKWARD_HPP_INCLUDED

///// iteration, depth == 1

// For gcc 4.4 compatability, we must include the
// AUTOBOOST_PP_ITERATION_DEPTH test inside an #else clause.
#else // AUTOBOOST_PP_IS_ITERATING
#if AUTOBOOST_PP_ITERATION_DEPTH() == 1
#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

template<>
struct advance_backward< AUTOBOOST_PP_FRAME_ITERATION(1) >
{
    template< typename Iterator > struct apply
    {
        typedef Iterator iter0;

#if i_ > 0
#   define AUTOBOOST_PP_ITERATION_PARAMS_2 \
    (3,(1, AUTOBOOST_PP_FRAME_ITERATION(1), <autoboost/mpl/aux_/advance_backward.hpp>))
#   include AUTOBOOST_PP_ITERATE()
#endif

        typedef AUTOBOOST_PP_CAT(iter,AUTOBOOST_PP_FRAME_ITERATION(1)) type;
    };

#if defined(AUTOBOOST_MPL_CFG_MSVC_60_ETI_BUG)
    /// ETI workaround
    template<> struct apply<int>
    {
        typedef int type;
    };
#endif
};

#undef i_

///// iteration, depth == 2

#elif AUTOBOOST_PP_ITERATION_DEPTH() == 2

#   define AUX778076_ITER_0 AUTOBOOST_PP_CAT(iter,AUTOBOOST_PP_DEC(AUTOBOOST_PP_FRAME_ITERATION(2)))
#   define AUX778076_ITER_1 AUTOBOOST_PP_CAT(iter,AUTOBOOST_PP_FRAME_ITERATION(2))

        typedef typename prior<AUX778076_ITER_0>::type AUX778076_ITER_1;

#   undef AUX778076_ITER_1
#   undef AUX778076_ITER_0

#endif // AUTOBOOST_PP_ITERATION_DEPTH()
#endif // AUTOBOOST_PP_IS_ITERATING

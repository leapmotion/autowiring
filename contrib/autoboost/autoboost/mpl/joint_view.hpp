
#ifndef AUTOBOOST_MPL_JOINT_VIEW_HPP_INCLUDED
#define AUTOBOOST_MPL_JOINT_VIEW_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/joint_iter.hpp>
#include <autoboost/mpl/plus.hpp>
#include <autoboost/mpl/size_fwd.hpp>
#include <autoboost/mpl/begin_end.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>

namespace autoboost { namespace mpl {

namespace aux {
struct joint_view_tag;
}

template<>
struct size_impl< aux::joint_view_tag >
{
    template < typename JointView > struct apply
      : plus<
            size<typename JointView::sequence1_>
          , size<typename JointView::sequence2_>
          >
    {};
};

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence1_)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence2_)
    >
struct joint_view
{
    typedef typename mpl::begin<Sequence1_>::type   first1_;
    typedef typename mpl::end<Sequence1_>::type     last1_;
    typedef typename mpl::begin<Sequence2_>::type   first2_;
    typedef typename mpl::end<Sequence2_>::type     last2_;

    // agurt, 25/may/03: for the 'size_traits' implementation above
    typedef Sequence1_ sequence1_;
    typedef Sequence2_ sequence2_;

    typedef joint_view type;
    typedef aux::joint_view_tag tag;
    typedef joint_iter<first1_,last1_,first2_>  begin;
    typedef joint_iter<last1_,last1_,last2_>    end;
};

AUTOBOOST_MPL_AUX_NA_SPEC(2, joint_view)

}}

#endif // AUTOBOOST_MPL_JOINT_VIEW_HPP_INCLUDED

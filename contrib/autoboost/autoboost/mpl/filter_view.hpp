
#ifndef AUTOBOOST_MPL_FILTER_VIEW_HPP_INCLUDED
#define AUTOBOOST_MPL_FILTER_VIEW_HPP_INCLUDED

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

#include <autoboost/mpl/begin_end.hpp>
#include <autoboost/mpl/lambda.hpp>
#include <autoboost/mpl/aux_/filter_iter.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(Predicate)
    >
struct filter_view
{
 private:
    typedef typename lambda<Predicate>::type pred_;
    typedef typename begin<Sequence>::type first_;
    typedef typename end<Sequence>::type last_;

 public:
    struct tag;
    typedef filter_view type;
    typedef typename aux::next_filter_iter< first_,last_,pred_ >::type begin;
    typedef aux::filter_iter< last_,last_,pred_ > end;
};

AUTOBOOST_MPL_AUX_NA_SPEC(2, filter_view)

}}

#endif // AUTOBOOST_MPL_FILTER_VIEW_HPP_INCLUDED

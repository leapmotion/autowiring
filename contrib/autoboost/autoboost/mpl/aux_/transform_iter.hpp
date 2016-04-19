
#ifndef AUTOBOOST_MPL_AUX_TRANSFORM_ITER_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_TRANSFORM_ITER_HPP_INCLUDED

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

#include <autoboost/mpl/apply.hpp>
#include <autoboost/mpl/iterator_tags.hpp>
#include <autoboost/mpl/next.hpp>
#include <autoboost/mpl/deref.hpp>
#include <autoboost/mpl/aux_/lambda_spec.hpp>
#include <autoboost/mpl/aux_/config/ctps.hpp>
#include <autoboost/type_traits/is_same.hpp>

namespace autoboost { namespace mpl {

namespace aux {

#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template<
      typename Iterator
    , typename LastIterator
    , typename F
    >
struct transform_iter
{
    typedef Iterator base;
    typedef forward_iterator_tag category;
    typedef transform_iter< typename mpl::next<base>::type,LastIterator,F > next;

    typedef typename apply1<
          F
        , typename deref<base>::type
        >::type type;
};

template<
      typename LastIterator
    , typename F
    >
struct transform_iter< LastIterator,LastIterator,F >
{
    typedef LastIterator base;
    typedef forward_iterator_tag category;
};

#else

template<
      typename Iterator
    , typename LastIterator
    , typename F
    >
struct transform_iter;

template< bool >
struct transform_iter_impl
{
    template<
          typename Iterator
        , typename LastIterator
        , typename F
        >
    struct result_
    {
        typedef Iterator base;
        typedef forward_iterator_tag category;
        typedef transform_iter< typename mpl::next<Iterator>::type,LastIterator,F > next;

        typedef typename apply1<
              F
            , typename deref<Iterator>::type
            >::type type;
    };
};

template<>
struct transform_iter_impl<true>
{
    template<
          typename Iterator
        , typename LastIterator
        , typename F
        >
    struct result_
    {
        typedef Iterator base;
        typedef forward_iterator_tag category;
    };
};

template<
      typename Iterator
    , typename LastIterator
    , typename F
    >
struct transform_iter
    : transform_iter_impl<
          ::autoboost::is_same<Iterator,LastIterator>::value
        >::template result_< Iterator,LastIterator,F >
{
};

#endif // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace aux

AUTOBOOST_MPL_AUX_PASS_THROUGH_LAMBDA_SPEC(3, aux::transform_iter)

}}

#endif // AUTOBOOST_MPL_AUX_TRANSFORM_ITER_HPP_INCLUDED

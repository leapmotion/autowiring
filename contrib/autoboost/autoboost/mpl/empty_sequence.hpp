
#ifndef AUTOBOOST_MPL_EMPTY_SEQUENCE_HPP_INCLUDED
#define AUTOBOOST_MPL_EMPTY_SEQUENCE_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2004
// Copyright Alexander Nasonov 2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/size_fwd.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/iterator_tags.hpp>

namespace autoboost { namespace mpl {

struct empty_sequence
{
    struct tag;
    typedef empty_sequence type;
    struct begin { typedef random_access_iterator_tag category; };
    typedef begin end;
};

template<>
struct size_impl<empty_sequence::tag>
{
    template< typename Sequence > struct apply
        : int_<0>
    {
    };
};

}}

#endif // #ifndef AUTOBOOST_MPL_EMPTY_SEQUENCE_HPP_INCLUDED

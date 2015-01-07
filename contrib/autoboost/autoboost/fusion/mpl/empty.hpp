/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_EMPTY_10022005_1619)
#define FUSION_EMPTY_10022005_1619

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/mpl/empty.hpp>
#include <autoboost/fusion/sequence/intrinsic/empty.hpp>

namespace autoboost { namespace mpl
{
    template <typename Tag>
    struct empty_impl;

    template <>
    struct empty_impl<fusion::fusion_sequence_tag>
    {
        template <typename Sequence>
        struct apply : fusion::result_of::empty<Sequence> {};
    };
}}

#endif

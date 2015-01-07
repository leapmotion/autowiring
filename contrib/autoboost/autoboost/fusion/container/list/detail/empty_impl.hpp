/*=============================================================================
    Copyright (c) 2007 Tobias Schwinger

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(AUTOBOOST_FUSION_SEQUENCE_EMPTY_IMPL_HPP_INCLUDED)
#define AUTOBOOST_FUSION_SEQUENCE_EMPTY_IMPL_HPP_INCLUDED

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/type_traits/is_convertible.hpp>
#include <autoboost/fusion/container/list/nil.hpp>

namespace autoboost { namespace fusion
{
    struct cons_tag;

    struct nil_;

    template <typename Car, typename Cdr>
    struct cons;

    namespace extension
    {
        template <typename Tag>
        struct empty_impl;

        template <>
        struct empty_impl<cons_tag>
        {
            template <typename Sequence>
            struct apply
                : autoboost::is_convertible<Sequence, nil_>
            {};
        };
    }
}}

#endif

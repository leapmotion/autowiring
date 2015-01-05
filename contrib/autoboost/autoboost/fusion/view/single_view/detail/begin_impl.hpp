/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(AUTOBOOST_FUSION_SINGLE_VIEW_BEGIN_IMPL_05052005_0305)
#define AUTOBOOST_FUSION_SINGLE_VIEW_BEGIN_IMPL_05052005_0305

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/mpl/int.hpp>

namespace autoboost { namespace fusion
{
    struct single_view_tag;

    template <typename SingleView, typename Pos>
    struct single_view_iterator;

    namespace extension
    {
        template <typename Tag>
        struct begin_impl;

        template <>
        struct begin_impl<single_view_tag>
        {
            template <typename Sequence>
            struct apply
            {
                typedef single_view_iterator<Sequence, mpl::int_<0> > type;
    
                AUTOBOOST_FUSION_GPU_ENABLED
                static type
                call(Sequence& seq)
                {
                    return type(seq);
                }
            };
        };
    }
}}

#endif



/*=============================================================================
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(AUTOBOOST_FUSION_ITERATOR_RANGE_SEGMENTS_HPP_INCLUDED)
#define AUTOBOOST_FUSION_ITERATOR_RANGE_SEGMENTS_HPP_INCLUDED

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/mpl/assert.hpp>
#include <autoboost/fusion/sequence/intrinsic/segments.hpp>
#include <autoboost/fusion/support/is_segmented.hpp>
#include <autoboost/fusion/view/iterator_range/detail/segmented_iterator_range.hpp>

namespace autoboost { namespace fusion
{
    struct iterator_range_tag;

    namespace extension
    {
        template <typename Tag>
        struct segments_impl;

        template <>
        struct segments_impl<iterator_range_tag>
        {
            template <typename Sequence>
            struct apply
            {
                typedef
                    detail::make_segmented_range<
                        typename Sequence::begin_type
                      , typename Sequence::end_type
                    >
                impl;

                AUTOBOOST_MPL_ASSERT((traits::is_segmented<typename impl::type>));

                typedef
                    typename result_of::segments<typename impl::type>::type
                type;

                AUTOBOOST_FUSION_GPU_ENABLED
                static type call(Sequence & seq)
                {
                    return fusion::segments(impl::call(seq.first, seq.last));
                }
            };
        };
    }
}}

#endif

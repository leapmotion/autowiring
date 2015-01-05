/*=============================================================================
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(AUTOBOOST_FUSION_SEGMENTED_END_IMPL_HPP_INCLUDED)
#define AUTOBOOST_FUSION_SEGMENTED_END_IMPL_HPP_INCLUDED

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/mpl/assert.hpp>
#include <autoboost/type_traits/add_const.hpp>
#include <autoboost/type_traits/remove_reference.hpp>
#include <autoboost/fusion/sequence/intrinsic_fwd.hpp>
#include <autoboost/fusion/container/list/cons_fwd.hpp>
#include <autoboost/fusion/support/is_segmented.hpp>

namespace autoboost { namespace fusion
{
    template <typename First, typename Last>
    struct iterator_range;
}}

namespace autoboost { namespace fusion { namespace detail
{
    //auto segmented_end_impl( seq, stack )
    //{
    //    assert(is_segmented(seq));
    //    auto it = end(segments(seq));
    //    return cons(iterator_range(it, it), stack);
    //}

    template <typename Sequence, typename Stack>
    struct segmented_end_impl
    {
        AUTOBOOST_MPL_ASSERT((traits::is_segmented<Sequence>));

        typedef
            typename result_of::end<
                typename remove_reference<
                    typename add_const<
                        typename result_of::segments<Sequence>::type
                    >::type
                >::type
            >::type
        end_type;

        typedef iterator_range<end_type, end_type>  pair_type;
        typedef cons<pair_type, Stack>              type;

        AUTOBOOST_FUSION_GPU_ENABLED
        static type call(Sequence & seq, Stack stack)
        {
            end_type end = fusion::end(fusion::segments(seq));
            return type(pair_type(end, end), stack);
        }
    };

}}}

#endif

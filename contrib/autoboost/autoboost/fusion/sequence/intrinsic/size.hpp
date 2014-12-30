/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SIZE_05052005_0214)
#define FUSION_SIZE_05052005_0214

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/utility/enable_if.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/fusion/sequence/intrinsic_fwd.hpp>
#include <autoboost/fusion/support/tag_of.hpp>
#include <autoboost/fusion/support/is_segmented.hpp>
#include <autoboost/fusion/sequence/intrinsic/detail/segmented_size.hpp>

namespace autoboost { namespace fusion
{
    // Special tags:
    struct sequence_facade_tag;
    struct boost_tuple_tag; // autoboost::tuples::tuple tag
    struct boost_array_tag; // autoboost::array tag
    struct mpl_sequence_tag; // mpl sequence tag
    struct std_pair_tag; // std::pair tag

    namespace extension
    {
        template <typename Tag>
        struct size_impl
        {
            template<typename Sequence>
            struct unsegmented_size : Sequence::size {};

            template <typename Sequence>
            struct apply
              : mpl::if_<
                    traits::is_segmented<Sequence>
                  , detail::segmented_size<Sequence>
                  , unsegmented_size<Sequence>
                >::type
            {};
        };

        template <>
        struct size_impl<sequence_facade_tag>
        {
            template <typename Sequence>
            struct apply : Sequence::template size<Sequence> {};
        };
 
        template <>
        struct size_impl<boost_tuple_tag>;
 
        template <>
        struct size_impl<boost_array_tag>;

        template <>
        struct size_impl<mpl_sequence_tag>;

        template <>
        struct size_impl<std_pair_tag>;
    }

    namespace result_of
    {
        template <typename Sequence>
        struct size
            : extension::size_impl<typename detail::tag_of<Sequence>::type>::
        template apply<Sequence>

        {
            typedef typename extension::size_impl<typename detail::tag_of<Sequence>::type>::
            template apply<Sequence>::type size_application;
            AUTOBOOST_STATIC_CONSTANT(int, value = size_application::value);
        };
    }

    template <typename Sequence>
    AUTOBOOST_FUSION_GPU_ENABLED
    inline typename result_of::size<Sequence>::type
    size(Sequence const&)
    {
        typedef typename result_of::size<Sequence>::type result;
        return result();
    }
}}

#endif

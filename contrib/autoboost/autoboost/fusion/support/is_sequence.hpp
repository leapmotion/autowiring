/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_IS_SEQUENCE_05052005_1002)
#define FUSION_IS_SEQUENCE_05052005_1002

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/fusion/support/sequence_base.hpp>
#include <autoboost/fusion/support/tag_of.hpp>
#include <autoboost/mpl/is_sequence.hpp>
#include <autoboost/mpl/or.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/type_traits/is_convertible.hpp>
#include <autoboost/type_traits/is_same.hpp>

namespace autoboost { namespace fusion
{
    // Special tags:
    struct non_fusion_tag;
    struct boost_tuple_tag; // autoboost::tuples::tuple tag
    struct boost_array_tag; // autoboost::array tag
    struct mpl_sequence_tag; // mpl sequence tag
    struct std_pair_tag; // std::pair tag

    namespace extension
    {
        template <typename T>
        struct is_sequence_impl
        {
            template <typename Sequence>
            struct apply
              : is_convertible<Sequence, detail::from_sequence_convertible_type>
            {};
        };

        template <>
        struct is_sequence_impl<non_fusion_tag>
        {
            template <typename T>
            struct apply : mpl::false_ {};
        };

        template <>
        struct is_sequence_impl<boost_tuple_tag>;

        template <>
        struct is_sequence_impl<boost_array_tag>;

        template <>
        struct is_sequence_impl<mpl_sequence_tag>;

        template <>
        struct is_sequence_impl<std_pair_tag>;
    }

    namespace traits
    {
        template <typename T>
        struct is_sequence
          : mpl::bool_<
                (bool)extension::is_sequence_impl<
                    typename fusion::detail::tag_of<T>::type
                >::template apply<T>::type::value
            >
        {};

        template <typename Sequence, typename Enable = void>
        struct is_native_fusion_sequence
          : is_convertible<Sequence, detail::from_sequence_convertible_type>
        {};
    }
}}

#endif

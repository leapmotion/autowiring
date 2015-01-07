/*=============================================================================
    Copyright (c) 2009 Christopher Schmidt

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef AUTOBOOST_FUSION_VIEW_JOINT_VIEW_DETAIL_DEREF_DATA_IMPL_HPP
#define AUTOBOOST_FUSION_VIEW_JOINT_VIEW_DETAIL_DEREF_DATA_IMPL_HPP

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/fusion/iterator/deref_data.hpp>

namespace autoboost { namespace fusion { namespace extension
{
    template <typename>
    struct deref_data_impl;

    template <>
    struct deref_data_impl<joint_view_iterator_tag>
    {
        template <typename It>
        struct apply
        {
            typedef typename
                result_of::deref_data<typename It::first_type>::type
            type;

            AUTOBOOST_FUSION_GPU_ENABLED
            static type
            call(It const& it)
            {
                return fusion::deref_data(it.first);
            }
        };
    };
}}}

#endif

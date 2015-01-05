/*=============================================================================
    Copyright (c) 2009 Christopher Schmidt

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef AUTOBOOST_FUSION_ITERATOR_DEREF_DATA_HPP
#define AUTOBOOST_FUSION_ITERATOR_DEREF_DATA_HPP

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/fusion/support/tag_of.hpp>

namespace autoboost { namespace fusion
{
    struct iterator_facade_tag;

    namespace extension
    {
        template <typename>
        struct deref_data_impl;

        template <>
        struct deref_data_impl<iterator_facade_tag>
        {
            template <typename It>
            struct apply
              : It::template deref_data<It>
            {};
       };
    }

    namespace result_of
    {
        template <typename It>
        struct deref_data
          : extension::deref_data_impl<typename traits::tag_of<It>::type>::
                template apply<It>
        {};
    }

    template <typename It>
    AUTOBOOST_FUSION_GPU_ENABLED
    typename result_of::deref_data<It>::type
    deref_data(It const& it)
    {
        return result_of::deref_data<It>::call(it);
    }
}}

#endif

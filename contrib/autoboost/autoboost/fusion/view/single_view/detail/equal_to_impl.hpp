/*=============================================================================
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#if !defined(AUTOBOOST_FUSION_SINGLE_VIEW_ITERATOR_JUL_07_2011_1348PM)
#define AUTOBOOST_FUSION_SINGLE_VIEW_ITERATOR_JUL_07_2011_1348PM

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/mpl/assert.hpp>
#include <autoboost/mpl/equal_to.hpp>
#include <autoboost/type_traits/is_same.hpp>
#include <autoboost/type_traits/add_const.hpp>

namespace autoboost { namespace fusion 
{
    struct single_view_iterator_tag;

    namespace extension
    {
        template<typename Tag>
        struct equal_to_impl;

        template<>
        struct equal_to_impl<single_view_iterator_tag>
        {
            template<typename It1, typename It2>
            struct apply
              : mpl::equal_to<typename It1::position, typename It2::position>
            {
                AUTOBOOST_MPL_ASSERT((is_same<typename add_const<typename It1::single_view_type>::type, 
                    typename add_const<typename It2::single_view_type>::type>));
            };
        };
    }
}}

#endif

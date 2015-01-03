/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(AUTOBOOST_FUSION_SINGLE_VIEW_VALUE_OF_IMPL_05052005_0324)
#define AUTOBOOST_FUSION_SINGLE_VIEW_VALUE_OF_IMPL_05052005_0324

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/assert.hpp>
#include <autoboost/mpl/equal_to.hpp>

namespace autoboost { namespace fusion
{
    struct single_view_iterator_tag;

    namespace extension
    {
        template <typename Tag>
        struct value_of_impl;

        template <>
        struct value_of_impl<single_view_iterator_tag>
        {
            template <typename Iterator>
            struct apply
            {
                AUTOBOOST_MPL_ASSERT((mpl::equal_to<typename Iterator::position, mpl::int_<0> >));
                typedef typename Iterator::value_type type;
            };
        };
    }
}}

#endif



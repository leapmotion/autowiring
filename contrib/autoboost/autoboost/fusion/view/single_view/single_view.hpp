/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(AUTOBOOST_FUSION_SINGLE_VIEW_05052005_0335)
#define AUTOBOOST_FUSION_SINGLE_VIEW_05052005_0335

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/fusion/support/detail/access.hpp>
#include <autoboost/fusion/support/detail/as_fusion_element.hpp>
#include <autoboost/fusion/support/sequence_base.hpp>
#include <autoboost/fusion/view/single_view/single_view_iterator.hpp>
#include <autoboost/fusion/view/single_view/detail/at_impl.hpp>
#include <autoboost/fusion/view/single_view/detail/begin_impl.hpp>
#include <autoboost/fusion/view/single_view/detail/end_impl.hpp>
#include <autoboost/fusion/view/single_view/detail/size_impl.hpp>
#include <autoboost/fusion/view/single_view/detail/value_at_impl.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/config.hpp>

#if defined (AUTOBOOST_MSVC)
#  pragma warning(push)
#  pragma warning (disable: 4512) // assignment operator could not be generated.
#endif

namespace autoboost { namespace fusion
{
    struct single_view_tag;
    struct random_access_traversal_tag;
    struct fusion_sequence_tag;

    template <typename T>
    struct single_view : sequence_base<single_view<T> >
    {
        typedef single_view_tag fusion_tag;
        typedef fusion_sequence_tag tag; // this gets picked up by MPL
        typedef random_access_traversal_tag category;
        typedef mpl::true_ is_view;
        typedef mpl::int_<1> size;
        typedef T value_type;

        AUTOBOOST_FUSION_GPU_ENABLED
        single_view()
            : val() {}

        AUTOBOOST_FUSION_GPU_ENABLED explicit single_view(typename detail::call_param<T>::type in_val)
            : val(in_val) {}

        value_type val;
    };
    
    template <typename T>
    AUTOBOOST_FUSION_GPU_ENABLED
    inline single_view<typename detail::as_fusion_element<T>::type>
    make_single_view(T const& v)
    {
        return single_view<typename detail::as_fusion_element<T>::type>(v);
    }
}}

#if defined (AUTOBOOST_MSVC)
#  pragma warning(pop)
#endif

#endif



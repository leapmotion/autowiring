/*=============================================================================
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(AUTOBOOST_FUSION_FIND_FWD_HPP_INCLUDED)
#define AUTOBOOST_FUSION_FIND_FWD_HPP_INCLUDED

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/utility/enable_if.hpp>
#include <autoboost/type_traits/is_const.hpp>

namespace autoboost { namespace fusion
{
    namespace result_of
    {
        template <typename Sequence, typename T>
        struct find;
    }

    template <typename T, typename Sequence>
    AUTOBOOST_FUSION_GPU_ENABLED
    inline typename 
        lazy_disable_if<
            is_const<Sequence>
          , result_of::find<Sequence, T>
        >::type const
    find(Sequence& seq);

    template <typename T, typename Sequence>
    AUTOBOOST_FUSION_GPU_ENABLED
    inline typename result_of::find<Sequence const, T>::type const
    find(Sequence const& seq);
}}

#endif

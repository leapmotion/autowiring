/*=============================================================================
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(AUTOBOOST_FUSION_FIND_IF_FWD_HPP_INCLUDED)
#define AUTOBOOST_FUSION_FIND_IF_FWD_HPP_INCLUDED

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/utility/enable_if.hpp>
#include <autoboost/type_traits/is_const.hpp>

// Forward declaration of find_if algorithm
namespace autoboost { namespace fusion
{
    namespace result_of
    {
        template <typename Sequence, typename Pred>
        struct find_if;
    }

    template <typename Pred, typename Sequence>
    AUTOBOOST_FUSION_GPU_ENABLED
    typename 
        lazy_disable_if<
            is_const<Sequence>
          , result_of::find_if<Sequence, Pred>
        >::type
    find_if(Sequence& seq);

    template <typename Pred, typename Sequence>
    AUTOBOOST_FUSION_GPU_ENABLED
    typename result_of::find_if<Sequence const, Pred>::type const
    find_if(Sequence const& seq);
}}

#endif

// Boost.Range library
//
//  Copyright Neil Groves 2014. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_TRAVERSAL_HPP
#define AUTOBOOST_RANGE_TRAVERSAL_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/range/config.hpp>
#include <autoboost/range/iterator.hpp>
#include <autoboost/iterator/iterator_traits.hpp>

namespace autoboost
{
    template<typename SinglePassRange>
    struct range_traversal
        : iterator_traversal<typename range_iterator<SinglePassRange>::type>
    {
    };
}

#endif

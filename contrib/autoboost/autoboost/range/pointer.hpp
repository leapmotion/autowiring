// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2006. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_POINTER_TYPE_HPP
#define AUTOBOOST_RANGE_POINTER_TYPE_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/range/config.hpp>
#include <autoboost/range/iterator.hpp>
#include <autoboost/iterator/iterator_traits.hpp>

namespace autoboost
{
    template< class T >
    struct range_pointer
            : iterator_pointer< AUTOBOOST_DEDUCED_TYPENAME range_iterator<T>::type >
    { };
}

#endif

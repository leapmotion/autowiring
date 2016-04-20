// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_CONST_REVERSE_ITERATOR_HPP
#define AUTOBOOST_RANGE_CONST_REVERSE_ITERATOR_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/range/reverse_iterator.hpp>
#include <autoboost/type_traits/remove_reference.hpp>

namespace autoboost
{
    //
    // This interface is deprecated, use range_reverse_iterator<const T>
    //

    template< typename C >
    struct range_const_reverse_iterator
            : range_reverse_iterator<
                const AUTOBOOST_DEDUCED_TYPENAME remove_reference<C>::type>
    { };

} // namespace autoboost

#endif

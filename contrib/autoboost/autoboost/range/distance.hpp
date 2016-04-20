// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2006. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_DISTANCE_HPP
#define AUTOBOOST_RANGE_DISTANCE_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/difference_type.hpp>

namespace autoboost
{

    template< class T >
    inline AUTOBOOST_DEDUCED_TYPENAME range_difference<T>::type
    distance( const T& r )
    {
        return std::distance( autoboost::begin( r ), autoboost::end( r ) );
    }

} // namespace 'autoboost'

#endif

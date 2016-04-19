// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_EMPTY_HPP
#define AUTOBOOST_RANGE_EMPTY_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/range/config.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>

namespace autoboost
{

    template< class T >
    inline bool empty( const T& r )
    {
        return autoboost::begin( r ) == autoboost::end( r );
    }

} // namespace 'autoboost'


#endif

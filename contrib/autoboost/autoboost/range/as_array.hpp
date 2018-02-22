// Boost.Range library
//
//  Copyright Thorsten Ottosen 2006. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_AS_ARRAY_HPP
#define AUTOBOOST_RANGE_AS_ARRAY_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/range/iterator_range.hpp>
#include <autoboost/range/detail/str_types.hpp>

namespace autoboost
{

    template< class R >
    inline iterator_range< AUTOBOOST_DEDUCED_TYPENAME range_iterator<R>::type >
    as_array( R& r )
    {
        return autoboost::make_iterator_range( r );
    }

#ifndef AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING

    template< class Range >
    inline autoboost::iterator_range< AUTOBOOST_DEDUCED_TYPENAME range_iterator<const Range>::type >
    as_array( const Range& r )
    {
        return autoboost::make_iterator_range( r );
    }

#endif

}

#endif


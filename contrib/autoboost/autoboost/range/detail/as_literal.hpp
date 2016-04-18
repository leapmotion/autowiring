// Boost.Range library
//
//  Copyright Thorsten Ottosen 2006. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_DETAIL_AS_LITERAL_HPP
#define AUTOBOOST_RANGE_DETAIL_AS_LITERAL_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/range/detail/detail_str.hpp>
#include <autoboost/range/iterator_range.hpp>

namespace autoboost
{
    template< class Range >
    inline iterator_range<AUTOBOOST_DEDUCED_TYPENAME range_iterator<Range>::type>
    as_literal( Range& r )
    {
        return ::autoboost::make_iterator_range( ::autoboost::range_detail::str_begin(r),
                                             ::autoboost::range_detail::str_end(r) );
    }

}

#endif

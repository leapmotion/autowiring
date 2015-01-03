// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_DETAIL_SIZE_TYPE_HPP
#define AUTOBOOST_RANGE_DETAIL_SIZE_TYPE_HPP

#include <autoboost/range/detail/common.hpp>

//////////////////////////////////////////////////////////////////////////////
// missing partial specialization  workaround.
//////////////////////////////////////////////////////////////////////////////

namespace autoboost
{
    namespace range_detail
    {
        template< typename T >
        struct range_size_type_
        {
            template< typename C >
            struct pts
            {
                typedef std::size_t type;
            };
        };

        template<>
        struct range_size_type_<std_container_>
        {
            template< typename C >
            struct pts
            {
                typedef AUTOBOOST_RANGE_DEDUCED_TYPENAME C::size_type type;
            };
        };
    }

    template< typename C >
    class range_size
    {
        typedef typename range_detail::range<C>::type c_type;
    public:
        typedef typename range_detail::range_size_type_<c_type>::AUTOBOOST_NESTED_TEMPLATE pts<C>::type type;
    };
}

#endif


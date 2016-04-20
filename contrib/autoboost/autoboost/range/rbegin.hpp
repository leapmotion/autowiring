// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_RBEGIN_HPP
#define AUTOBOOST_RANGE_RBEGIN_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/range/end.hpp>
#include <autoboost/range/reverse_iterator.hpp>

namespace autoboost
{

#ifdef AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING

template< class C >
inline AUTOBOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type
rbegin( C& c )
{
    return AUTOBOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type( autoboost::end( c ) );
}

#else

template< class C >
inline AUTOBOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type
rbegin( C& c )
{
    typedef AUTOBOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type
        iter_type;
    return iter_type( autoboost::end( c ) );
}

template< class C >
inline AUTOBOOST_DEDUCED_TYPENAME range_reverse_iterator<const C>::type
rbegin( const C& c )
{
    typedef AUTOBOOST_DEDUCED_TYPENAME range_reverse_iterator<const C>::type
        iter_type;
    return iter_type( autoboost::end( c ) );
}

#endif // AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING

template< class T >
inline AUTOBOOST_DEDUCED_TYPENAME range_reverse_iterator<const T>::type
const_rbegin( const T& r )
{
    return autoboost::rbegin( r );
}

} // namespace 'autoboost'

#endif


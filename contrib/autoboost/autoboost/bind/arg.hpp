#ifndef AUTOBOOST_BIND_ARG_HPP_INCLUDED
#define AUTOBOOST_BIND_ARG_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  bind/arg.hpp
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/bind/bind.html for documentation.
//

#include <autoboost/config.hpp>
#include <autoboost/is_placeholder.hpp>
#include <autoboost/static_assert.hpp>

namespace autoboost
{

template< int I > struct arg
{
    AUTOBOOST_CONSTEXPR arg()
    {
    }

    template< class T > AUTOBOOST_CONSTEXPR arg( T const & /* t */ )
    {
        AUTOBOOST_STATIC_ASSERT( I == is_placeholder<T>::value );
    }
};

template< int I > AUTOBOOST_CONSTEXPR bool operator==( arg<I> const &, arg<I> const & )
{
    return true;
}

#if !defined( AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION )

template< int I > struct is_placeholder< arg<I> >
{
    enum _vt { value = I };
};

template< int I > struct is_placeholder< arg<I> (*) () >
{
    enum _vt { value = I };
};

#endif

} // namespace autoboost

#endif // #ifndef AUTOBOOST_BIND_ARG_HPP_INCLUDED

#ifndef AUTOBOOST_SMART_PTR_SHARED_ARRAY_HPP_INCLUDED
#define AUTOBOOST_SMART_PTR_SHARED_ARRAY_HPP_INCLUDED

//
//  shared_array.hpp
//
//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001, 2002, 2012 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/shared_array.htm for documentation.
//

#include <autoboost/config.hpp>   // for broken compiler workarounds

#include <memory>             // TR1 cyclic inclusion fix

#include <autoboost/assert.hpp>
#include <autoboost/checked_delete.hpp>

#include <autoboost/smart_ptr/shared_ptr.hpp>
#include <autoboost/smart_ptr/detail/shared_count.hpp>
#include <autoboost/smart_ptr/detail/sp_nullptr_t.hpp>
#include <autoboost/detail/workaround.hpp>

#include <cstddef>            // for std::ptrdiff_t
#include <algorithm>          // for std::swap
#include <functional>         // for std::less

namespace autoboost
{

//
//  shared_array
//
//  shared_array extends shared_ptr to arrays.
//  The array pointed to is deleted when the last shared_array pointing to it
//  is destroyed or reset.
//

template<class T> class shared_array
{
private:

    // Borland 5.5.1 specific workarounds
    typedef checked_array_deleter<T> deleter;
    typedef shared_array<T> this_type;

public:

    typedef T element_type;

    shared_array() AUTOBOOST_NOEXCEPT : px( 0 ), pn()
    {
    }

#if !defined( AUTOBOOST_NO_CXX11_NULLPTR )

    shared_array( autoboost::detail::sp_nullptr_t ) AUTOBOOST_NOEXCEPT : px( 0 ), pn()
    {
    }

#endif

    template<class Y>
    explicit shared_array( Y * p ): px( p ), pn( p, checked_array_deleter<Y>() )
    {
        autoboost::detail::sp_assert_convertible< Y[], T[] >();
    }

    //
    // Requirements: D's copy constructor must not throw
    //
    // shared_array will release p by calling d(p)
    //

    template<class Y, class D> shared_array( Y * p, D d ): px( p ), pn( p, d )
    {
        autoboost::detail::sp_assert_convertible< Y[], T[] >();
    }

    // As above, but with allocator. A's copy constructor shall not throw.

    template<class Y, class D, class A> shared_array( Y * p, D d, A a ): px( p ), pn( p, d, a )
    {
        autoboost::detail::sp_assert_convertible< Y[], T[] >();
    }

//  generated copy constructor, destructor are fine...

#if !defined( AUTOBOOST_NO_CXX11_RVALUE_REFERENCES )

// ... except in C++0x, move disables the implicit copy

    shared_array( shared_array const & r ) AUTOBOOST_NOEXCEPT : px( r.px ), pn( r.pn )
    {
    }

    shared_array( shared_array && r ) AUTOBOOST_NOEXCEPT : px( r.px ), pn()
    {
        pn.swap( r.pn );
        r.px = 0;
    }

#endif

    // conversion

    template<class Y>
#if !defined( AUTOBOOST_SP_NO_SP_CONVERTIBLE )

    shared_array( shared_array<Y> const & r, typename autoboost::detail::sp_enable_if_convertible< Y[], T[] >::type = autoboost::detail::sp_empty() )

#else

    shared_array( shared_array<Y> const & r )

#endif
    AUTOBOOST_NOEXCEPT : px( r.px ), pn( r.pn ) // never throws
    {
        autoboost::detail::sp_assert_convertible< Y[], T[] >();
    }

    // aliasing

    template< class Y >
    shared_array( shared_array<Y> const & r, element_type * p ) AUTOBOOST_NOEXCEPT : px( p ), pn( r.pn )
    {
    }

    // assignment

    shared_array & operator=( shared_array const & r ) AUTOBOOST_NOEXCEPT
    {
        this_type( r ).swap( *this );
        return *this;
    }

#if !defined(AUTOBOOST_MSVC) || (AUTOBOOST_MSVC >= 1400)

    template<class Y>
    shared_array & operator=( shared_array<Y> const & r ) AUTOBOOST_NOEXCEPT
    {
        this_type( r ).swap( *this );
        return *this;
    }

#endif

#if !defined( AUTOBOOST_NO_CXX11_RVALUE_REFERENCES )

    shared_array & operator=( shared_array && r ) AUTOBOOST_NOEXCEPT
    {
        this_type( static_cast< shared_array && >( r ) ).swap( *this );
        return *this;
    }

    template<class Y>
    shared_array & operator=( shared_array<Y> && r ) AUTOBOOST_NOEXCEPT
    {
        this_type( static_cast< shared_array<Y> && >( r ) ).swap( *this );
        return *this;
    }

#endif

    void reset() AUTOBOOST_NOEXCEPT
    {
        this_type().swap( *this );
    }

    template<class Y> void reset( Y * p ) // Y must be complete
    {
        AUTOBOOST_ASSERT( p == 0 || p != px ); // catch self-reset errors
        this_type( p ).swap( *this );
    }

    template<class Y, class D> void reset( Y * p, D d )
    {
        this_type( p, d ).swap( *this );
    }

    template<class Y, class D, class A> void reset( Y * p, D d, A a )
    {
        this_type( p, d, a ).swap( *this );
    }

    template<class Y> void reset( shared_array<Y> const & r, element_type * p )
    {
        this_type( r, p ).swap( *this );
    }

    T & operator[] (std::ptrdiff_t i) const // never throws (but has a AUTOBOOST_ASSERT in it, so not marked with AUTOBOOST_NOEXCEPT)
    {
        AUTOBOOST_ASSERT(px != 0);
        AUTOBOOST_ASSERT(i >= 0);
        return px[i];
    }

    T * get() const AUTOBOOST_NOEXCEPT
    {
        return px;
    }

// implicit conversion to "bool"
#include <autoboost/smart_ptr/detail/operator_bool.hpp>

    bool unique() const AUTOBOOST_NOEXCEPT
    {
        return pn.unique();
    }

    long use_count() const AUTOBOOST_NOEXCEPT
    {
        return pn.use_count();
    }

    void swap(shared_array<T> & other) AUTOBOOST_NOEXCEPT
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    void * _internal_get_deleter( autoboost::detail::sp_typeinfo const & ti ) const
    {
        return pn.get_deleter( ti );
    }

private:

    template<class Y> friend class shared_array;

    T * px;                     // contained pointer
    detail::shared_count pn;    // reference counter

};  // shared_array

template<class T> inline bool operator==(shared_array<T> const & a, shared_array<T> const & b) AUTOBOOST_NOEXCEPT
{
    return a.get() == b.get();
}

template<class T> inline bool operator!=(shared_array<T> const & a, shared_array<T> const & b) AUTOBOOST_NOEXCEPT
{
    return a.get() != b.get();
}

#if !defined( AUTOBOOST_NO_CXX11_NULLPTR )

template<class T> inline bool operator==( shared_array<T> const & p, autoboost::detail::sp_nullptr_t ) AUTOBOOST_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator==( autoboost::detail::sp_nullptr_t, shared_array<T> const & p ) AUTOBOOST_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator!=( shared_array<T> const & p, autoboost::detail::sp_nullptr_t ) AUTOBOOST_NOEXCEPT
{
    return p.get() != 0;
}

template<class T> inline bool operator!=( autoboost::detail::sp_nullptr_t, shared_array<T> const & p ) AUTOBOOST_NOEXCEPT
{
    return p.get() != 0;
}

#endif

template<class T> inline bool operator<(shared_array<T> const & a, shared_array<T> const & b) AUTOBOOST_NOEXCEPT
{
    return std::less<T*>()(a.get(), b.get());
}

template<class T> void swap(shared_array<T> & a, shared_array<T> & b) AUTOBOOST_NOEXCEPT
{
    a.swap(b);
}

template< class D, class T > D * get_deleter( shared_array<T> const & p )
{
    return static_cast< D * >( p._internal_get_deleter( AUTOBOOST_SP_TYPEID(D) ) );
}

} // namespace autoboost

#endif  // #ifndef AUTOBOOST_SMART_PTR_SHARED_ARRAY_HPP_INCLUDED

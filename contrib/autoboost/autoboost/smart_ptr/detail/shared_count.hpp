#ifndef AUTOBOOST_SMART_PTR_DETAIL_SHARED_COUNT_HPP_INCLUDED
#define AUTOBOOST_SMART_PTR_DETAIL_SHARED_COUNT_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  detail/shared_count.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov and Multi Media Ltd.
//  Copyright 2004-2005 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifdef __BORLANDC__
# pragma warn -8027     // Functions containing try are not expanded inline
#endif

#include <autoboost/config.hpp>
#include <autoboost/checked_delete.hpp>
#include <autoboost/throw_exception.hpp>
#include <autoboost/smart_ptr/bad_weak_ptr.hpp>
#include <autoboost/smart_ptr/detail/sp_counted_base.hpp>
#include <autoboost/smart_ptr/detail/sp_counted_impl.hpp>
#include <autoboost/smart_ptr/detail/sp_disable_deprecated.hpp>
#include <autoboost/detail/workaround.hpp>
// In order to avoid circular dependencies with Boost.TR1
// we make sure that our include of <memory> doesn't try to
// pull in the TR1 headers: that's why we use this header
// rather than including <memory> directly:
#include <autoboost/config/no_tr1/memory.hpp>  // std::auto_ptr
#include <functional>       // std::less

#ifdef AUTOBOOST_NO_EXCEPTIONS
# include <new>              // std::bad_alloc
#endif

#include <autoboost/core/addressof.hpp>

#if defined( AUTOBOOST_SP_DISABLE_DEPRECATED )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace autoboost
{

namespace movelib
{

    template< class T, class D > class unique_ptr;

} // namespace movelib

namespace detail
{

#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)

int const shared_count_id = 0x2C35F101;
int const   weak_count_id = 0x298C38A4;

#endif

struct sp_nothrow_tag {};

template< class D > struct sp_inplace_tag
{
};

template< class T > class sp_reference_wrapper
{
public:

    explicit sp_reference_wrapper( T & t): t_( autoboost::addressof( t ) )
    {
    }

    template< class Y > void operator()( Y * p ) const
    {
        (*t_)( p );
    }

private:

    T * t_;
};

template< class D > struct sp_convert_reference
{
    typedef D type;
};

template< class D > struct sp_convert_reference< D& >
{
    typedef sp_reference_wrapper< D > type;
};

class weak_count;

class shared_count
{
private:

    sp_counted_base * pi_;

#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
    int id_;
#endif

    friend class weak_count;

public:

    shared_count(): pi_(0) // nothrow
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
    }

    template<class Y> explicit shared_count( Y * p ): pi_( 0 )
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
#ifndef AUTOBOOST_NO_EXCEPTIONS

        try
        {
            pi_ = new sp_counted_impl_p<Y>( p );
        }
        catch(...)
        {
            autoboost::checked_delete( p );
            throw;
        }

#else

        pi_ = new sp_counted_impl_p<Y>( p );

        if( pi_ == 0 )
        {
            autoboost::checked_delete( p );
            autoboost::throw_exception( std::bad_alloc() );
        }

#endif
    }

#if defined( AUTOBOOST_MSVC ) && AUTOBOOST_WORKAROUND( AUTOBOOST_MSVC, <= 1200 )
    template<class Y, class D> shared_count( Y * p, D d ): pi_(0)
#else
    template<class P, class D> shared_count( P p, D d ): pi_(0)
#endif
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
#if defined( AUTOBOOST_MSVC ) && AUTOBOOST_WORKAROUND( AUTOBOOST_MSVC, <= 1200 )
        typedef Y* P;
#endif
#ifndef AUTOBOOST_NO_EXCEPTIONS

        try
        {
            pi_ = new sp_counted_impl_pd<P, D>(p, d);
        }
        catch(...)
        {
            d(p); // delete p
            throw;
        }

#else

        pi_ = new sp_counted_impl_pd<P, D>(p, d);

        if(pi_ == 0)
        {
            d(p); // delete p
            autoboost::throw_exception(std::bad_alloc());
        }

#endif
    }

#if !defined( AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING )

    template< class P, class D > shared_count( P p, sp_inplace_tag<D> ): pi_( 0 )
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
#ifndef AUTOBOOST_NO_EXCEPTIONS

        try
        {
            pi_ = new sp_counted_impl_pd< P, D >( p );
        }
        catch( ... )
        {
            D::operator_fn( p ); // delete p
            throw;
        }

#else

        pi_ = new sp_counted_impl_pd< P, D >( p );

        if( pi_ == 0 )
        {
            D::operator_fn( p ); // delete p
            autoboost::throw_exception( std::bad_alloc() );
        }

#endif // #ifndef AUTOBOOST_NO_EXCEPTIONS
    }

#endif // !defined( AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING )

    template<class P, class D, class A> shared_count( P p, D d, A a ): pi_( 0 )
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
        typedef sp_counted_impl_pda<P, D, A> impl_type;

#if !defined( AUTOBOOST_NO_CXX11_ALLOCATOR )

        typedef typename std::allocator_traits<A>::template rebind_alloc< impl_type > A2;

#else

        typedef typename A::template rebind< impl_type >::other A2;

#endif

        A2 a2( a );

#ifndef AUTOBOOST_NO_EXCEPTIONS

        try
        {
#if !defined( AUTOBOOST_NO_CXX11_ALLOCATOR )

            impl_type * pi = std::allocator_traits<A2>::allocate( a2, 1 );
            pi_ = pi;
            std::allocator_traits<A2>::construct( a2, pi, p, d, a );

#else

            pi_ = a2.allocate( 1, static_cast< impl_type* >( 0 ) );
            ::new( static_cast< void* >( pi_ ) ) impl_type( p, d, a );

#endif
        }
        catch(...)
        {
            d( p );

            if( pi_ != 0 )
            {
                a2.deallocate( static_cast< impl_type* >( pi_ ), 1 );
            }

            throw;
        }

#else

#if !defined( AUTOBOOST_NO_CXX11_ALLOCATOR )

        impl_type * pi = std::allocator_traits<A2>::allocate( a2, 1 );
        pi_ = pi;

#else

        pi_ = a2.allocate( 1, static_cast< impl_type* >( 0 ) );

#endif

        if( pi_ != 0 )
        {
#if !defined( AUTOBOOST_NO_CXX11_ALLOCATOR )

            std::allocator_traits<A2>::construct( a2, pi, p, d, a );

#else

            ::new( static_cast< void* >( pi_ ) ) impl_type( p, d, a );

#endif
        }
        else
        {
            d( p );
            autoboost::throw_exception( std::bad_alloc() );
        }

#endif
    }

#if !defined( AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING )

    template< class P, class D, class A > shared_count( P p, sp_inplace_tag< D >, A a ): pi_( 0 )
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
        typedef sp_counted_impl_pda< P, D, A > impl_type;

#if !defined( AUTOBOOST_NO_CXX11_ALLOCATOR )

        typedef typename std::allocator_traits<A>::template rebind_alloc< impl_type > A2;

#else

        typedef typename A::template rebind< impl_type >::other A2;

#endif

        A2 a2( a );

#ifndef AUTOBOOST_NO_EXCEPTIONS

        try
        {
#if !defined( AUTOBOOST_NO_CXX11_ALLOCATOR )

            impl_type * pi = std::allocator_traits<A2>::allocate( a2, 1 );
            pi_ = pi;
            std::allocator_traits<A2>::construct( a2, pi, p, a );

#else

            pi_ = a2.allocate( 1, static_cast< impl_type* >( 0 ) );
            ::new( static_cast< void* >( pi_ ) ) impl_type( p, a );

#endif
        }
        catch(...)
        {
            D::operator_fn( p );

            if( pi_ != 0 )
            {
                a2.deallocate( static_cast< impl_type* >( pi_ ), 1 );
            }

            throw;
        }

#else

#if !defined( AUTOBOOST_NO_CXX11_ALLOCATOR )

        impl_type * pi = std::allocator_traits<A2>::allocate( a2, 1 );
        pi_ = pi;

#else

        pi_ = a2.allocate( 1, static_cast< impl_type* >( 0 ) );

#endif

        if( pi_ != 0 )
        {
#if !defined( AUTOBOOST_NO_CXX11_ALLOCATOR )

            std::allocator_traits<A2>::construct( a2, pi, p, a );

#else

            ::new( static_cast< void* >( pi_ ) ) impl_type( p, a );

#endif
        }
        else
        {
            D::operator_fn( p );
            autoboost::throw_exception( std::bad_alloc() );
        }

#endif // #ifndef AUTOBOOST_NO_EXCEPTIONS
    }

#endif // !defined( AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING )

#ifndef AUTOBOOST_NO_AUTO_PTR

    // auto_ptr<Y> is special cased to provide the strong guarantee

    template<class Y>
    explicit shared_count( std::auto_ptr<Y> & r ): pi_( new sp_counted_impl_p<Y>( r.get() ) )
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
#ifdef AUTOBOOST_NO_EXCEPTIONS

        if( pi_ == 0 )
        {
            autoboost::throw_exception(std::bad_alloc());
        }

#endif

        r.release();
    }

#endif

#if !defined( AUTOBOOST_NO_CXX11_SMART_PTR )

    template<class Y, class D>
    explicit shared_count( std::unique_ptr<Y, D> & r ): pi_( 0 )
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
        typedef typename sp_convert_reference<D>::type D2;

        D2 d2( r.get_deleter() );
        pi_ = new sp_counted_impl_pd< typename std::unique_ptr<Y, D>::pointer, D2 >( r.get(), d2 );

#ifdef AUTOBOOST_NO_EXCEPTIONS

        if( pi_ == 0 )
        {
            autoboost::throw_exception( std::bad_alloc() );
        }

#endif

        r.release();
    }

#endif

    template<class Y, class D>
    explicit shared_count( autoboost::movelib::unique_ptr<Y, D> & r ): pi_( 0 )
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
        typedef typename sp_convert_reference<D>::type D2;

        D2 d2( r.get_deleter() );
        pi_ = new sp_counted_impl_pd< typename autoboost::movelib::unique_ptr<Y, D>::pointer, D2 >( r.get(), d2 );

#ifdef AUTOBOOST_NO_EXCEPTIONS

        if( pi_ == 0 )
        {
            autoboost::throw_exception( std::bad_alloc() );
        }

#endif

        r.release();
    }

    ~shared_count() // nothrow
    {
        if( pi_ != 0 ) pi_->release();
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        id_ = 0;
#endif
    }

    shared_count(shared_count const & r): pi_(r.pi_) // nothrow
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
        if( pi_ != 0 ) pi_->add_ref_copy();
    }

#if !defined( AUTOBOOST_NO_CXX11_RVALUE_REFERENCES )

    shared_count(shared_count && r): pi_(r.pi_) // nothrow
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
    {
        r.pi_ = 0;
    }

#endif

    explicit shared_count(weak_count const & r); // throws bad_weak_ptr when r.use_count() == 0
    shared_count( weak_count const & r, sp_nothrow_tag ); // constructs an empty *this when r.use_count() == 0

    shared_count & operator= (shared_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;

        if( tmp != pi_ )
        {
            if( tmp != 0 ) tmp->add_ref_copy();
            if( pi_ != 0 ) pi_->release();
            pi_ = tmp;
        }

        return *this;
    }

    void swap(shared_count & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }

    long use_count() const // nothrow
    {
        return pi_ != 0? pi_->use_count(): 0;
    }

    bool unique() const // nothrow
    {
        return use_count() == 1;
    }

    bool empty() const // nothrow
    {
        return pi_ == 0;
    }

    friend inline bool operator==(shared_count const & a, shared_count const & b)
    {
        return a.pi_ == b.pi_;
    }

    friend inline bool operator<(shared_count const & a, shared_count const & b)
    {
        return std::less<sp_counted_base *>()( a.pi_, b.pi_ );
    }

    void * get_deleter( sp_typeinfo const & ti ) const
    {
        return pi_? pi_->get_deleter( ti ): 0;
    }

    void * get_untyped_deleter() const
    {
        return pi_? pi_->get_untyped_deleter(): 0;
    }
};


class weak_count
{
private:

    sp_counted_base * pi_;

#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
    int id_;
#endif

    friend class shared_count;

public:

    weak_count(): pi_(0) // nothrow
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(weak_count_id)
#endif
    {
    }

    weak_count(shared_count const & r): pi_(r.pi_) // nothrow
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(weak_count_id)
#endif
    {
        if(pi_ != 0) pi_->weak_add_ref();
    }

    weak_count(weak_count const & r): pi_(r.pi_) // nothrow
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(weak_count_id)
#endif
    {
        if(pi_ != 0) pi_->weak_add_ref();
    }

// Move support

#if !defined( AUTOBOOST_NO_CXX11_RVALUE_REFERENCES )

    weak_count(weak_count && r): pi_(r.pi_) // nothrow
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(weak_count_id)
#endif
    {
        r.pi_ = 0;
    }

#endif

    ~weak_count() // nothrow
    {
        if(pi_ != 0) pi_->weak_release();
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        id_ = 0;
#endif
    }

    weak_count & operator= (shared_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;

        if( tmp != pi_ )
        {
            if(tmp != 0) tmp->weak_add_ref();
            if(pi_ != 0) pi_->weak_release();
            pi_ = tmp;
        }

        return *this;
    }

    weak_count & operator= (weak_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;

        if( tmp != pi_ )
        {
            if(tmp != 0) tmp->weak_add_ref();
            if(pi_ != 0) pi_->weak_release();
            pi_ = tmp;
        }

        return *this;
    }

    void swap(weak_count & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }

    long use_count() const // nothrow
    {
        return pi_ != 0? pi_->use_count(): 0;
    }

    bool empty() const // nothrow
    {
        return pi_ == 0;
    }

    friend inline bool operator==(weak_count const & a, weak_count const & b)
    {
        return a.pi_ == b.pi_;
    }

    friend inline bool operator<(weak_count const & a, weak_count const & b)
    {
        return std::less<sp_counted_base *>()(a.pi_, b.pi_);
    }
};

inline shared_count::shared_count( weak_count const & r ): pi_( r.pi_ )
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
{
    if( pi_ == 0 || !pi_->add_ref_lock() )
    {
        autoboost::throw_exception( autoboost::bad_weak_ptr() );
    }
}

inline shared_count::shared_count( weak_count const & r, sp_nothrow_tag ): pi_( r.pi_ )
#if defined(AUTOBOOST_SP_ENABLE_DEBUG_HOOKS)
        , id_(shared_count_id)
#endif
{
    if( pi_ != 0 && !pi_->add_ref_lock() )
    {
        pi_ = 0;
    }
}

} // namespace detail

} // namespace autoboost

#if defined( AUTOBOOST_SP_DISABLE_DEPRECATED )
#pragma GCC diagnostic pop
#endif

#ifdef __BORLANDC__
# pragma warn .8027     // Functions containing try are not expanded inline
#endif

#endif  // #ifndef AUTOBOOST_SMART_PTR_DETAIL_SHARED_COUNT_HPP_INCLUDED

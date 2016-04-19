/*
 * Copyright (c) 2012-2014 Glen Joseph Fernandes
 * glenfe at live dot com
 *
 * Distributed under the Boost Software License,
 * Version 1.0. (See accompanying file LICENSE_1_0.txt
 * or copy at http://autoboost.org/LICENSE_1_0.txt)
 */
#ifndef AUTOBOOST_SMART_PTR_ALLOCATE_SHARED_ARRAY_HPP
#define AUTOBOOST_SMART_PTR_ALLOCATE_SHARED_ARRAY_HPP

#include <autoboost/smart_ptr/detail/array_count_impl.hpp>
#include <autoboost/smart_ptr/detail/sp_if_array.hpp>

namespace autoboost {
    template<class T, class A>
    inline typename autoboost::detail::sp_if_array<T>::type
    allocate_shared(const A& allocator, std::size_t size) {
        typedef typename autoboost::detail::array_inner<T>::type T1;
        typedef typename autoboost::detail::array_base<T1>::type T2;
        typedef autoboost::detail::ms_init_tag R1;
        typedef autoboost::detail::as_allocator<A, T, R1> A1;
        typedef autoboost::detail::ms_in_allocator_tag D1;
        std::size_t n1 = size * autoboost::detail::array_total<T1>::size;
        T1* p1 = 0;
        T2* p2 = 0;
        D1 d1;
        A1 a1(allocator, size, &p2);
        shared_ptr<T> s1(p1, d1, a1);
        A1* a2 = static_cast<A1*>(s1._internal_get_untyped_deleter());
        a2->set(0);
#if !defined(AUTOBOOST_NO_CXX11_ALLOCATOR)
        autoboost::detail::as_init(allocator, p2, n1);
#else
        autoboost::detail::ms_init(p2, n1);
#endif
        a2->set(p2);
        p1 = reinterpret_cast<T1*>(p2);
        return shared_ptr<T>(s1, p1);
    }

    template<class T, class A>
    inline typename autoboost::detail::sp_if_size_array<T>::type
    allocate_shared(const A& allocator) {
        typedef typename autoboost::detail::array_inner<T>::type T1;
        typedef typename autoboost::detail::array_base<T1>::type T2;
        typedef autoboost::detail::ms_init_tag R1;
        typedef autoboost::detail::as_allocator<A, T, R1> A1;
        typedef autoboost::detail::ms_in_allocator_tag D1;
        enum {
            N = autoboost::detail::array_total<T>::size
        };
        T1* p1 = 0;
        T2* p2 = 0;
        D1 d1;
        A1 a1(allocator, &p2);
        shared_ptr<T> s1(p1, d1, a1);
        A1* a2 = static_cast<A1*>(s1._internal_get_untyped_deleter());
        a2->set(0);
#if !defined(AUTOBOOST_NO_CXX11_ALLOCATOR)
        autoboost::detail::as_init(allocator, p2, N);
#else
        autoboost::detail::ms_init(p2, N);
#endif
        a2->set(p2);
        p1 = reinterpret_cast<T1*>(p2);
        return shared_ptr<T>(s1, p1);
    }

    template<class T, class A>
    inline typename autoboost::detail::sp_if_array<T>::type
    allocate_shared(const A& allocator, std::size_t size,
        const typename autoboost::detail::array_inner<T>::type& value) {
        typedef typename autoboost::detail::array_inner<T>::type T1;
        typedef typename autoboost::detail::array_base<T1>::type T2;
        typedef const T2 T3;
        typedef autoboost::detail::ms_init_tag R1;
        typedef autoboost::detail::as_allocator<A, T, R1> A1;
        typedef autoboost::detail::ms_in_allocator_tag D1;
        enum {
            M = autoboost::detail::array_total<T1>::size
        };
        std::size_t n1 = M * size;
        T1* p1 = 0;
        T2* p2 = 0;
        T3* p3 = reinterpret_cast<T3*>(&value);
        D1 d1;
        A1 a1(allocator, size, &p2);
        shared_ptr<T> s1(p1, d1, a1);
        A1* a2 = static_cast<A1*>(s1._internal_get_untyped_deleter());
        a2->set(0);
#if !defined(AUTOBOOST_NO_CXX11_ALLOCATOR)
        autoboost::detail::as_init<T2, A, M>(allocator, p2, n1, p3);
#else
        autoboost::detail::ms_init<T2, M>(p2, n1, p3);
#endif
        a2->set(p2);
        p1 = reinterpret_cast<T1*>(p2);
        return shared_ptr<T>(s1, p1);
    }

    template<class T, class A>
    inline typename autoboost::detail::sp_if_size_array<T>::type
    allocate_shared(const A& allocator,
        const typename autoboost::detail::array_inner<T>::type& value) {
        typedef typename autoboost::detail::array_inner<T>::type T1;
        typedef typename autoboost::detail::array_base<T1>::type T2;
        typedef const T2 T3;
        typedef autoboost::detail::ms_init_tag R1;
        typedef autoboost::detail::as_allocator<A, T, R1> A1;
        typedef autoboost::detail::ms_in_allocator_tag D1;
        enum {
            N = autoboost::detail::array_total<T>::size,
            M = autoboost::detail::array_total<T1>::size
        };
        T1* p1 = 0;
        T2* p2 = 0;
        T3* p3 = reinterpret_cast<T3*>(&value);
        D1 d1;
        A1 a1(allocator, &p2);
        shared_ptr<T> s1(p1, d1, a1);
        A1* a2 = static_cast<A1*>(s1._internal_get_untyped_deleter());
        a2->set(0);
#if !defined(AUTOBOOST_NO_CXX11_ALLOCATOR)
        autoboost::detail::as_init<T2, A, M>(allocator, p2, N, p3);
#else
        autoboost::detail::ms_init<T2,  M>(p2, N, p3);
#endif
        a2->set(p2);
        p1 = reinterpret_cast<T1*>(p2);
        return shared_ptr<T>(s1, p1);
    }

    template<class T, class A>
    inline typename autoboost::detail::sp_if_array<T>::type
    allocate_shared_noinit(const A& allocator, std::size_t size) {
        typedef typename autoboost::detail::array_inner<T>::type T1;
        typedef typename autoboost::detail::array_base<T1>::type T2;
        typedef autoboost::detail::ms_noinit_tag R1;
        typedef autoboost::detail::as_allocator<A, T, R1> A1;
        typedef autoboost::detail::ms_in_allocator_tag D1;
        std::size_t n1 = size * autoboost::detail::array_total<T1>::size;
        T1* p1 = 0;
        T2* p2 = 0;
        D1 d1;
        A1 a1(allocator, size, &p2);
        shared_ptr<T> s1(p1, d1, a1);
        A1* a2 = static_cast<A1*>(s1._internal_get_untyped_deleter());
        a2->set(0);
        autoboost::detail::ms_noinit(p2, n1);
        a2->set(p2);
        p1 = reinterpret_cast<T1*>(p2);
        return shared_ptr<T>(s1, p1);
    }

    template<class T, class A>
    inline typename autoboost::detail::sp_if_size_array<T>::type
    allocate_shared_noinit(const A& allocator) {
        typedef typename autoboost::detail::array_inner<T>::type T1;
        typedef typename autoboost::detail::array_base<T1>::type T2;
        typedef autoboost::detail::ms_noinit_tag R1;
        typedef autoboost::detail::as_allocator<A, T, R1> A1;
        typedef autoboost::detail::ms_in_allocator_tag D1;
        enum {
            N = autoboost::detail::array_total<T>::size
        };
        T1* p1 = 0;
        T2* p2 = 0;
        D1 d1;
        A1 a1(allocator, &p2);
        shared_ptr<T> s1(p1, d1, a1);
        A1* a2 = static_cast<A1*>(s1._internal_get_untyped_deleter());
        a2->set(0);
        autoboost::detail::ms_noinit(p2, N);
        a2->set(p2);
        p1 = reinterpret_cast<T1*>(p2);
        return shared_ptr<T>(s1, p1);
    }
}

#endif

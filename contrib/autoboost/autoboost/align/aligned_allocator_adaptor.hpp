/*
Copyright 2014-2016 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_ALIGNED_ALLOCATOR_ADAPTOR_HPP
#define AUTOBOOST_ALIGN_ALIGNED_ALLOCATOR_ADAPTOR_HPP

#include <autoboost/align/detail/addressof.hpp>
#include <autoboost/align/detail/is_alignment_constant.hpp>
#include <autoboost/align/detail/max_align.hpp>
#include <autoboost/align/detail/max_size.hpp>
#include <autoboost/align/align.hpp>
#include <autoboost/align/aligned_allocator_adaptor_forward.hpp>
#include <autoboost/align/alignment_of.hpp>
#include <autoboost/static_assert.hpp>
#include <new>

#if !defined(AUTOBOOST_NO_CXX11_ALLOCATOR)
#include <memory>
#endif

#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
#include <utility>
#endif

namespace autoboost {
namespace alignment {

template<class Allocator, std::size_t Alignment>
class aligned_allocator_adaptor
    : public Allocator {
    AUTOBOOST_STATIC_ASSERT(detail::
        is_alignment_constant<Alignment>::value);

#if !defined(AUTOBOOST_NO_CXX11_ALLOCATOR)
    typedef std::allocator_traits<Allocator> traits;

    typedef typename traits::
        template rebind_alloc<char> char_alloc;

    typedef typename traits::
        template rebind_traits<char> char_traits;

    typedef typename char_traits::pointer char_ptr;
#else
    typedef typename Allocator::
        template rebind<char>::other char_alloc;

    typedef typename char_alloc::pointer char_ptr;
#endif

public:
#if !defined(AUTOBOOST_NO_CXX11_ALLOCATOR)
    typedef typename traits::value_type value_type;
    typedef typename traits::size_type size_type;
#else
    typedef typename Allocator::value_type value_type;
    typedef typename Allocator::size_type size_type;
#endif

    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef void* void_pointer;
    typedef const void* const_void_pointer;
    typedef std::ptrdiff_t difference_type;

private:
    enum {
        min_align = detail::max_size<Alignment,
            detail::max_align<value_type, char_ptr>::value>::value
    };

public:
    template<class U>
    struct rebind {
#if !defined(AUTOBOOST_NO_CXX11_ALLOCATOR)
        typedef aligned_allocator_adaptor<typename traits::
            template rebind_alloc<U>, Alignment> other;
#else
        typedef aligned_allocator_adaptor<typename Allocator::
            template rebind<U>::other, Alignment> other;
#endif
    };

#if !defined(AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS)
    aligned_allocator_adaptor() = default;
#else
    aligned_allocator_adaptor()
        : Allocator() { }
#endif

#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
    template<class A>
    explicit aligned_allocator_adaptor(A&& alloc) AUTOBOOST_NOEXCEPT
        : Allocator(std::forward<A>(alloc)) { }
#else
    template<class A>
    explicit aligned_allocator_adaptor(const A& alloc) AUTOBOOST_NOEXCEPT
        : Allocator(alloc) { }
#endif

    template<class U>
    aligned_allocator_adaptor(const aligned_allocator_adaptor<U,
        Alignment>& other) AUTOBOOST_NOEXCEPT
        : Allocator(other.base()) { }

    Allocator& base() AUTOBOOST_NOEXCEPT {
        return static_cast<Allocator&>(*this);
    }

    const Allocator& base() const AUTOBOOST_NOEXCEPT {
        return static_cast<const Allocator&>(*this);
    }

    pointer allocate(size_type size) {
        std::size_t s = size * sizeof(value_type);
        std::size_t n = s + min_align - 1;
        char_alloc a(base());
        char_ptr p = a.allocate(sizeof p + n);
        void* r = detail::addressof(*p) + sizeof p;
        (void)align(min_align, s, r, n);
        ::new(static_cast<void*>(static_cast<char_ptr*>(r)
            - 1)) char_ptr(p);
        return static_cast<pointer>(r);
    }

    pointer allocate(size_type size, const_void_pointer hint) {
        std::size_t s = size * sizeof(value_type);
        std::size_t n = s + min_align - 1;
        char_ptr h = char_ptr();
        if (hint) {
            h = *(static_cast<const char_ptr*>(hint) - 1);
        }
        char_alloc a(base());
#if !defined(AUTOBOOST_NO_CXX11_ALLOCATOR)
        char_ptr p = char_traits::allocate(a, sizeof p + n, h);
#else
        char_ptr p = a.allocate(sizeof p + n, h);
#endif
        void* r = detail::addressof(*p) + sizeof p;
        (void)align(min_align, s, r, n);
        ::new(static_cast<void*>(static_cast<char_ptr*>(r)
            - 1)) char_ptr(p);
        return static_cast<pointer>(r);
    }

    void deallocate(pointer ptr, size_type size) {
        char_ptr* p = reinterpret_cast<char_ptr*>(ptr) - 1;
        char_ptr r = *p;
        p->~char_ptr();
        char_alloc a(base());
        a.deallocate(r, sizeof r + size * sizeof(value_type) +
            min_align - 1);
    }
};

template<class A, class B, std::size_t Alignment>
inline bool
operator==(const aligned_allocator_adaptor<A, Alignment>& a,
    const aligned_allocator_adaptor<B, Alignment>& b) AUTOBOOST_NOEXCEPT
{
    return a.base() == b.base();
}

template<class A, class B, std::size_t Alignment>
inline bool
operator!=(const aligned_allocator_adaptor<A, Alignment>& a,
    const aligned_allocator_adaptor<B, Alignment>& b) AUTOBOOST_NOEXCEPT
{
    return !(a == b);
}

} /* alignment */
} /* autoboost */

#endif

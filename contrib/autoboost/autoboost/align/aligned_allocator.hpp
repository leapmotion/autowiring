/*
(c) 2014-2015 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software
License, Version 1.0.
http://autoboost.org/LICENSE_1_0.txt
*/
#ifndef AUTOBOOST_ALIGN_ALIGNED_ALLOCATOR_HPP
#define AUTOBOOST_ALIGN_ALIGNED_ALLOCATOR_HPP

#include <autoboost/config.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/throw_exception.hpp>
#include <autoboost/align/aligned_alloc.hpp>
#include <autoboost/align/aligned_allocator_forward.hpp>
#include <autoboost/align/alignment_of.hpp>
#include <autoboost/align/detail/addressof.hpp>
#include <autoboost/align/detail/is_alignment_constant.hpp>
#include <autoboost/align/detail/max_objects.hpp>
#include <autoboost/align/detail/max_size.hpp>
#include <new>

#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
#include <utility>
#endif

namespace autoboost {
namespace alignment {

template<class T, std::size_t Alignment>
class aligned_allocator {
    AUTOBOOST_STATIC_ASSERT(detail::
        is_alignment_constant<Alignment>::value);

public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef void* void_pointer;
    typedef const void* const_void_pointer;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T& reference;
    typedef const T& const_reference;

private:
    enum {
        min_align = detail::max_size<Alignment,
            alignment_of<value_type>::value>::value
    };

public:
    template<class U>
    struct rebind {
        typedef aligned_allocator<U, Alignment> other;
    };

#if !defined(AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS)
    aligned_allocator() = default;
#else
    aligned_allocator() AUTOBOOST_NOEXCEPT {
    }
#endif

    template<class U>
    aligned_allocator(const aligned_allocator<U, Alignment>&)
        AUTOBOOST_NOEXCEPT {
    }

    pointer address(reference value) const AUTOBOOST_NOEXCEPT {
        return detail::addressof(value);
    }

    const_pointer address(const_reference value) const
        AUTOBOOST_NOEXCEPT {
        return detail::addressof(value);
    }

    pointer allocate(size_type size, const_void_pointer = 0) {
        void* p = aligned_alloc(min_align, sizeof(T) * size);
        if (size > 0 && !p) {
            ::autoboost::throw_exception(std::bad_alloc());
        }
        return static_cast<T*>(p);
    }

    void deallocate(pointer ptr, size_type) {
        ::autoboost::alignment::aligned_free(ptr);
    }

    AUTOBOOST_CONSTEXPR size_type max_size() const AUTOBOOST_NOEXCEPT {
        return detail::max_objects<T>::value;
    }

#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template<class U, class... Args>
    void construct(U* ptr, Args&&... args) {
        void* p = ptr;
        ::new(p) U(std::forward<Args>(args)...);
    }
#else
    template<class U, class V>
    void construct(U* ptr, V&& value) {
        void* p = ptr;
        ::new(p) U(std::forward<V>(value));
    }
#endif
#else
    template<class U, class V>
    void construct(U* ptr, const V& value) {
        void* p = ptr;
        ::new(p) U(value);
    }
#endif

    template<class U>
    void construct(U* ptr) {
        void* p = ptr;
        ::new(p) U();
    }

    template<class U>
    void destroy(U* ptr) {
        (void)ptr;
        ptr->~U();
    }
};

template<std::size_t Alignment>
class aligned_allocator<void, Alignment> {
    AUTOBOOST_STATIC_ASSERT(detail::
        is_alignment_constant<Alignment>::value);

public:
    typedef void value_type;
    typedef void* pointer;
    typedef const void* const_pointer;

    template<class U>
    struct rebind {
        typedef aligned_allocator<U, Alignment> other;
    };
};

template<class T1, class T2, std::size_t Alignment>
inline bool operator==(const aligned_allocator<T1,
    Alignment>&, const aligned_allocator<T2,
    Alignment>&) AUTOBOOST_NOEXCEPT
{
    return true;
}

template<class T1, class T2, std::size_t Alignment>
inline bool operator!=(const aligned_allocator<T1,
    Alignment>&, const aligned_allocator<T2,
    Alignment>&) AUTOBOOST_NOEXCEPT
{
    return false;
}

} /* .alignment */
} /* .boost */

#endif

/*
Copyright (C) 2002 Brad King (brad.king@kitware.com)
                   Douglas Gregor (gregod@cs.rpi.edu)

Copyright (C) 2002, 2008, 2013 Peter Dimov

Copyright (C) 2017 Glen Joseph Fernandes (glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_CORE_ADDRESSOF_HPP
#define AUTOBOOST_CORE_ADDRESSOF_HPP

#include <autoboost/config.hpp>

#if defined(AUTOBOOST_MSVC_FULL_VER) && AUTOBOOST_MSVC_FULL_VER >= 190024215
#define AUTOBOOST_CORE_HAS_BUILTIN_ADDRESSOF
#elif defined(AUTOBOOST_GCC) && AUTOBOOST_GCC >= 70000
#define AUTOBOOST_CORE_HAS_BUILTIN_ADDRESSOF
#elif defined(__has_builtin)
#if __has_builtin(__builtin_addressof)
#define AUTOBOOST_CORE_HAS_BUILTIN_ADDRESSOF
#endif
#endif

#if defined(AUTOBOOST_CORE_HAS_BUILTIN_ADDRESSOF)
#if defined(AUTOBOOST_NO_CXX11_CONSTEXPR)
#define AUTOBOOST_CORE_NO_CONSTEXPR_ADDRESSOF
#endif

namespace autoboost {

template<class T>
AUTOBOOST_CONSTEXPR inline T*
addressof(T& o) AUTOBOOST_NOEXCEPT
{
    return __builtin_addressof(o);
}

} /* autoboost */
#else
#include <autoboost/detail/workaround.hpp>
#include <cstddef>

namespace autoboost {
namespace detail {

template<class T>
class addressof_ref {
public:
    AUTOBOOST_FORCEINLINE addressof_ref(T& o) AUTOBOOST_NOEXCEPT
        : o_(o) { }
    AUTOBOOST_FORCEINLINE operator T&() const AUTOBOOST_NOEXCEPT {
        return o_;
    }
private:
    addressof_ref& operator=(const addressof_ref&);
    T& o_;
};

template<class T>
struct address_of {
    static AUTOBOOST_FORCEINLINE T* get(T& o, long) AUTOBOOST_NOEXCEPT {
        return reinterpret_cast<T*>(&
            const_cast<char&>(reinterpret_cast<const volatile char&>(o)));
    }
    static AUTOBOOST_FORCEINLINE T* get(T* p, int) AUTOBOOST_NOEXCEPT {
        return p;
    }
};

#if !defined(AUTOBOOST_NO_CXX11_NULLPTR)
#if !defined(AUTOBOOST_NO_CXX11_DECLTYPE) && \
    (defined(__INTEL_COMPILER) || \
        (defined(__clang__) && !defined(_LIBCPP_VERSION)))
typedef decltype(nullptr) addressof_null_t;
#else
typedef std::nullptr_t addressof_null_t;
#endif

template<>
struct address_of<addressof_null_t> {
    typedef addressof_null_t type;
    static AUTOBOOST_FORCEINLINE type* get(type& o, int) AUTOBOOST_NOEXCEPT {
        return &o;
    }
};

template<>
struct address_of<const addressof_null_t> {
    typedef const addressof_null_t type;
    static AUTOBOOST_FORCEINLINE type* get(type& o, int) AUTOBOOST_NOEXCEPT {
        return &o;
    }
};

template<>
struct address_of<volatile addressof_null_t> {
    typedef volatile addressof_null_t type;
    static AUTOBOOST_FORCEINLINE type* get(type& o, int) AUTOBOOST_NOEXCEPT {
        return &o;
    }
};

template<>
struct address_of<const volatile addressof_null_t> {
    typedef const volatile addressof_null_t type;
    static AUTOBOOST_FORCEINLINE type* get(type& o, int) AUTOBOOST_NOEXCEPT {
        return &o;
    }
};
#endif

} /* detail */

#if defined(AUTOBOOST_NO_CXX11_SFINAE_EXPR) || \
    defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) || \
    defined(AUTOBOOST_NO_CXX11_CONSTEXPR) || \
    defined(AUTOBOOST_NO_CXX11_DECLTYPE)
#define AUTOBOOST_CORE_NO_CONSTEXPR_ADDRESSOF

template<class T>
AUTOBOOST_FORCEINLINE T*
addressof(T& o) AUTOBOOST_NOEXCEPT
{
#if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610)) || \
    AUTOBOOST_WORKAROUND(__SUNPRO_CC, <= 0x5120)
    return detail::address_of<T>::get(o, 0);
#else
    return detail::address_of<T>::get(detail::addressof_ref<T>(o), 0);
#endif
}

#if AUTOBOOST_WORKAROUND(__SUNPRO_CC, AUTOBOOST_TESTED_AT(0x590))
namespace detail {

template<class T>
struct addressof_result {
    typedef T* type;
};

} /* detail */

template<class T, std::size_t N>
AUTOBOOST_FORCEINLINE typename detail::addressof_result<T[N]>::type
addressof(T (&o)[N]) AUTOBOOST_NOEXCEPT
{
    return &o;
}
#endif

#if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))
template<class T, std::size_t N>
AUTOBOOST_FORCEINLINE
T (*addressof(T (&o)[N]) AUTOBOOST_NOEXCEPT)[N]
{
   return reinterpret_cast<T(*)[N]>(&o);
}

template<class T, std::size_t N>
AUTOBOOST_FORCEINLINE
const T (*addressof(const T (&o)[N]) AUTOBOOST_NOEXCEPT)[N]
{
   return reinterpret_cast<const T(*)[N]>(&o);
}
#endif
#else
namespace detail {

template<class T>
T&& addressof_declval() AUTOBOOST_NOEXCEPT;

template<class>
struct addressof_void {
    typedef void type;
};

template<class T, class E = void>
struct addressof_member_operator {
    static constexpr bool value = false;
};

template<class T>
struct addressof_member_operator<T, typename
    addressof_void<decltype(addressof_declval<T&>().operator&())>::type> {
    static constexpr bool value = true;
};

#if AUTOBOOST_WORKAROUND(AUTOBOOST_INTEL, < 1600)
struct addressof_addressable { };

addressof_addressable*
operator&(addressof_addressable&) AUTOBOOST_NOEXCEPT;
#endif

template<class T, class E = void>
struct addressof_non_member_operator {
    static constexpr bool value = false;
};

template<class T>
struct addressof_non_member_operator<T, typename
    addressof_void<decltype(operator&(addressof_declval<T&>()))>::type> {
    static constexpr bool value = true;
};

template<class T, class E = void>
struct addressof_expression {
    static constexpr bool value = false;
};

template<class T>
struct addressof_expression<T,
    typename addressof_void<decltype(&addressof_declval<T&>())>::type> {
    static constexpr bool value = true;
};

template<class T>
struct addressof_is_constexpr {
    static constexpr bool value = addressof_expression<T>::value &&
        !addressof_member_operator<T>::value &&
        !addressof_non_member_operator<T>::value;
};

template<bool E, class T>
struct addressof_if { };

template<class T>
struct addressof_if<true, T> {
    typedef T* type;
};

template<class T>
AUTOBOOST_FORCEINLINE
typename addressof_if<!addressof_is_constexpr<T>::value, T>::type
addressof(T& o) AUTOBOOST_NOEXCEPT
{
    return address_of<T>::get(addressof_ref<T>(o), 0);
}

template<class T>
constexpr AUTOBOOST_FORCEINLINE
typename addressof_if<addressof_is_constexpr<T>::value, T>::type
addressof(T& o) AUTOBOOST_NOEXCEPT
{
    return &o;
}

} /* detail */

template<class T>
constexpr AUTOBOOST_FORCEINLINE T*
addressof(T& o) AUTOBOOST_NOEXCEPT
{
    return detail::addressof(o);
}
#endif

} /* autoboost */
#endif

#endif

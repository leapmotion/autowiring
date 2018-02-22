
// Copyright 2005-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  Based on Peter Dimov's proposal
//  http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1756.pdf
//  issue 6.18.

// This implements the extensions to the standard.
// It's undocumented, so you shouldn't use it....

#if !defined(AUTOBOOST_FUNCTIONAL_HASH_EXTENSIONS_HPP)
#define AUTOBOOST_FUNCTIONAL_HASH_EXTENSIONS_HPP

#include <autoboost/config.hpp>
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

#include <autoboost/functional/hash/hash.hpp>
#include <autoboost/detail/container_fwd.hpp>
#include <autoboost/utility/enable_if.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/preprocessor/repetition/repeat_from_to.hpp>
#include <autoboost/preprocessor/repetition/enum_params.hpp>

#if !defined(AUTOBOOST_NO_CXX11_HDR_ARRAY)
#   include <array>
#endif

#if !defined(AUTOBOOST_NO_CXX11_HDR_TUPLE)
#   include <tuple>
#endif

#if !defined(AUTOBOOST_NO_CXX11_HDR_MEMORY)
#   include <memory>
#endif

#if defined(AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING)
#include <autoboost/type_traits/is_array.hpp>
#endif

namespace autoboost
{
    template <class A, class B>
    std::size_t hash_value(std::pair<A, B> const&);
    template <class T, class A>
    std::size_t hash_value(std::vector<T, A> const&);
    template <class T, class A>
    std::size_t hash_value(std::list<T, A> const& v);
    template <class T, class A>
    std::size_t hash_value(std::deque<T, A> const& v);
    template <class K, class C, class A>
    std::size_t hash_value(std::set<K, C, A> const& v);
    template <class K, class C, class A>
    std::size_t hash_value(std::multiset<K, C, A> const& v);
    template <class K, class T, class C, class A>
    std::size_t hash_value(std::map<K, T, C, A> const& v);
    template <class K, class T, class C, class A>
    std::size_t hash_value(std::multimap<K, T, C, A> const& v);

    template <class T>
    std::size_t hash_value(std::complex<T> const&);

    template <class A, class B>
    std::size_t hash_value(std::pair<A, B> const& v)
    {
        std::size_t seed = 0;
        autoboost::hash_combine(seed, v.first);
        autoboost::hash_combine(seed, v.second);
        return seed;
    }

    template <class T, class A>
    std::size_t hash_value(std::vector<T, A> const& v)
    {
        return autoboost::hash_range(v.begin(), v.end());
    }

    template <class T, class A>
    std::size_t hash_value(std::list<T, A> const& v)
    {
        return autoboost::hash_range(v.begin(), v.end());
    }

    template <class T, class A>
    std::size_t hash_value(std::deque<T, A> const& v)
    {
        return autoboost::hash_range(v.begin(), v.end());
    }

    template <class K, class C, class A>
    std::size_t hash_value(std::set<K, C, A> const& v)
    {
        return autoboost::hash_range(v.begin(), v.end());
    }

    template <class K, class C, class A>
    std::size_t hash_value(std::multiset<K, C, A> const& v)
    {
        return autoboost::hash_range(v.begin(), v.end());
    }

    template <class K, class T, class C, class A>
    std::size_t hash_value(std::map<K, T, C, A> const& v)
    {
        return autoboost::hash_range(v.begin(), v.end());
    }

    template <class K, class T, class C, class A>
    std::size_t hash_value(std::multimap<K, T, C, A> const& v)
    {
        return autoboost::hash_range(v.begin(), v.end());
    }

    template <class T>
    std::size_t hash_value(std::complex<T> const& v)
    {
        autoboost::hash<T> hasher;
        std::size_t seed = hasher(v.imag());
        seed ^= hasher(v.real()) + (seed<<6) + (seed>>2);
        return seed;
    }

#if !defined(AUTOBOOST_NO_CXX11_HDR_ARRAY)
    template <class T, std::size_t N>
    std::size_t hash_value(std::array<T, N> const& v)
    {
        return autoboost::hash_range(v.begin(), v.end());
    }
#endif

#if !defined(AUTOBOOST_NO_CXX11_HDR_TUPLE)
    namespace hash_detail {
        template <std::size_t I, typename T>
        inline typename autoboost::enable_if_c<(I == std::tuple_size<T>::value),
                void>::type
            hash_combine_tuple(std::size_t&, T const&)
        {
        }

        template <std::size_t I, typename T>
        inline typename autoboost::enable_if_c<(I < std::tuple_size<T>::value),
                void>::type
            hash_combine_tuple(std::size_t& seed, T const& v)
        {
            autoboost::hash_combine(seed, std::get<I>(v));
            autoboost::hash_detail::hash_combine_tuple<I + 1>(seed, v);
        }

        template <typename T>
        inline std::size_t hash_tuple(T const& v)
        {
            std::size_t seed = 0;
            autoboost::hash_detail::hash_combine_tuple<0>(seed, v);
            return seed;
        }
    }

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template <typename... T>
    inline std::size_t hash_value(std::tuple<T...> const& v)
    {
        return autoboost::hash_detail::hash_tuple(v);
    }
#else

    inline std::size_t hash_value(std::tuple<> const& v)
    {
        return autoboost::hash_detail::hash_tuple(v);
    }

#   define AUTOBOOST_HASH_TUPLE_F(z, n, _)                                      \
    template<                                                               \
        AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, typename A)                            \
    >                                                                       \
    inline std::size_t hash_value(std::tuple<                               \
        AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, A)                                     \
    > const& v)                                                             \
    {                                                                       \
        return autoboost::hash_detail::hash_tuple(v);                           \
    }

    AUTOBOOST_PP_REPEAT_FROM_TO(1, 11, AUTOBOOST_HASH_TUPLE_F, _)
#   undef AUTOBOOST_HASH_TUPLE_F
#endif

#endif

#if !defined(AUTOBOOST_NO_CXX11_SMART_PTR)
    template <typename T>
    inline std::size_t hash_value(std::shared_ptr<T> const& x) {
        return autoboost::hash_value(x.get());
    }

    template <typename T, typename Deleter>
    inline std::size_t hash_value(std::unique_ptr<T, Deleter> const& x) {
        return autoboost::hash_value(x.get());
    }
#endif

    //
    // call_hash_impl
    //

    // On compilers without function template ordering, this deals with arrays.

#if defined(AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING)
    namespace hash_detail
    {
        template <bool IsArray>
        struct call_hash_impl
        {
            template <class T>
            struct inner
            {
                static std::size_t call(T const& v)
                {
                    using namespace autoboost;
                    return hash_value(v);
                }
            };
        };

        template <>
        struct call_hash_impl<true>
        {
            template <class Array>
            struct inner
            {
                static std::size_t call(Array const& v)
                {
                    const int size = sizeof(v) / sizeof(*v);
                    return autoboost::hash_range(v, v + size);
                }
            };
        };

        template <class T>
        struct call_hash
            : public call_hash_impl<autoboost::is_array<T>::value>
                ::AUTOBOOST_NESTED_TEMPLATE inner<T>
        {
        };
    }
#endif // AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING

    //
    // autoboost::hash
    //


#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

    template <class T> struct hash
        : autoboost::hash_detail::hash_base<T>
    {
#if !defined(AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING)
        std::size_t operator()(T const& val) const
        {
            return hash_value(val);
        }
#else
        std::size_t operator()(T const& val) const
        {
            return hash_detail::call_hash<T>::call(val);
        }
#endif
    };

#if AUTOBOOST_WORKAROUND(__DMC__, <= 0x848)
    template <class T, unsigned int n> struct hash<T[n]>
        : autoboost::hash_detail::hash_base<T[n]>
    {
        std::size_t operator()(const T* val) const
        {
            return autoboost::hash_range(val, val+n);
        }
    };
#endif

#else // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

    // On compilers without partial specialization, autoboost::hash<T>
    // has already been declared to deal with pointers, so just
    // need to supply the non-pointer version of hash_impl.

    namespace hash_detail
    {
        template <bool IsPointer>
        struct hash_impl;

        template <>
        struct hash_impl<false>
        {
            template <class T>
            struct inner
                : autoboost::hash_detail::hash_base<T>
            {
#if !defined(AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING)
                std::size_t operator()(T const& val) const
                {
                    return hash_value(val);
                }
#else
                std::size_t operator()(T const& val) const
                {
                    return hash_detail::call_hash<T>::call(val);
                }
#endif
            };
        };
    }
#endif  // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
}

#endif


// Copyright 2005-2011 Daniel James.
// Copyright 2009 Pablo Halpern.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/unordered for documentation

#ifndef AUTOBOOST_UNORDERED_ALLOCATE_HPP
#define AUTOBOOST_UNORDERED_ALLOCATE_HPP

#include <autoboost/config.hpp>
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

#include <autoboost/unordered/detail/fwd.hpp>
#include <autoboost/move/move.hpp>
#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/inc.hpp>
#include <autoboost/preprocessor/dec.hpp>
#include <autoboost/preprocessor/repetition/enum.hpp>
#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/preprocessor/repetition/enum_binary_params.hpp>
#include <autoboost/preprocessor/repetition/repeat_from_to.hpp>
#include <autoboost/type_traits/is_class.hpp>
#include <autoboost/type_traits/add_lvalue_reference.hpp>
#include <autoboost/tuple/tuple.hpp>
#include <autoboost/utility/enable_if.hpp>
#include <autoboost/utility/addressof.hpp>
#include <autoboost/detail/select_type.hpp>
#include <autoboost/assert.hpp>
#include <utility>

#if !defined(AUTOBOOST_NO_CXX11_HDR_TUPLE)
#include <tuple>
#endif

#if defined(AUTOBOOST_MSVC)
#pragma warning(push)
#pragma warning(disable:4512) // assignment operator could not be generated.
#pragma warning(disable:4345) // behavior change: an object of POD type
                              // constructed with an initializer of the form ()
                              // will be default-initialized.
#endif

#define AUTOBOOST_UNORDERED_EMPLACE_LIMIT 10

namespace autoboost { namespace unordered { namespace detail {

    ////////////////////////////////////////////////////////////////////////////
    // Bits and pieces for implementing traits

    template <typename T> typename autoboost::add_lvalue_reference<T>::type make();
    struct choice9 { typedef char (&type)[9]; };
    struct choice8 : choice9 { typedef char (&type)[8]; };
    struct choice7 : choice8 { typedef char (&type)[7]; };
    struct choice6 : choice7 { typedef char (&type)[6]; };
    struct choice5 : choice6 { typedef char (&type)[5]; };
    struct choice4 : choice5 { typedef char (&type)[4]; };
    struct choice3 : choice4 { typedef char (&type)[3]; };
    struct choice2 : choice3 { typedef char (&type)[2]; };
    struct choice1 : choice2 { typedef char (&type)[1]; };
    choice1 choose();

    typedef choice1::type yes_type;
    typedef choice2::type no_type;

    struct private_type
    {
       private_type const &operator,(int) const;
    };

    template <typename T>
    no_type is_private_type(T const&);
    yes_type is_private_type(private_type const&);

    struct convert_from_anything {
        template <typename T>
        convert_from_anything(T const&);
    };

    ////////////////////////////////////////////////////////////////////////////
    // emplace_args
    //
    // Either forwarding variadic arguments, or storing the arguments in
    // emplace_args##n

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

#define AUTOBOOST_UNORDERED_EMPLACE_TEMPLATE typename... Args
#define AUTOBOOST_UNORDERED_EMPLACE_ARGS AUTOBOOST_FWD_REF(Args)... args
#define AUTOBOOST_UNORDERED_EMPLACE_FORWARD autoboost::forward<Args>(args)...

#define AUTOBOOST_UNORDERED_EMPLACE_ARGS1(a0) a0
#define AUTOBOOST_UNORDERED_EMPLACE_ARGS2(a0, a1) a0, a1
#define AUTOBOOST_UNORDERED_EMPLACE_ARGS3(a0, a1, a2) a0, a1, a2

#else

#define AUTOBOOST_UNORDERED_EMPLACE_TEMPLATE typename Args
#define AUTOBOOST_UNORDERED_EMPLACE_ARGS Args const& args
#define AUTOBOOST_UNORDERED_EMPLACE_FORWARD args

#define AUTOBOOST_UNORDERED_FWD_PARAM(z, n, a) \
    AUTOBOOST_FWD_REF(AUTOBOOST_PP_CAT(A, n)) AUTOBOOST_PP_CAT(a, n)

#define AUTOBOOST_UNORDERED_CALL_FORWARD(z, i, a) \
    autoboost::forward<AUTOBOOST_PP_CAT(A,i)>(AUTOBOOST_PP_CAT(a,i))

#define AUTOBOOST_UNORDERED_EARGS(z, n, _)                                      \
    template <AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                     \
    struct AUTOBOOST_PP_CAT(emplace_args, n)                                    \
    {                                                                       \
        AUTOBOOST_PP_REPEAT_##z(n, AUTOBOOST_UNORDERED_EARGS_MEMBER, _)             \
        AUTOBOOST_PP_CAT(emplace_args, n) (                                     \
            AUTOBOOST_PP_ENUM_BINARY_PARAMS_Z(z, n, Arg, b)                     \
        ) : AUTOBOOST_PP_ENUM_##z(n, AUTOBOOST_UNORDERED_EARGS_INIT, _)             \
        {}                                                                  \
                                                                            \
    };                                                                      \
                                                                            \
    template <AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                     \
    inline AUTOBOOST_PP_CAT(emplace_args, n) <                                  \
        AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, A)                                     \
    > create_emplace_args(                                                  \
        AUTOBOOST_PP_ENUM_##z(n, AUTOBOOST_UNORDERED_FWD_PARAM, b)                  \
    )                                                                       \
    {                                                                       \
        AUTOBOOST_PP_CAT(emplace_args, n) <                                     \
            AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, A)                                 \
        > e(AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, b));                               \
        return e;                                                           \
    }

#define AUTOBOOST_UNORDERED_EMPLACE_ARGS1 create_emplace_args
#define AUTOBOOST_UNORDERED_EMPLACE_ARGS2 create_emplace_args
#define AUTOBOOST_UNORDERED_EMPLACE_ARGS3 create_emplace_args

#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)

#define AUTOBOOST_UNORDERED_EARGS_MEMBER(z, n, _)                               \
    typedef AUTOBOOST_FWD_REF(AUTOBOOST_PP_CAT(A, n)) AUTOBOOST_PP_CAT(Arg, n);         \
    AUTOBOOST_PP_CAT(Arg, n) AUTOBOOST_PP_CAT(a, n);

#define AUTOBOOST_UNORDERED_EARGS_INIT(z, n, _)                                 \
    AUTOBOOST_PP_CAT(a, n)(                                                     \
        autoboost::forward<AUTOBOOST_PP_CAT(A,n)>(AUTOBOOST_PP_CAT(b, n)))

#else

#define AUTOBOOST_UNORDERED_EARGS_MEMBER(z, n, _)                               \
    typedef typename autoboost::add_lvalue_reference<AUTOBOOST_PP_CAT(A, n)>::type  \
        AUTOBOOST_PP_CAT(Arg, n);                                               \
    AUTOBOOST_PP_CAT(Arg, n) AUTOBOOST_PP_CAT(a, n);

#define AUTOBOOST_UNORDERED_EARGS_INIT(z, n, _)                                 \
    AUTOBOOST_PP_CAT(a, n)(AUTOBOOST_PP_CAT(b, n))

#endif

AUTOBOOST_PP_REPEAT_FROM_TO(1, AUTOBOOST_UNORDERED_EMPLACE_LIMIT, AUTOBOOST_UNORDERED_EARGS,
    _)

#undef AUTOBOOST_UNORDERED_DEFINE_EMPLACE_ARGS
#undef AUTOBOOST_UNORDERED_EARGS_MEMBER
#undef AUTOBOOST_UNORDERED_EARGS_INIT

#endif

}}}

////////////////////////////////////////////////////////////////////////////////
//
// Pick which version of allocator_traits to use
//
// 0 = Own partial implementation
// 1 = std::allocator_traits
// 2 = autoboost::container::allocator_traits

#if !defined(AUTOBOOST_UNORDERED_USE_ALLOCATOR_TRAITS)
#   if defined(__GXX_EXPERIMENTAL_CXX0X__) && \
            (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))
#       define AUTOBOOST_UNORDERED_USE_ALLOCATOR_TRAITS 0
#   elif defined(AUTOBOOST_MSVC)
#       if AUTOBOOST_MSVC < 1400
            // Use container's allocator_traits for older versions of Visual
            // C++ as I don't test with them.
#           define AUTOBOOST_UNORDERED_USE_ALLOCATOR_TRAITS 2
#       endif
#   endif
#endif

#if !defined(AUTOBOOST_UNORDERED_USE_ALLOCATOR_TRAITS)
#   define AUTOBOOST_UNORDERED_USE_ALLOCATOR_TRAITS 0
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Some utilities for implementing allocator_traits, but useful elsewhere so
// they're always defined.

#if !defined(AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS)
#  include <type_traits>
#endif

namespace autoboost { namespace unordered { namespace detail {

    ////////////////////////////////////////////////////////////////////////////
    // Integral_constrant, true_type, false_type
    //
    // Uses the standard versions if available.

#if !defined(AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS)

    using std::integral_constant;
    using std::true_type;
    using std::false_type;

#else

    template <typename T, T Value>
    struct integral_constant { enum { value = Value }; };

    typedef autoboost::unordered::detail::integral_constant<bool, true> true_type;
    typedef autoboost::unordered::detail::integral_constant<bool, false> false_type;

#endif

    ////////////////////////////////////////////////////////////////////////////
    // Explicitly call a destructor

#if defined(AUTOBOOST_MSVC)
#pragma warning(push)
#pragma warning(disable:4100) // unreferenced formal parameter
#endif

    namespace func {
        template <class T>
        inline void destroy(T* x) {
            x->~T();
        }
    }

#if defined(AUTOBOOST_MSVC)
#pragma warning(pop)
#endif

    ////////////////////////////////////////////////////////////////////////////
    // Expression test mechanism
    //
    // When SFINAE expressions are available, define
    // AUTOBOOST_UNORDERED_HAS_FUNCTION which can check if a function call is
    // supported by a class, otherwise define AUTOBOOST_UNORDERED_HAS_MEMBER which
    // can detect if a class has the specified member, but not that it has the
    // correct type, this is good enough for a passable impression of
    // allocator_traits.

#if !defined(AUTOBOOST_NO_SFINAE_EXPR)

    template <typename T, unsigned int> struct expr_test;
    template <typename T> struct expr_test<T, sizeof(char)> : T {};

#   define AUTOBOOST_UNORDERED_CHECK_EXPRESSION(count, result, expression)      \
        template <typename U>                                               \
        static typename autoboost::unordered::detail::expr_test<                \
            AUTOBOOST_PP_CAT(choice, result),                                   \
            sizeof(for_expr_test((                                          \
                (expression),                                               \
            0)))>::type test(                                               \
            AUTOBOOST_PP_CAT(choice, count))

#   define AUTOBOOST_UNORDERED_DEFAULT_EXPRESSION(count, result)                \
        template <typename U>                                               \
        static AUTOBOOST_PP_CAT(choice, result)::type test(                     \
            AUTOBOOST_PP_CAT(choice, count))

#   define AUTOBOOST_UNORDERED_HAS_FUNCTION(name, thing, args, _)               \
    struct AUTOBOOST_PP_CAT(has_, name)                                         \
    {                                                                       \
        template <typename U> static char for_expr_test(U const&);          \
        AUTOBOOST_UNORDERED_CHECK_EXPRESSION(1, 1,                              \
            autoboost::unordered::detail::make< thing >().name args);           \
        AUTOBOOST_UNORDERED_DEFAULT_EXPRESSION(2, 2);                           \
                                                                            \
        enum { value = sizeof(test<T>(choose())) == sizeof(choice1::type) };\
    }

#else

    template <typename T> struct identity { typedef T type; };

#   define AUTOBOOST_UNORDERED_CHECK_MEMBER(count, result, name, member)        \
                                                                            \
    typedef typename autoboost::unordered::detail::identity<member>::type       \
        AUTOBOOST_PP_CAT(check, count);                                         \
                                                                            \
    template <AUTOBOOST_PP_CAT(check, count) e>                                 \
    struct AUTOBOOST_PP_CAT(test, count) {                                      \
        typedef AUTOBOOST_PP_CAT(choice, result) type;                          \
    };                                                                      \
                                                                            \
    template <class U> static typename                                      \
        AUTOBOOST_PP_CAT(test, count)<&U::name>::type                           \
        test(AUTOBOOST_PP_CAT(choice, count))

#   define AUTOBOOST_UNORDERED_DEFAULT_MEMBER(count, result)                    \
    template <class U> static AUTOBOOST_PP_CAT(choice, result)::type            \
        test(AUTOBOOST_PP_CAT(choice, count))

#   define AUTOBOOST_UNORDERED_HAS_MEMBER(name)                                 \
    struct AUTOBOOST_PP_CAT(has_, name)                                         \
    {                                                                       \
        struct impl {                                                       \
            struct base_mixin { int name; };                                \
            struct base : public T, public base_mixin {};                   \
                                                                            \
            AUTOBOOST_UNORDERED_CHECK_MEMBER(1, 1, name, int base_mixin::*);    \
            AUTOBOOST_UNORDERED_DEFAULT_MEMBER(2, 2);                           \
                                                                            \
            enum { value = sizeof(choice2::type) ==                         \
                sizeof(test<base>(choose()))                                \
            };                                                              \
        };                                                                  \
                                                                            \
        enum { value = impl::value };                                       \
    }

#endif

}}}

////////////////////////////////////////////////////////////////////////////////
//
// Allocator traits
//
// First our implementation, then later light wrappers around the alternatives

#if AUTOBOOST_UNORDERED_USE_ALLOCATOR_TRAITS == 0

#   include <autoboost/limits.hpp>
#   include <autoboost/utility/enable_if.hpp>
#   include <autoboost/pointer_to_other.hpp>
#   if defined(AUTOBOOST_NO_SFINAE_EXPR)
#       include <autoboost/type_traits/is_same.hpp>
#   endif

#   if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) && \
        !defined(AUTOBOOST_NO_SFINAE_EXPR)
#       define AUTOBOOST_UNORDERED_DETAIL_FULL_CONSTRUCT 1
#   else
#       define AUTOBOOST_UNORDERED_DETAIL_FULL_CONSTRUCT 0
#   endif

namespace autoboost { namespace unordered { namespace detail {

    // TODO: Does this match std::allocator_traits<Alloc>::rebind_alloc<T>?
    template <typename Alloc, typename T>
    struct rebind_wrap
    {
        typedef typename Alloc::AUTOBOOST_NESTED_TEMPLATE rebind<T>::other type;
    };

#   if defined(AUTOBOOST_MSVC) && AUTOBOOST_MSVC <= 1400

#       define AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT(tname)                    \
    template <typename Tp, typename Default>                                \
    struct default_type_ ## tname {                                         \
                                                                            \
        template <typename X>                                               \
        static choice1::type test(choice1, typename X::tname* = 0);         \
                                                                            \
        template <typename X>                                               \
        static choice2::type test(choice2, void* = 0);                      \
                                                                            \
        struct DefaultWrap { typedef Default tname; };                      \
                                                                            \
        enum { value = (1 == sizeof(test<Tp>(choose()))) };                 \
                                                                            \
        typedef typename autoboost::detail::if_true<value>::                    \
            AUTOBOOST_NESTED_TEMPLATE then<Tp, DefaultWrap>                     \
            ::type::tname type;                                             \
    }

#   else

    template <typename T, typename T2>
    struct sfinae : T2 {};

#       define AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT(tname)                    \
    template <typename Tp, typename Default>                                \
    struct default_type_ ## tname {                                         \
                                                                            \
        template <typename X>                                               \
        static typename autoboost::unordered::detail::sfinae<                   \
                typename X::tname, choice1>::type                           \
            test(choice1);                                                  \
                                                                            \
        template <typename X>                                               \
        static choice2::type test(choice2);                                 \
                                                                            \
        struct DefaultWrap { typedef Default tname; };                      \
                                                                            \
        enum { value = (1 == sizeof(test<Tp>(choose()))) };                 \
                                                                            \
        typedef typename autoboost::detail::if_true<value>::                    \
            AUTOBOOST_NESTED_TEMPLATE then<Tp, DefaultWrap>                     \
            ::type::tname type;                                             \
    }

#   endif

#   define AUTOBOOST_UNORDERED_DEFAULT_TYPE(T,tname, arg)                   \
    typename default_type_ ## tname<T, arg>::type

    AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT(pointer);
    AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT(const_pointer);
    AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT(void_pointer);
    AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT(const_void_pointer);
    AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT(difference_type);
    AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT(size_type);
    AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT(propagate_on_container_copy_assignment);
    AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT(propagate_on_container_move_assignment);
    AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT(propagate_on_container_swap);

#   if !defined(AUTOBOOST_NO_SFINAE_EXPR)

    template <typename T>
    AUTOBOOST_UNORDERED_HAS_FUNCTION(
        select_on_container_copy_construction, U const, (), 0
    );

    template <typename T>
    AUTOBOOST_UNORDERED_HAS_FUNCTION(
        max_size, U const, (), 0
    );

#       if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

    template <typename T, typename ValueType, typename... Args>
    AUTOBOOST_UNORDERED_HAS_FUNCTION(
    construct, U, (
        autoboost::unordered::detail::make<ValueType*>(),
        autoboost::unordered::detail::make<Args const>()...), 2
    );

#       else

    template <typename T, typename ValueType>
    AUTOBOOST_UNORDERED_HAS_FUNCTION(
    construct, U, (
        autoboost::unordered::detail::make<ValueType*>(),
        autoboost::unordered::detail::make<ValueType const>()), 2
    );

#       endif

    template <typename T, typename ValueType>
    AUTOBOOST_UNORDERED_HAS_FUNCTION(
        destroy, U, (autoboost::unordered::detail::make<ValueType*>()), 1
    );

#   else

    template <typename T>
    AUTOBOOST_UNORDERED_HAS_MEMBER(select_on_container_copy_construction);

    template <typename T>
    AUTOBOOST_UNORDERED_HAS_MEMBER(max_size);

    template <typename T, typename ValueType>
    AUTOBOOST_UNORDERED_HAS_MEMBER(construct);

    template <typename T, typename ValueType>
    AUTOBOOST_UNORDERED_HAS_MEMBER(destroy);

#   endif

    namespace func
    {

    template <typename Alloc>
    inline Alloc call_select_on_container_copy_construction(const Alloc& rhs,
        typename autoboost::enable_if_c<
            autoboost::unordered::detail::
            has_select_on_container_copy_construction<Alloc>::value, void*
        >::type = 0)
    {
        return rhs.select_on_container_copy_construction();
    }

    template <typename Alloc>
    inline Alloc call_select_on_container_copy_construction(const Alloc& rhs,
        typename autoboost::disable_if_c<
            autoboost::unordered::detail::
            has_select_on_container_copy_construction<Alloc>::value, void*
        >::type = 0)
    {
        return rhs;
    }

    template <typename SizeType, typename Alloc>
    inline SizeType call_max_size(const Alloc& a,
        typename autoboost::enable_if_c<
            autoboost::unordered::detail::has_max_size<Alloc>::value, void*
        >::type = 0)
    {
        return a.max_size();
    }

    template <typename SizeType, typename Alloc>
    inline SizeType call_max_size(const Alloc&, typename autoboost::disable_if_c<
            autoboost::unordered::detail::has_max_size<Alloc>::value, void*
        >::type = 0)
    {
        return (std::numeric_limits<SizeType>::max)();
    }

    } // namespace func.

    template <typename Alloc>
    struct allocator_traits
    {
        typedef Alloc allocator_type;
        typedef typename Alloc::value_type value_type;

        typedef AUTOBOOST_UNORDERED_DEFAULT_TYPE(Alloc, pointer, value_type*)
            pointer;

        template <typename T>
        struct pointer_to_other : autoboost::pointer_to_other<pointer, T> {};

        typedef AUTOBOOST_UNORDERED_DEFAULT_TYPE(Alloc, const_pointer,
            typename pointer_to_other<const value_type>::type)
            const_pointer;

        //typedef AUTOBOOST_UNORDERED_DEFAULT_TYPE(Alloc, void_pointer,
        //    typename pointer_to_other<void>::type)
        //    void_pointer;
        //
        //typedef AUTOBOOST_UNORDERED_DEFAULT_TYPE(Alloc, const_void_pointer,
        //    typename pointer_to_other<const void>::type)
        //    const_void_pointer;

        typedef AUTOBOOST_UNORDERED_DEFAULT_TYPE(Alloc, difference_type,
            std::ptrdiff_t) difference_type;

        typedef AUTOBOOST_UNORDERED_DEFAULT_TYPE(Alloc, size_type, std::size_t)
            size_type;

        // TODO: rebind_alloc and rebind_traits

        static pointer allocate(Alloc& a, size_type n)
            { return a.allocate(n); }

        // I never use this, so I'll just comment it out for now.
        //
        //static pointer allocate(Alloc& a, size_type n,
        //        const_void_pointer hint)
        //    { return DEFAULT_FUNC(allocate, pointer)(a, n, hint); }

        static void deallocate(Alloc& a, pointer p, size_type n)
            { a.deallocate(p, n); }

    public:

#   if AUTOBOOST_UNORDERED_DETAIL_FULL_CONSTRUCT

        template <typename T, typename... Args>
        static typename autoboost::enable_if_c<
                autoboost::unordered::detail::has_construct<Alloc, T, Args...>
                ::value>::type
            construct(Alloc& a, T* p, AUTOBOOST_FWD_REF(Args)... x)
        {
            a.construct(p, autoboost::forward<Args>(x)...);
        }

        template <typename T, typename... Args>
        static typename autoboost::disable_if_c<
                autoboost::unordered::detail::has_construct<Alloc, T, Args...>
                ::value>::type
            construct(Alloc&, T* p, AUTOBOOST_FWD_REF(Args)... x)
        {
            new ((void*) p) T(autoboost::forward<Args>(x)...);
        }

        template <typename T>
        static typename autoboost::enable_if_c<
                autoboost::unordered::detail::has_destroy<Alloc, T>::value>::type
            destroy(Alloc& a, T* p)
        {
            a.destroy(p);
        }

        template <typename T>
        static typename autoboost::disable_if_c<
                autoboost::unordered::detail::has_destroy<Alloc, T>::value>::type
            destroy(Alloc&, T* p)
        {
            autoboost::unordered::detail::func::destroy(p);
        }

#   elif !defined(AUTOBOOST_NO_SFINAE_EXPR)

        template <typename T>
        static typename autoboost::enable_if_c<
                autoboost::unordered::detail::has_construct<Alloc, T>::value>::type
            construct(Alloc& a, T* p, T const& x)
        {
            a.construct(p, x);
        }

        template <typename T>
        static typename autoboost::disable_if_c<
                autoboost::unordered::detail::has_construct<Alloc, T>::value>::type
            construct(Alloc&, T* p, T const& x)
        {
            new ((void*) p) T(x);
        }

        template <typename T>
        static typename autoboost::enable_if_c<
                autoboost::unordered::detail::has_destroy<Alloc, T>::value>::type
            destroy(Alloc& a, T* p)
        {
            a.destroy(p);
        }

        template <typename T>
        static typename autoboost::disable_if_c<
                autoboost::unordered::detail::has_destroy<Alloc, T>::value>::type
            destroy(Alloc&, T* p)
        {
            autoboost::unordered::detail::func::destroy(p);
        }

#   else

        // If we don't have SFINAE expressions, only call construct for the
        // copy constructor for the allocator's value_type - as that's
        // the only construct method that old fashioned allocators support.

        template <typename T>
        static void construct(Alloc& a, T* p, T const& x,
            typename autoboost::enable_if_c<
                    autoboost::unordered::detail::has_construct<Alloc, T>::value &&
                    autoboost::is_same<T, value_type>::value,
                    void*>::type = 0)
        {
            a.construct(p, x);
        }

        template <typename T>
        static void construct(Alloc&, T* p, T const& x,
            typename autoboost::disable_if_c<
                autoboost::unordered::detail::has_construct<Alloc, T>::value &&
                autoboost::is_same<T, value_type>::value,
                void*>::type = 0)
        {
            new ((void*) p) T(x);
        }

        template <typename T>
        static void destroy(Alloc& a, T* p,
            typename autoboost::enable_if_c<
                autoboost::unordered::detail::has_destroy<Alloc, T>::value &&
                autoboost::is_same<T, value_type>::value,
                void*>::type = 0)
        {
            a.destroy(p);
        }

        template <typename T>
        static void destroy(Alloc&, T* p,
            typename autoboost::disable_if_c<
                autoboost::unordered::detail::has_destroy<Alloc, T>::value &&
                autoboost::is_same<T, value_type>::value,
                void*>::type = 0)
        {
            autoboost::unordered::detail::func::destroy(p);
        }

#   endif

        static size_type max_size(const Alloc& a)
        {
            return autoboost::unordered::detail::func::
                call_max_size<size_type>(a);
        }

        // Allocator propagation on construction

        static Alloc select_on_container_copy_construction(Alloc const& rhs)
        {
            return autoboost::unordered::detail::func::
                call_select_on_container_copy_construction(rhs);
        }

        // Allocator propagation on assignment and swap.
        // Return true if lhs is modified.
        typedef AUTOBOOST_UNORDERED_DEFAULT_TYPE(
            Alloc, propagate_on_container_copy_assignment, false_type)
            propagate_on_container_copy_assignment;
        typedef AUTOBOOST_UNORDERED_DEFAULT_TYPE(
            Alloc,propagate_on_container_move_assignment, false_type)
            propagate_on_container_move_assignment;
        typedef AUTOBOOST_UNORDERED_DEFAULT_TYPE(
            Alloc,propagate_on_container_swap,false_type)
            propagate_on_container_swap;
    };
}}}

#   undef AUTOBOOST_UNORDERED_DEFAULT_TYPE_TMPLT
#   undef AUTOBOOST_UNORDERED_DEFAULT_TYPE

////////////////////////////////////////////////////////////////////////////////
//
// std::allocator_traits

#elif AUTOBOOST_UNORDERED_USE_ALLOCATOR_TRAITS == 1

#   include <memory>

#   define AUTOBOOST_UNORDERED_DETAIL_FULL_CONSTRUCT 1

namespace autoboost { namespace unordered { namespace detail {

    template <typename Alloc>
    struct allocator_traits : std::allocator_traits<Alloc> {};

    template <typename Alloc, typename T>
    struct rebind_wrap
    {
        typedef typename std::allocator_traits<Alloc>::
            template rebind_alloc<T> type;
    };
}}}

////////////////////////////////////////////////////////////////////////////////
//
// autoboost::container::allocator_traits

#elif AUTOBOOST_UNORDERED_USE_ALLOCATOR_TRAITS == 2

#   include <autoboost/container/allocator_traits.hpp>

#   define AUTOBOOST_UNORDERED_DETAIL_FULL_CONSTRUCT 0

namespace autoboost { namespace unordered { namespace detail {

    template <typename Alloc>
    struct allocator_traits :
        autoboost::container::allocator_traits<Alloc> {};

    template <typename Alloc, typename T>
    struct rebind_wrap :
        autoboost::container::allocator_traits<Alloc>::
            template portable_rebind_alloc<T>
    {};

}}}

#else

#error "Invalid AUTOBOOST_UNORDERED_USE_ALLOCATOR_TRAITS value."

#endif


namespace autoboost { namespace unordered { namespace detail { namespace func {

    ////////////////////////////////////////////////////////////////////////////
    // call_construct

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

#   if AUTOBOOST_UNORDERED_DETAIL_FULL_CONSTRUCT

    template <typename Alloc, typename T, typename... Args>
    inline void call_construct(Alloc& alloc, T* address,
        AUTOBOOST_FWD_REF(Args)... args)
    {
        autoboost::unordered::detail::allocator_traits<Alloc>::construct(alloc,
            address, autoboost::forward<Args>(args)...);
    }

    template <typename Alloc, typename T>
    inline void destroy_value_impl(Alloc& alloc, T* x) {
        autoboost::unordered::detail::allocator_traits<Alloc>::destroy(alloc, x);
    }


#   else

    template <typename Alloc, typename T, typename... Args>
    inline void call_construct(Alloc&, T* address,
        AUTOBOOST_FWD_REF(Args)... args)
    {
        new((void*) address) T(autoboost::forward<Args>(args)...);
    }

    template <typename Alloc, typename T>
    inline void destroy_value_impl(Alloc&, T* x) {
        autoboost::unordered::detail::func::destroy(x);
    }


#   endif

#else

    template <typename Alloc, typename T>
    inline void destroy_value_impl(Alloc&, T* x) {
        autoboost::unordered::detail::func::destroy(x);
    }

#endif

    ////////////////////////////////////////////////////////////////////////////
    // Construct from tuple
    //
    // Used for piecewise construction.

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

#   define AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE(n, namespace_)              \
    template<typename Alloc, typename T>                                    \
    void construct_from_tuple(Alloc& alloc, T* ptr, namespace_ tuple<>)     \
    {                                                                       \
        autoboost::unordered::detail::func::call_construct(alloc, ptr);         \
    }                                                                       \
                                                                            \
    AUTOBOOST_PP_REPEAT_FROM_TO(1, n,                                           \
        AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE_IMPL, namespace_)

#   define AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE_IMPL(z, n, namespace_)      \
    template<typename Alloc, typename T,                                    \
        AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                           \
    void construct_from_tuple(Alloc& alloc, T* ptr,                         \
            namespace_ tuple<AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, A)> const& x)     \
    {                                                                       \
        autoboost::unordered::detail::func::call_construct(alloc, ptr,          \
            AUTOBOOST_PP_ENUM_##z(n, AUTOBOOST_UNORDERED_GET_TUPLE_ARG, namespace_) \
        );                                                                  \
    }

#   define AUTOBOOST_UNORDERED_GET_TUPLE_ARG(z, n, namespace_)                  \
    namespace_ get<n>(x)

#elif !defined(__SUNPRO_CC)

#   define AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE(n, namespace_)              \
    template<typename Alloc, typename T>                                    \
    void construct_from_tuple(Alloc&, T* ptr, namespace_ tuple<>)           \
    {                                                                       \
        new ((void*) ptr) T();                                              \
    }                                                                       \
                                                                            \
    AUTOBOOST_PP_REPEAT_FROM_TO(1, n,                                           \
        AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE_IMPL, namespace_)

#   define AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE_IMPL(z, n, namespace_)      \
    template<typename Alloc, typename T,                                    \
        AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                           \
    void construct_from_tuple(Alloc&, T* ptr,                               \
            namespace_ tuple<AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, A)> const& x)     \
    {                                                                       \
        new ((void*) ptr) T(                                                \
            AUTOBOOST_PP_ENUM_##z(n, AUTOBOOST_UNORDERED_GET_TUPLE_ARG, namespace_) \
        );                                                                  \
    }

#   define AUTOBOOST_UNORDERED_GET_TUPLE_ARG(z, n, namespace_)                  \
    namespace_ get<n>(x)

#else

    template <int N> struct length {};

#   define AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE(n, namespace_)              \
    template<typename Alloc, typename T>                                    \
    void construct_from_tuple_impl(                                         \
            autoboost::unordered::detail::func::length<0>, Alloc&, T* ptr,      \
            namespace_ tuple<>)                                             \
    {                                                                       \
        new ((void*) ptr) T();                                              \
    }                                                                       \
                                                                            \
    AUTOBOOST_PP_REPEAT_FROM_TO(1, n,                                           \
        AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE_IMPL, namespace_)

#   define AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE_IMPL(z, n, namespace_)      \
    template<typename Alloc, typename T,                                    \
        AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                           \
    void construct_from_tuple_impl(                                         \
            autoboost::unordered::detail::func::length<n>, Alloc&, T* ptr,      \
            namespace_ tuple<AUTOBOOST_PP_ENUM_PARAMS_Z(z, n, A)> const& x)     \
    {                                                                       \
        new ((void*) ptr) T(                                                \
            AUTOBOOST_PP_ENUM_##z(n, AUTOBOOST_UNORDERED_GET_TUPLE_ARG, namespace_) \
        );                                                                  \
    }

#   define AUTOBOOST_UNORDERED_GET_TUPLE_ARG(z, n, namespace_)                  \
    namespace_ get<n>(x)

#endif

AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE(10, autoboost::)

#if !defined(__SUNPRO_CC) && !defined(AUTOBOOST_NO_CXX11_HDR_TUPLE)
   AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE(10, std::)
#endif

#undef AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE
#undef AUTOBOOST_UNORDERED_CONSTRUCT_FROM_TUPLE_IMPL
#undef AUTOBOOST_UNORDERED_GET_TUPLE_ARG

#if defined(__SUNPRO_CC)

    template <typename Alloc, typename T, typename Tuple>
    void construct_from_tuple(Alloc& alloc, T* ptr, Tuple const& x)
    {
        construct_from_tuple_impl(
            autoboost::unordered::detail::func::length<
                autoboost::tuples::length<Tuple>::value>(),
            alloc, ptr, x);
    }

#endif

    ////////////////////////////////////////////////////////////////////////////
    // Trait to check for piecewise construction.

    template <typename A0>
    struct use_piecewise {
        static choice1::type test(choice1,
            autoboost::unordered::piecewise_construct_t);

        static choice2::type test(choice2, ...);

        enum { value = sizeof(choice1::type) ==
            sizeof(test(choose(), autoboost::unordered::detail::make<A0>())) };
    };

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

    ////////////////////////////////////////////////////////////////////////////
    // Construct from variadic parameters

    // For the standard pair constructor.

    template <typename Alloc, typename T, typename... Args>
    inline void construct_value_impl(Alloc& alloc, T* address,
        AUTOBOOST_FWD_REF(Args)... args)
    {
        autoboost::unordered::detail::func::call_construct(alloc,
            address, autoboost::forward<Args>(args)...);
    }

    // Special case for piece_construct
    //
    // TODO: When possible, it might be better to use std::pair's
    // constructor for std::piece_construct with std::tuple.

    template <typename Alloc, typename A, typename B,
        typename A0, typename A1, typename A2>
    inline typename enable_if<use_piecewise<A0>, void>::type
        construct_value_impl(Alloc& alloc, std::pair<A, B>* address,
            AUTOBOOST_FWD_REF(A0), AUTOBOOST_FWD_REF(A1) a1, AUTOBOOST_FWD_REF(A2) a2)
    {
        autoboost::unordered::detail::func::construct_from_tuple(alloc,
            autoboost::addressof(address->first), autoboost::forward<A1>(a1));
        autoboost::unordered::detail::func::construct_from_tuple(alloc,
            autoboost::addressof(address->second), autoboost::forward<A2>(a2));
    }

#else // AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES

////////////////////////////////////////////////////////////////////////////////
// Construct from emplace_args

    // Explicitly write out first three overloads for the sake of sane
    // error messages.

    template <typename Alloc, typename T, typename A0>
    inline void construct_value_impl(Alloc&, T* address,
            emplace_args1<A0> const& args)
    {
        new((void*) address) T(autoboost::forward<A0>(args.a0));
    }

    template <typename Alloc, typename T, typename A0, typename A1>
    inline void construct_value_impl(Alloc&, T* address,
            emplace_args2<A0, A1> const& args)
    {
        new((void*) address) T(
            autoboost::forward<A0>(args.a0),
            autoboost::forward<A1>(args.a1)
        );
    }

    template <typename Alloc, typename T, typename A0, typename A1, typename A2>
    inline void construct_value_impl(Alloc&, T* address,
            emplace_args3<A0, A1, A2> const& args)
    {
        new((void*) address) T(
            autoboost::forward<A0>(args.a0),
            autoboost::forward<A1>(args.a1),
            autoboost::forward<A2>(args.a2)
        );
    }

    // Use a macro for the rest.

#define AUTOBOOST_UNORDERED_CONSTRUCT_IMPL(z, num_params, _)                    \
    template <                                                              \
        typename Alloc, typename T,                                         \
        AUTOBOOST_PP_ENUM_PARAMS_Z(z, num_params, typename A)                   \
    >                                                                       \
    inline void construct_value_impl(Alloc&, T* address,                    \
        autoboost::unordered::detail::AUTOBOOST_PP_CAT(emplace_args,num_params) <   \
            AUTOBOOST_PP_ENUM_PARAMS_Z(z, num_params, A)                        \
        > const& args)                                                      \
    {                                                                       \
        new((void*) address) T(                                             \
            AUTOBOOST_PP_ENUM_##z(num_params, AUTOBOOST_UNORDERED_CALL_FORWARD,     \
                args.a));                                                   \
    }

    AUTOBOOST_PP_REPEAT_FROM_TO(4, AUTOBOOST_UNORDERED_EMPLACE_LIMIT,
        AUTOBOOST_UNORDERED_CONSTRUCT_IMPL, _)

#undef AUTOBOOST_UNORDERED_CONSTRUCT_IMPL

    // Construct with piece_construct

    template <typename Alloc, typename A, typename B,
        typename A0, typename A1, typename A2>
    inline void construct_value_impl(Alloc& alloc, std::pair<A, B>* address,
            autoboost::unordered::detail::emplace_args3<A0, A1, A2> const& args,
            typename enable_if<use_piecewise<A0>, void*>::type = 0)
    {
        autoboost::unordered::detail::func::construct_from_tuple(alloc,
            autoboost::addressof(address->first), args.a1);
        autoboost::unordered::detail::func::construct_from_tuple(alloc,
            autoboost::addressof(address->second), args.a2);
    }

#endif // AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES

}}}}

namespace autoboost { namespace unordered { namespace detail {

    ////////////////////////////////////////////////////////////////////////////
    //
    // array_constructor
    //
    // Allocate and construct an array in an exception safe manner, and
    // clean up if an exception is thrown before the container takes charge
    // of it.

    template <typename Allocator>
    struct array_constructor
    {
        typedef autoboost::unordered::detail::allocator_traits<Allocator> traits;
        typedef typename traits::pointer pointer;

        Allocator& alloc_;
        pointer ptr_;
        pointer constructed_;
        std::size_t length_;

        array_constructor(Allocator& a)
            : alloc_(a), ptr_(), constructed_(), length_(0)
        {
            constructed_ = pointer();
            ptr_ = pointer();
        }

        ~array_constructor() {
            if (ptr_) {
                for(pointer p = ptr_; p != constructed_; ++p) {
                    autoboost::unordered::detail::func::destroy(
                            autoboost::addressof(*p));
                }

                traits::deallocate(alloc_, ptr_, length_);
            }
        }

        template <typename V>
        void construct(V const& v, std::size_t l)
        {
            AUTOBOOST_ASSERT(!ptr_);
            length_ = l;
            ptr_ = traits::allocate(alloc_, length_);
            pointer end = ptr_ + static_cast<std::ptrdiff_t>(length_);
            for(constructed_ = ptr_; constructed_ != end; ++constructed_) {
                new ((void*) autoboost::addressof(*constructed_)) V(v);
            }
        }

        pointer get() const
        {
            return ptr_;
        }

        pointer release()
        {
            pointer p(ptr_);
            ptr_ = pointer();
            return p;
        }

    private:

        array_constructor(array_constructor const&);
        array_constructor& operator=(array_constructor const&);
    };
}}}

#if defined(AUTOBOOST_MSVC)
#pragma warning(pop)
#endif

#endif

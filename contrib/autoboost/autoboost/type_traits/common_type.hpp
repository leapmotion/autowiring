//  common_type.hpp  ---------------------------------------------------------//

//  Copyright 2008 Howard Hinnant
//  Copyright 2008 Beman Dawes

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_TYPE_TRAITS_COMMON_TYPE_HPP
#define AUTOBOOST_TYPE_TRAITS_COMMON_TYPE_HPP

#include <autoboost/config.hpp>

#if defined(__SUNPRO_CC) && !defined(AUTOBOOST_COMMON_TYPE_DONT_USE_TYPEOF)
#  define AUTOBOOST_COMMON_TYPE_DONT_USE_TYPEOF
#endif
#if defined(__IBMCPP__) && !defined(AUTOBOOST_COMMON_TYPE_DONT_USE_TYPEOF)
#  define AUTOBOOST_COMMON_TYPE_DONT_USE_TYPEOF
#endif

//----------------------------------------------------------------------------//
#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(AUTOBOOST_COMMON_TYPE_ARITY)
#define AUTOBOOST_COMMON_TYPE_ARITY 3
#endif

//----------------------------------------------------------------------------//
#if defined(AUTOBOOST_NO_CXX11_DECLTYPE) && !defined(AUTOBOOST_COMMON_TYPE_DONT_USE_TYPEOF)
#include <autoboost/typeof/typeof.hpp>   // autoboost wonders never cease!
#endif

//----------------------------------------------------------------------------//
#ifndef AUTOBOOST_NO_CXX11_STATIC_ASSERT
#define AUTOBOOST_COMMON_TYPE_STATIC_ASSERT(CND, MSG, TYPES) static_assert(CND,MSG)
#elif defined(AUTOBOOST_COMMON_TYPE_USES_MPL_ASSERT)
#include <autoboost/mpl/assert.hpp>
#include <autoboost/mpl/bool.hpp>
#define AUTOBOOST_COMMON_TYPE_STATIC_ASSERT(CND, MSG, TYPES)                                 \
    AUTOBOOST_MPL_ASSERT_MSG(autoboost::mpl::bool_< (CND) >::type::value, MSG, TYPES)
#else
#include <autoboost/static_assert.hpp>
#define AUTOBOOST_COMMON_TYPE_STATIC_ASSERT(CND, MSG, TYPES) AUTOBOOST_STATIC_ASSERT(CND)
#endif

#if !defined(AUTOBOOST_NO_CXX11_STATIC_ASSERT) || !defined(AUTOBOOST_COMMON_TYPE_USES_MPL_ASSERT)
#define AUTOBOOST_COMMON_TYPE_MUST_BE_A_COMPLE_TYPE "must be complete type"
#endif

#if defined(AUTOBOOST_NO_CXX11_DECLTYPE) && defined(AUTOBOOST_COMMON_TYPE_DONT_USE_TYPEOF)
#include <autoboost/type_traits/detail/common_type_imp.hpp>
#include <autoboost/type_traits/remove_cv.hpp>
#endif
#include <autoboost/mpl/if.hpp>
#include <autoboost/utility/declval.hpp>
#include <autoboost/type_traits/add_rvalue_reference.hpp>

//----------------------------------------------------------------------------//
//                                                                            //
//                           C++03 implementation of                          //
//             20.9.7.6 Other transformations [meta.trans.other]              //
//                          Written by Howard Hinnant                         //
//      Adapted for Boost by Beman Dawes, Vicente Botet and  Jeffrey Hellrung //
//                                                                            //
//----------------------------------------------------------------------------//

namespace autoboost {

// prototype
#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template<typename... T>
    struct common_type;
#else // or no specialization
    template <class T, class U = void, class V = void>
    struct common_type
    {
    public:
        typedef typename common_type<typename common_type<T, U>::type, V>::type type;
    };
#endif


// 1 arg
    template<typename T>
#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    struct common_type<T>
#else
    struct common_type<T, void, void>

#endif
    {
        AUTOBOOST_COMMON_TYPE_STATIC_ASSERT(sizeof(T) > 0, AUTOBOOST_COMMON_TYPE_MUST_BE_A_COMPLE_TYPE, (T));
    public:
        typedef T type;
    };

// 2 args
namespace type_traits_detail {

    template <class T, class U>
    struct common_type_2
    {
    private:
        AUTOBOOST_COMMON_TYPE_STATIC_ASSERT(sizeof(T) > 0, AUTOBOOST_COMMON_TYPE_MUST_BE_A_COMPLE_TYPE, (T));
        AUTOBOOST_COMMON_TYPE_STATIC_ASSERT(sizeof(U) > 0, AUTOBOOST_COMMON_TYPE_MUST_BE_A_COMPLE_TYPE, (U));
        static bool declval_bool();  // workaround gcc bug; not required by std
        static typename add_rvalue_reference<T>::type declval_T();  // workaround gcc bug; not required by std
        static typename add_rvalue_reference<U>::type declval_U();  // workaround gcc bug; not required by std
        static typename add_rvalue_reference<bool>::type declval_b();

#if !defined(AUTOBOOST_NO_CXX11_DECLTYPE)
    public:
        typedef decltype(declval<bool>() ? declval<T>() : declval<U>()) type;
#elif defined(AUTOBOOST_COMMON_TYPE_DONT_USE_TYPEOF)
    public:
    typedef typename detail_type_traits_common_type::common_type_impl<
          typename remove_cv<T>::type,
          typename remove_cv<U>::type
      >::type type;
#else
    public:
        typedef AUTOBOOST_TYPEOF_TPL(declval_b() ? declval_T() : declval_U()) type;
#endif

#if defined(__GNUC__) && __GNUC__ == 3 && __GNUC_MINOR__ == 3
    public:
        void public_dummy_function_just_to_silence_warning();
#endif
    };

    template <class T>
    struct common_type_2<T, T>
    {
        typedef T type;
    };
    }

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template <class T, class U>
    struct common_type<T, U>
#else
    template <class T, class U>
    struct common_type<T, U, void>
#endif
    : public type_traits_detail::common_type_2<T,U>
    { };


// 3 or more args
#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template<typename T, typename U, typename... V>
    struct common_type<T, U, V...> {
    public:
        typedef typename common_type<typename common_type<T, U>::type, V...>::type type;
    };
#endif
}  // namespace autoboost

#endif  // AUTOBOOST_TYPE_TRAITS_COMMON_TYPE_HPP

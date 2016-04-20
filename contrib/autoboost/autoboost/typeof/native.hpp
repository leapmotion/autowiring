// Copyright (C) 2006 Arkadiy Vertleyb
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_NATIVE_HPP_INCLUDED
#define AUTOBOOST_TYPEOF_NATIVE_HPP_INCLUDED

#ifndef MSVC_TYPEOF_HACK

#ifdef AUTOBOOST_NO_SFINAE

namespace autoboost { namespace type_of {

    template<class T>
        T& ensure_obj(const T&);

}}

#else

#include <autoboost/type_traits/is_function.hpp>
#include <autoboost/utility/enable_if.hpp>

namespace autoboost { namespace type_of {
# ifdef AUTOBOOST_NO_SFINAE
    template<class T>
    T& ensure_obj(const T&);
# else
    template<typename T>
        typename enable_if<is_function<T>, T&>::type
        ensure_obj(T&);

    template<typename T>
        typename disable_if<is_function<T>, T&>::type
        ensure_obj(const T&);
# endif
}}

#endif//AUTOBOOST_NO_SFINAE

#define AUTOBOOST_TYPEOF(expr) AUTOBOOST_TYPEOF_KEYWORD(autoboost::type_of::ensure_obj(expr))
#define AUTOBOOST_TYPEOF_TPL AUTOBOOST_TYPEOF

#define AUTOBOOST_TYPEOF_NESTED_TYPEDEF_TPL(name,expr) \
    struct name {\
        typedef AUTOBOOST_TYPEOF_TPL(expr) type;\
    };

#define AUTOBOOST_TYPEOF_NESTED_TYPEDEF(name,expr) \
    struct name {\
        typedef AUTOBOOST_TYPEOF(expr) type;\
    };

#endif//MSVC_TYPEOF_HACK

#define AUTOBOOST_TYPEOF_REGISTER_TYPE(x)
#define AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(x, params)

#endif//AUTOBOOST_TYPEOF_NATIVE_HPP_INCLUDED


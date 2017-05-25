/*
Copyright 2017 Glen Joseph Fernandes
<glenjofe -at- gmail.com>

Distributed under the Boost Software License,
Version 1.0. (See accompanying file LICENSE_1_0.txt
or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_TT_MAKE_VOID_HPP_INCLUDED
#define AUTOBOOST_TT_MAKE_VOID_HPP_INCLUDED

#include <autoboost/config.hpp>

namespace autoboost {

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
template<class...>
struct make_void {
    typedef void type;
};

#if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES)
template<class... Ts>
using void_t = typename make_void<Ts...>::type;
#endif

#else /* AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES */

template<class = void,
         class = void,
         class = void,
         class = void,
         class = void>
struct make_void {
    typedef void type;
};

#if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES)
template<class A = void,
         class B = void,
         class C = void,
         class D = void,
         class E = void>
using void_t = typename make_void<A, B, C, D, E>::type;
#endif

#endif

} /* autoboost */

#endif

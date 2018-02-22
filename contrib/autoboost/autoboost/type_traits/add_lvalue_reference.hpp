//  Copyright 2010 John Maddock

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_TYPE_TRAITS_EXT_ADD_LVALUE_REFERENCE__HPP
#define AUTOBOOST_TYPE_TRAITS_EXT_ADD_LVALUE_REFERENCE__HPP

#include <autoboost/type_traits/add_reference.hpp>

namespace autoboost{

template <class T> struct add_lvalue_reference
{
   typedef typename autoboost::add_reference<T>::type type;
};

#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
template <class T> struct add_lvalue_reference<T&&>
{
   typedef T& type;
};
#endif

}

#endif  // AUTOBOOST_TYPE_TRAITS_EXT_ADD_LVALUE_REFERENCE__HPP

//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
//
//       This code comes from N1953 document by Howard E. Hinnant
//
//////////////////////////////////////////////////////////////////////////////


#ifndef AUTOBOOST_CONTAINER_DETAIL_VERSION_TYPE_HPP
#define AUTOBOOST_CONTAINER_DETAIL_VERSION_TYPE_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>

#include <autoboost/container/detail/mpl.hpp>
#include <autoboost/container/detail/type_traits.hpp>

namespace autoboost{
namespace container {
namespace container_detail {

//using namespace autoboost;

template <class T, unsigned V>
struct version_type
    : public container_detail::integral_constant<unsigned, V>
{
    typedef T type;

    version_type(const version_type<T, 0>&);
};

namespace impl{

template <class T,
          bool = container_detail::is_convertible<version_type<T, 0>, typename T::version>::value>
struct extract_version
{
   static const unsigned value = 1;
};

template <class T>
struct extract_version<T, true>
{
   static const unsigned value = T::version::value;
};

template <class T>
struct has_version
{
   private:
   struct two {char _[2];};
   template <class U> static two test(...);
   template <class U> static char test(const typename U::version*);
   public:
   static const bool value = sizeof(test<T>(0)) == 1;
   void dummy(){}
};

template <class T, bool = has_version<T>::value>
struct version
{
   static const unsigned value = 1;
};

template <class T>
struct version<T, true>
{
   static const unsigned value = extract_version<T>::value;
};

}  //namespace impl

template <class T>
struct version
   : public container_detail::integral_constant<unsigned, impl::version<T>::value>
{};

template<class T, unsigned N>
struct is_version
{
   static const bool value =
      is_same< typename version<T>::type, integral_constant<unsigned, N> >::value;
};

}  //namespace container_detail {
}  //namespace container {
}  //namespace autoboost{

#include <autoboost/container/detail/config_end.hpp>

#endif   //#define AUTOBOOST_CONTAINER_DETAIL_VERSION_TYPE_HPP
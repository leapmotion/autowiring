//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2014-2014.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_DETAIL_CONSTRUCT_IN_PLACE_HPP
#define AUTOBOOST_CONTAINER_DETAIL_CONSTRUCT_IN_PLACE_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/allocator_traits.hpp>
#include <autoboost/container/detail/iterators.hpp>
#include <autoboost/container/detail/value_init.hpp>

namespace autoboost {
namespace container {

//In place construction

template<class Allocator, class T, class InpIt>
AUTOBOOST_CONTAINER_FORCEINLINE void construct_in_place(Allocator &a, T* dest, InpIt source)
{     autoboost::container::allocator_traits<Allocator>::construct(a, dest, *source);  }

template<class Allocator, class T, class U, class D>
AUTOBOOST_CONTAINER_FORCEINLINE void construct_in_place(Allocator &a, T *dest, value_init_construct_iterator<U, D>)
{
   autoboost::container::allocator_traits<Allocator>::construct(a, dest);
}

template <class T, class Difference>
class default_init_construct_iterator;

template<class Allocator, class T, class U, class D>
AUTOBOOST_CONTAINER_FORCEINLINE void construct_in_place(Allocator &a, T *dest, default_init_construct_iterator<U, D>)
{
   autoboost::container::allocator_traits<Allocator>::construct(a, dest, default_init);
}

template <class T, class EmplaceFunctor, class Difference>
class emplace_iterator;

template<class Allocator, class T, class U, class EF, class D>
AUTOBOOST_CONTAINER_FORCEINLINE void construct_in_place(Allocator &a, T *dest, emplace_iterator<U, EF, D> ei)
{
   ei.construct_in_place(a, dest);
}

//Assignment

template<class DstIt, class InpIt>
AUTOBOOST_CONTAINER_FORCEINLINE void assign_in_place(DstIt dest, InpIt source)
{  *dest = *source;  }

template<class DstIt, class U, class D>
AUTOBOOST_CONTAINER_FORCEINLINE void assign_in_place(DstIt dest, value_init_construct_iterator<U, D>)
{
   container_detail::value_init<U> val;
   *dest = autoboost::move(val.get());
}

template <class DstIt, class Difference>
class default_init_construct_iterator;

template<class DstIt, class U, class D>
AUTOBOOST_CONTAINER_FORCEINLINE void assign_in_place(DstIt dest, default_init_construct_iterator<U, D>)
{
   U u;
   *dest = autoboost::move(u);
}

template <class T, class EmplaceFunctor, class Difference>
class emplace_iterator;

template<class DstIt, class U, class EF, class D>
AUTOBOOST_CONTAINER_FORCEINLINE void assign_in_place(DstIt dest, emplace_iterator<U, EF, D> ei)
{
   ei.assign_in_place(dest);
}

}  //namespace container {
}  //namespace autoboost {

#endif   //#ifndef AUTOBOOST_CONTAINER_DETAIL_CONSTRUCT_IN_PLACE_HPP

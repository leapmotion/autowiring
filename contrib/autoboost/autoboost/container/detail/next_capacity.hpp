//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2014-2015. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_CONTAINER_DETAIL_NEXT_CAPACITY_HPP
#define AUTOBOOST_CONTAINER_DETAIL_NEXT_CAPACITY_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

// container
#include <autoboost/container/throw_exception.hpp>
// container/detail
#include <autoboost/container/detail/min_max.hpp>

namespace autoboost {
namespace container {
namespace container_detail {

enum NextCapacityOption { NextCapacityDouble, NextCapacity60Percent };

template<class SizeType, NextCapacityOption Option>
struct next_capacity_calculator;

template<class SizeType>
struct next_capacity_calculator<SizeType, NextCapacityDouble>
{
   static SizeType get(const SizeType max_size
                      ,const SizeType capacity
                      ,const SizeType n)
   {
      const SizeType remaining = max_size - capacity;
      if ( remaining < n )
         autoboost::container::throw_length_error("get_next_capacity, allocator's max_size reached");
      const SizeType additional = max_value(n, capacity);
      return ( remaining < additional ) ? max_size : ( capacity + additional );
   }
};

template<class SizeType>
struct next_capacity_calculator<SizeType, NextCapacity60Percent>
{
   static SizeType get(const SizeType max_size
                     ,const SizeType capacity
                     ,const SizeType n)
   {
      const SizeType remaining = max_size - capacity;
      if ( remaining < n )
         autoboost::container::throw_length_error("get_next_capacity, allocator's max_size reached");
      const SizeType m3 = max_size/3;

      if (capacity < m3)
         return capacity + max_value(3*(capacity+1)/5, n);

      if (capacity < m3*2)
         return capacity + max_value((capacity+1)/2, n);
      return max_size;
   }
};

}  //namespace container_detail {
}  //namespace container {
}  //namespace autoboost {

#endif   //#ifndef AUTOBOOST_CONTAINER_DETAIL_NEXT_CAPACITY_HPP

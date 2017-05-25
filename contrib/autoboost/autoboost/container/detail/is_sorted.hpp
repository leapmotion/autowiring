//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2016-2016. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_CONTAINER_DETAIL_IS_SORTED_HPP
#define AUTOBOOST_CONTAINER_DETAIL_IS_SORTED_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

namespace autoboost {
namespace container {
namespace container_detail {

template <class ForwardIterator, class Pred>
bool is_sorted (ForwardIterator first, ForwardIterator last, Pred pred)
{
   if(first != last){
      ForwardIterator next = first;
      while (++next != last){
         if(pred(*next, *first))
            return false;
         ++first;
      }
   }
   return true;
}

template <class ForwardIterator, class Pred>
bool is_sorted_and_unique (ForwardIterator first, ForwardIterator last, Pred pred)
{
   if(first != last){
      ForwardIterator next = first;
      while (++next != last){
         if(!pred(*first, *next))
            return false;
         ++first;
      }
   }
   return true;
}

}  //namespace container_detail {
}  //namespace container {
}  //namespace autoboost {

#endif   //#ifndef AUTOBOOST_CONTAINER_DETAIL_IS_SORTED_HPP

//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2015-2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////

//! \file

#ifndef AUTOBOOST_MOVE_DETAIL_DESTRUCT_N_HPP
#define AUTOBOOST_MOVE_DETAIL_DESTRUCT_N_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <cstddef>

namespace autoboost {
namespace movelib{

template<class T>
class destruct_n
{
   public:
   explicit destruct_n(T *raw)
      : m_ptr(raw), m_size()
   {}

   void incr()
   {
      ++m_size;
   }

   void incr(std::size_t n)
   {
      m_size += n;
   }

   void release()
   {
      m_size = 0u;
      m_ptr = 0;
   }

   ~destruct_n()
   {
      while(m_size--){
         m_ptr[m_size].~T();
      }
   }
   private:
   T *m_ptr;
   std::size_t m_size;
};

}} //namespace autoboost {  namespace movelib{

#endif //#ifndef AUTOBOOST_MOVE_DETAIL_DESTRUCT_N_HPP

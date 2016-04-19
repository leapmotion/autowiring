//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_IN_PLACE_INTERFACE_HPP
#define AUTOBOOST_INTERPROCESS_IN_PLACE_INTERFACE_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/interprocess/detail/workaround.hpp>
#include <autoboost/interprocess/detail/type_traits.hpp>
#include <autoboost/container/detail/type_traits.hpp>  //alignment_of, aligned_storage
#include <typeinfo>  //typeid

//!\file
//!Describes an abstract interface for placement construction and destruction.

namespace autoboost {
namespace interprocess {
namespace ipcdetail {

struct in_place_interface
{
   in_place_interface(std::size_t alignm, std::size_t sz, const char *tname)
   :  alignment(alignm), size(sz), type_name(tname)
   {}

   std::size_t alignment;
   std::size_t size;
   const char *type_name;

   virtual void construct_n(void *mem, std::size_t num, std::size_t &constructed) = 0;
   virtual void destroy_n(void *mem, std::size_t num, std::size_t &destroyed) = 0;
   virtual ~in_place_interface(){}
};

template<class T>
struct placement_destroy :  public in_place_interface
{
   placement_destroy()
      :  in_place_interface(::autoboost::container::container_detail::alignment_of<T>::value, sizeof(T), typeid(T).name())
   {}

   virtual void destroy_n(void *mem, std::size_t num, std::size_t &destroyed)
   {
      T* memory = static_cast<T*>(mem);
      for(destroyed = 0; destroyed < num; ++destroyed)
         (memory++)->~T();
   }

   virtual void construct_n(void *, std::size_t, std::size_t &) {}

   private:
   void destroy(void *mem)
   {  static_cast<T*>(mem)->~T();   }
};

}
}
}   //namespace autoboost { namespace interprocess { namespace ipcdetail {

#include <autoboost/interprocess/detail/config_end.hpp>

#endif //#ifndef AUTOBOOST_INTERPROCESS_IN_PLACE_INTERFACE_HPP

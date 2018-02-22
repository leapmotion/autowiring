//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_DETAIL_ALLOCATOR_VERSION_TRAITS_HPP
#define AUTOBOOST_CONTAINER_DETAIL_ALLOCATOR_VERSION_TRAITS_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>

#include <autoboost/container/allocator_traits.hpp>             //allocator_traits
#include <autoboost/container/throw_exception.hpp>
#include <autoboost/container/detail/multiallocation_chain.hpp> //multiallocation_chain
#include <autoboost/container/detail/version_type.hpp>          //version_type
#include <autoboost/container/detail/allocation_type.hpp>       //allocation_type
#include <autoboost/container/detail/mpl.hpp>                   //integral_constant
#include <autoboost/intrusive/pointer_traits.hpp>               //pointer_traits
#include <autoboost/core/no_exceptions_support.hpp>             //AUTOBOOST_TRY

namespace autoboost {
namespace container {
namespace container_detail {

template<class Allocator, unsigned Version = autoboost::container::container_detail::version<Allocator>::value>
struct allocator_version_traits
{
   typedef ::autoboost::container::container_detail::integral_constant
      <unsigned, Version> alloc_version;

   typedef typename Allocator::multiallocation_chain multiallocation_chain;

   typedef typename autoboost::container::allocator_traits<Allocator>::pointer    pointer;
   typedef typename autoboost::container::allocator_traits<Allocator>::size_type  size_type;

   //Node allocation interface
   static pointer allocate_one(Allocator &a)
   {  return a.allocate_one();   }

   static void deallocate_one(Allocator &a, const pointer &p)
   {  a.deallocate_one(p);   }

   static void allocate_individual(Allocator &a, size_type n, multiallocation_chain &m)
   {  return a.allocate_individual(n, m);   }

   static void deallocate_individual(Allocator &a, multiallocation_chain &holder)
   {  a.deallocate_individual(holder);   }

   static pointer allocation_command(Allocator &a, allocation_type command,
                         size_type limit_size, size_type &prefer_in_recvd_out_size, pointer &reuse)
   {  return a.allocation_command(command, limit_size, prefer_in_recvd_out_size, reuse);  }
};

template<class Allocator>
struct allocator_version_traits<Allocator, 1>
{
   typedef ::autoboost::container::container_detail::integral_constant
      <unsigned, 1> alloc_version;

   typedef typename autoboost::container::allocator_traits<Allocator>::pointer    pointer;
   typedef typename autoboost::container::allocator_traits<Allocator>::size_type  size_type;
   typedef typename autoboost::container::allocator_traits<Allocator>::value_type value_type;

   typedef typename autoboost::intrusive::pointer_traits<pointer>::
         template rebind_pointer<void>::type                void_ptr;
   typedef container_detail::basic_multiallocation_chain
      <void_ptr>                                            multialloc_cached_counted;
   typedef autoboost::container::container_detail::
      transform_multiallocation_chain
         < multialloc_cached_counted, value_type>           multiallocation_chain;

   //Node allocation interface
   static pointer allocate_one(Allocator &a)
   {  return a.allocate(1);   }

   static void deallocate_one(Allocator &a, const pointer &p)
   {  a.deallocate(p, 1);   }

   static void deallocate_individual(Allocator &a, multiallocation_chain &holder)
   {
      size_type n = holder.size();
      typename multiallocation_chain::iterator it = holder.begin();
      while(n--){
         pointer p = autoboost::intrusive::pointer_traits<pointer>::pointer_to(*it);
         ++it;
         a.deallocate(p, 1);
      }
   }

   struct allocate_individual_rollback
   {
      allocate_individual_rollback(Allocator &a, multiallocation_chain &chain)
         : mr_a(a), mp_chain(&chain)
      {}

      ~allocate_individual_rollback()
      {
         if(mp_chain)
            allocator_version_traits::deallocate_individual(mr_a, *mp_chain);
      }

      void release()
      {
         mp_chain = 0;
      }

      Allocator &mr_a;
      multiallocation_chain * mp_chain;
   };

   static void allocate_individual(Allocator &a, size_type n, multiallocation_chain &m)
   {
      allocate_individual_rollback rollback(a, m);
      while(n--){
         m.push_front(a.allocate(1));
      }
      rollback.release();
   }

   static pointer allocation_command(Allocator &a, allocation_type command,
                         size_type, size_type &prefer_in_recvd_out_size, pointer &reuse)
   {
      pointer ret = pointer();
      if(AUTOBOOST_UNLIKELY(!(command & allocate_new) && !(command & nothrow_allocation))){
         throw_logic_error("version 1 allocator without allocate_new flag");
      }
      else{
         AUTOBOOST_TRY{
            ret = a.allocate(prefer_in_recvd_out_size);
         }
         AUTOBOOST_CATCH(...){
            if(!(command & nothrow_allocation)){
               AUTOBOOST_RETHROW
            }
         }
         AUTOBOOST_CATCH_END
         reuse = pointer();
      }
      return ret;
   }
};

}  //namespace container_detail {
}  //namespace container {
}  //namespace autoboost {

#include <autoboost/container/detail/config_end.hpp>

#endif // ! defined(AUTOBOOST_CONTAINER_DETAIL_ALLOCATOR_VERSION_TRAITS_HPP)

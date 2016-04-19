//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2009-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_BASIC_GLOBAL_MEMORY_HPP
#define AUTOBOOST_INTERPROCESS_BASIC_GLOBAL_MEMORY_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/interprocess/detail/workaround.hpp>

#include <autoboost/interprocess/offset_ptr.hpp>
#include <autoboost/interprocess/sync/spin/mutex.hpp>
#include <autoboost/interprocess/sync/spin/recursive_mutex.hpp>
#include <autoboost/interprocess/detail/managed_memory_impl.hpp>
#include <autoboost/interprocess/detail/managed_open_or_create_impl.hpp>
#include <autoboost/interprocess/mem_algo/rbtree_best_fit.hpp>
#include <autoboost/interprocess/indexes/iset_index.hpp>
#include <autoboost/interprocess/creation_tags.hpp>
#include <autoboost/interprocess/permissions.hpp>

namespace autoboost{
namespace interprocess{
namespace ipcdetail{

struct intermodule_singleton_mutex_family
{
   typedef autoboost::interprocess::ipcdetail::spin_mutex              mutex_type;
   typedef autoboost::interprocess::ipcdetail::spin_recursive_mutex    recursive_mutex_type;
};

struct intermodule_types
{
   //We must use offset_ptr since a loaded DLL can map the singleton holder shared memory
   //at a different address than other DLLs or the main executable
   typedef rbtree_best_fit<intermodule_singleton_mutex_family, offset_ptr<void> > mem_algo;
   template<class Device, bool FileBased>
   struct open_or_create
   {
      typedef managed_open_or_create_impl
            <Device, mem_algo::Alignment, FileBased, false> type;
   };
};

//we must implement our own managed shared memory to avoid circular dependencies
template<class Device, bool FileBased>
class basic_managed_global_memory
   : public basic_managed_memory_impl
      < char
      , intermodule_types::mem_algo
      , iset_index
      , intermodule_types::open_or_create<Device, FileBased>::type::ManagedOpenOrCreateUserOffset
      >
   , private intermodule_types::open_or_create<Device, FileBased>::type
{
   #if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
   typedef typename intermodule_types::template open_or_create<Device, FileBased>::type base2_t;

   typedef basic_managed_memory_impl
      < char
      , intermodule_types::mem_algo
      , iset_index
      , base2_t::ManagedOpenOrCreateUserOffset
      > base_t;

   typedef create_open_func<base_t>        create_open_func_t;

   basic_managed_global_memory *get_this_pointer()
   {  return this;   }

   public:
   typedef typename base_t::size_type              size_type;

   private:
   typedef typename base_t::char_ptr_holder_t   char_ptr_holder_t;
   AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE(basic_managed_global_memory)
   #endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

   public: //functions

   basic_managed_global_memory (open_or_create_t open_or_create,
                              const char *name, size_type size,
                              const void *addr = 0, const permissions& perm = permissions())
      : base_t()
      , base2_t(open_or_create, name, size, read_write, addr,
                create_open_func_t(get_this_pointer(),
                DoOpenOrCreate), perm)
   {}

   basic_managed_global_memory (open_only_t open_only, const char* name,
                                const void *addr = 0)
      : base_t()
      , base2_t(open_only, name, read_write, addr,
                create_open_func_t(get_this_pointer(),
                DoOpen))
   {}
};


}  //namespace ipcdetail{
}  //namespace interprocess{
}  //namespace autoboost{

#include <autoboost/interprocess/detail/config_end.hpp>

#endif   //#ifndef AUTOBOOST_INTERPROCESS_BASIC_GLOBAL_MEMORY_HPP

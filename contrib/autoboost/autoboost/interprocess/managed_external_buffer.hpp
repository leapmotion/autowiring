//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_MANAGED_EXTERNAL_BUFFER_HPP
#define AUTOBOOST_INTERPROCESS_MANAGED_EXTERNAL_BUFFER_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/interprocess/detail/workaround.hpp>
#include <autoboost/interprocess/creation_tags.hpp>
#include <autoboost/interprocess/detail/managed_memory_impl.hpp>
#include <autoboost/move/utility_core.hpp>
#include <autoboost/assert.hpp>
//These includes needed to fulfill default template parameters of
//predeclarations in interprocess_fwd.hpp
#include <autoboost/interprocess/mem_algo/rbtree_best_fit.hpp>
#include <autoboost/interprocess/sync/mutex_family.hpp>
#include <autoboost/interprocess/indexes/iset_index.hpp>

//!\file
//!Describes a named user memory allocation user class.

namespace autoboost {
namespace interprocess {

//!A basic user memory named object creation class. Inherits all
//!basic functionality from
//!basic_managed_memory_impl<CharType, AllocationAlgorithm, IndexType>*/
template
      <
         class CharType,
         class AllocationAlgorithm,
         template<class IndexConfig> class IndexType
      >
class basic_managed_external_buffer
   : public ipcdetail::basic_managed_memory_impl <CharType, AllocationAlgorithm, IndexType>
{
   #if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
   typedef ipcdetail::basic_managed_memory_impl
      <CharType, AllocationAlgorithm, IndexType>    base_t;
   AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE(basic_managed_external_buffer)
   #endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

   public:
   typedef typename base_t::size_type              size_type;

   //!Default constructor. Does nothing.
   //!Useful in combination with move semantics
   basic_managed_external_buffer()
   {}

   //!Creates and places the segment manager. This can throw
   basic_managed_external_buffer
      (create_only_t, void *addr, size_type size)
   {
      //Check if alignment is correct
      AUTOBOOST_ASSERT((0 == (((std::size_t)addr) & (AllocationAlgorithm::Alignment - size_type(1u)))));
      if(!base_t::create_impl(addr, size)){
         throw interprocess_exception("Could not initialize buffer in basic_managed_external_buffer constructor");
      }
   }

   //!Creates and places the segment manager. This can throw
   basic_managed_external_buffer
      (open_only_t, void *addr, size_type size)
   {
      //Check if alignment is correct
      AUTOBOOST_ASSERT((0 == (((std::size_t)addr) & (AllocationAlgorithm::Alignment - size_type(1u)))));
      if(!base_t::open_impl(addr, size)){
         throw interprocess_exception("Could not initialize buffer in basic_managed_external_buffer constructor");
      }
   }

   //!Moves the ownership of "moved"'s managed memory to *this. Does not throw
   basic_managed_external_buffer(AUTOBOOST_RV_REF(basic_managed_external_buffer) moved)
   {
      this->swap(moved);
   }

   //!Moves the ownership of "moved"'s managed memory to *this. Does not throw
   basic_managed_external_buffer &operator=(AUTOBOOST_RV_REF(basic_managed_external_buffer) moved)
   {
      basic_managed_external_buffer tmp(autoboost::move(moved));
      this->swap(tmp);
      return *this;
   }

   void grow(size_type extra_bytes)
   {  base_t::grow(extra_bytes);   }

   //!Swaps the ownership of the managed heap memories managed by *this and other.
   //!Never throws.
   void swap(basic_managed_external_buffer &other)
   {  base_t::swap(other); }
};

#ifdef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

//!Typedef for a default basic_managed_external_buffer
//!of narrow characters
typedef basic_managed_external_buffer
   <char
   ,rbtree_best_fit<null_mutex_family>
   ,iset_index>
managed_external_buffer;

//!Typedef for a default basic_managed_external_buffer
//!of wide characters
typedef basic_managed_external_buffer
   <wchar_t
   ,rbtree_best_fit<null_mutex_family>
   ,iset_index>
wmanaged_external_buffer;

#endif   //#ifdef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

}  //namespace interprocess {
}  //namespace autoboost {

#include <autoboost/interprocess/detail/config_end.hpp>

#endif   //AUTOBOOST_INTERPROCESS_MANAGED_EXTERNAL_BUFFER_HPP


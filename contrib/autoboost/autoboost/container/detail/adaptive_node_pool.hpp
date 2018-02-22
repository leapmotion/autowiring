//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_DETAIL_ADAPTIVE_NODE_POOL_HPP
#define AUTOBOOST_CONTAINER_DETAIL_ADAPTIVE_NODE_POOL_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>

#include <autoboost/intrusive/set.hpp>
#include <autoboost/container/detail/multiallocation_chain.hpp>
#include <autoboost/container/detail/pool_common_alloc.hpp>
#include <autoboost/container/detail/mutex.hpp>
#include <autoboost/container/detail/adaptive_node_pool_impl.hpp>
#include <autoboost/container/detail/multiallocation_chain.hpp>
#include <autoboost/container/detail/type_traits.hpp>

#include <cstddef>
#include <cmath>
#include <cassert>


namespace autoboost {
namespace container {
namespace container_detail {

template<bool AlignOnly>
struct select_private_adaptive_node_pool_impl
{
   typedef autoboost::container::container_detail::
         private_adaptive_node_pool_impl
            < fake_segment_manager
            , unsigned(AlignOnly)*::autoboost::container::adaptive_pool_flag::align_only
            | ::autoboost::container::adaptive_pool_flag::size_ordered | ::autoboost::container::adaptive_pool_flag::address_ordered
            > type;
};

//!Pooled memory allocator using an smart adaptive pool. Includes
//!a reference count but the class does not delete itself, this is
//!responsibility of user classes. Node size (NodeSize) and the number of
//!nodes allocated per block (NodesPerBlock) are known at compile time.
template< std::size_t NodeSize
        , std::size_t NodesPerBlock
        , std::size_t MaxFreeBlocks
        , std::size_t OverheadPercent
        >
class private_adaptive_node_pool
   :  public select_private_adaptive_node_pool_impl<(OverheadPercent == 0)>::type
{
   typedef typename select_private_adaptive_node_pool_impl<OverheadPercent == 0>::type base_t;
   //Non-copyable
   private_adaptive_node_pool(const private_adaptive_node_pool &);
   private_adaptive_node_pool &operator=(const private_adaptive_node_pool &);

   public:
   typedef typename base_t::multiallocation_chain multiallocation_chain;
   static const std::size_t nodes_per_block = NodesPerBlock;

   //!Constructor. Never throws
   private_adaptive_node_pool()
      :  base_t(0
               , NodeSize
               , NodesPerBlock
               , MaxFreeBlocks
               , (unsigned char)OverheadPercent)
   {}
};

//!Pooled memory allocator using adaptive pool. Includes
//!a reference count but the class does not delete itself, this is
//!responsibility of user classes. Node size (NodeSize) and the number of
//!nodes allocated per block (NodesPerBlock) are known at compile time
template< std::size_t NodeSize
        , std::size_t NodesPerBlock
        , std::size_t MaxFreeBlocks
        , std::size_t OverheadPercent
        >
class shared_adaptive_node_pool
   : public private_adaptive_node_pool
      <NodeSize, NodesPerBlock, MaxFreeBlocks, OverheadPercent>
{
 private:
   typedef private_adaptive_node_pool
      <NodeSize, NodesPerBlock, MaxFreeBlocks, OverheadPercent> private_node_allocator_t;
 public:
   typedef typename private_node_allocator_t::multiallocation_chain multiallocation_chain;

   //!Constructor. Never throws
   shared_adaptive_node_pool()
   : private_node_allocator_t(){}

   //!Destructor. Deallocates all allocated blocks. Never throws
   ~shared_adaptive_node_pool()
   {}

   //!Allocates array of count elements. Can throw std::bad_alloc
   void *allocate_node()
   {
      //-----------------------
      scoped_lock<default_mutex> guard(mutex_);
      //-----------------------
      return private_node_allocator_t::allocate_node();
   }

   //!Deallocates an array pointed by ptr. Never throws
   void deallocate_node(void *ptr)
   {
      //-----------------------
      scoped_lock<default_mutex> guard(mutex_);
      //-----------------------
      private_node_allocator_t::deallocate_node(ptr);
   }

   //!Allocates a singly linked list of n nodes ending in null pointer.
   //!can throw std::bad_alloc
   void allocate_nodes(const std::size_t n, multiallocation_chain &chain)
   {
      //-----------------------
      scoped_lock<default_mutex> guard(mutex_);
      //-----------------------
      return private_node_allocator_t::allocate_nodes(n, chain);
   }

   void deallocate_nodes(multiallocation_chain &chain)
   {
      //-----------------------
      scoped_lock<default_mutex> guard(mutex_);
      //-----------------------
      private_node_allocator_t::deallocate_nodes(chain);
   }

   //!Deallocates all the free blocks of memory. Never throws
   void deallocate_free_blocks()
   {
      //-----------------------
      scoped_lock<default_mutex> guard(mutex_);
      //-----------------------
      private_node_allocator_t::deallocate_free_blocks();
   }

   private:
   default_mutex mutex_;
};

}  //namespace container_detail {
}  //namespace container {
}  //namespace autoboost {

#include <autoboost/container/detail/config_end.hpp>

#endif   //#ifndef AUTOBOOST_CONTAINER_DETAIL_ADAPTIVE_NODE_POOL_HPP

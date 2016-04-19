//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2015-2015. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_DETAIL_BLOCK_SLIST_HEADER
#define AUTOBOOST_CONTAINER_DETAIL_BLOCK_SLIST_HEADER

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif


#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>
#include <autoboost/container/container_fwd.hpp>
#include <autoboost/container/pmr/memory_resource.hpp>
#include <autoboost/container/throw_exception.hpp>

#include <autoboost/move/detail/type_traits.hpp>
#include <autoboost/intrusive/linear_slist_algorithms.hpp>
#include <autoboost/assert.hpp>

#include <cstddef>

namespace autoboost {
namespace container {
namespace pmr {

struct slist_node
{
   slist_node *next;
};

struct slist_node_traits
{
   typedef slist_node         node;
   typedef slist_node*        node_ptr;
   typedef const slist_node*  const_node_ptr;

   static node_ptr get_next(const_node_ptr n)
   {  return n->next;  }

   static void set_next(const node_ptr & n, const node_ptr & next)
   {  n->next = next;  }
};

struct block_slist_header
   : public slist_node
{
   std::size_t size;
};

typedef bi::linear_slist_algorithms<slist_node_traits> slist_algo;

template<class DerivedFromBlockSlistHeader = block_slist_header>
class block_slist_base
{
   slist_node m_slist;

   static const std::size_t MaxAlignMinus1 = memory_resource::max_align-1u;

   public:

   static const std::size_t header_size = std::size_t(sizeof(DerivedFromBlockSlistHeader) + MaxAlignMinus1) & std::size_t(~MaxAlignMinus1);

   explicit block_slist_base()
   {  slist_algo::init_header(&m_slist);  }

   #if !defined(AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS) || defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
   block_slist_base(const block_slist_base&) = delete;
   block_slist_base operator=(const block_slist_base&) = delete;
   #else
   private:
   block_slist_base          (const block_slist_base&);
   block_slist_base operator=(const block_slist_base&);
   public:
   #endif

   ~block_slist_base()
   {}

   void *allocate(std::size_t size, memory_resource &mr)
   {
      if((size_t(-1) - header_size) < size)
         throw_bad_alloc();
      void *p = mr.allocate(size+header_size);
      block_slist_header &mb  = *::new((void*)p) DerivedFromBlockSlistHeader;
      mb.size = size+header_size;
      slist_algo::link_after(&m_slist, &mb);
      return (char *)p + header_size;
   }

   void release(memory_resource &mr) AUTOBOOST_NOEXCEPT
   {
      slist_node *n = slist_algo::node_traits::get_next(&m_slist);
      while(n){
         DerivedFromBlockSlistHeader &d = static_cast<DerivedFromBlockSlistHeader&>(*n);
         n = slist_algo::node_traits::get_next(n);
         std::size_t size = d.block_slist_header::size;
         d.~DerivedFromBlockSlistHeader();
         mr.deallocate(reinterpret_cast<char*>(&d), size, memory_resource::max_align);
      }
      slist_algo::init_header(&m_slist);
   }
};

class block_slist
   : public block_slist_base<>
{
   memory_resource &m_upstream_rsrc;

   public:

   explicit block_slist(memory_resource &upstream_rsrc)
      : block_slist_base<>(), m_upstream_rsrc(upstream_rsrc)
   {}

   #if !defined(AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS) || defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
   block_slist(const block_slist&) = delete;
   block_slist operator=(const block_slist&) = delete;
   #else
   private:
   block_slist          (const block_slist&);
   block_slist operator=(const block_slist&);
   public:
   #endif

   ~block_slist()
   {  this->release();  }

   void *allocate(std::size_t size)
   {  return this->block_slist_base<>::allocate(size, m_upstream_rsrc);  }

   void release() AUTOBOOST_NOEXCEPT
   {  return this->block_slist_base<>::release(m_upstream_rsrc);  }

   memory_resource& upstream_resource() const AUTOBOOST_NOEXCEPT
   {  return m_upstream_rsrc;   }
};

}  //namespace pmr {
}  //namespace container {
}  //namespace autoboost {

#include <autoboost/container/detail/config_end.hpp>

#endif   //AUTOBOOST_CONTAINER_DETAIL_BLOCK_SLIST_HEADER

//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_POOLED_NODE_ALLOCATOR_HPP
#define AUTOBOOST_CONTAINER_POOLED_NODE_ALLOCATOR_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>
#include <autoboost/container/container_fwd.hpp>
#include <autoboost/container/throw_exception.hpp>
#include <autoboost/container/detail/node_pool.hpp>
#include <autoboost/container/detail/mpl.hpp>
#include <autoboost/container/detail/multiallocation_chain.hpp>
#include <autoboost/container/detail/dlmalloc.hpp>
#include <autoboost/container/detail/singleton.hpp>

#include <autoboost/assert.hpp>
#include <autoboost/static_assert.hpp>
#include <cstddef>

namespace autoboost {
namespace container {

//!An STL node allocator that uses a modified DlMalloc as memory
//!source.
//!
//!This node allocator shares a segregated storage between all instances
//!of node_allocator with equal sizeof(T).
//!
//!NodesPerBlock is the number of nodes allocated at once when the allocator
//!runs out of nodes
#ifdef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
template
   < class T
   , std::size_t NodesPerBlock = NodeAlloc_nodes_per_block>
#else
template
   < class T
   , std::size_t NodesPerBlock
   , std::size_t Version>
#endif
class node_allocator
{
   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   //! If Version is 1, the allocator is a STL conforming allocator. If Version is 2,
   //! the allocator offers advanced expand in place and burst allocation capabilities.
   public:
   typedef unsigned int allocation_type;
   typedef node_allocator<T, NodesPerBlock, Version>   self_t;

   static const std::size_t nodes_per_block = NodesPerBlock;

   AUTOBOOST_STATIC_ASSERT((Version <=2));
   #endif

   public:
   //-------
   typedef T                                    value_type;
   typedef T *                                  pointer;
   typedef const T *                            const_pointer;
   typedef typename ::autoboost::container::
      container_detail::unvoid_ref<T>::type     reference;
   typedef typename ::autoboost::container::
      container_detail::unvoid_ref<const T>::type     const_reference;
   typedef std::size_t                          size_type;
   typedef std::ptrdiff_t                       difference_type;

   typedef autoboost::container::container_detail::
      version_type<self_t, Version>             version;

   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   typedef autoboost::container::container_detail::
      basic_multiallocation_chain<void*>              multiallocation_chain_void;
   typedef autoboost::container::container_detail::
      transform_multiallocation_chain
         <multiallocation_chain_void, T>              multiallocation_chain;
   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

   //!Obtains node_allocator from
   //!node_allocator
   template<class T2>
   struct rebind
   {
      typedef node_allocator< T2, NodesPerBlock
                            #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
                            , Version
                            #endif
                            > other;
   };

   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   private:
   //!Not assignable from related node_allocator
   template<class T2, std::size_t N2>
   node_allocator& operator=
      (const node_allocator<T2, N2>&);
   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

   public:

   //!Default constructor
   node_allocator() AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {}

   //!Copy constructor from other node_allocator.
   node_allocator(const node_allocator &) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {}

   //!Copy constructor from related node_allocator.
   template<class T2>
   node_allocator
      (const node_allocator<T2, NodesPerBlock
            #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
            , Version
            #endif
            > &) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {}

   //!Destructor
   ~node_allocator() AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {}

   //!Returns the number of elements that could be allocated.
   //!Never throws
   size_type max_size() const
   {  return size_type(-1)/sizeof(T);   }

   //!Allocate memory for an array of count elements.
   //!Throws std::bad_alloc if there is no enough memory
   pointer allocate(size_type count, const void * = 0)
   {
      if(AUTOBOOST_UNLIKELY(count > this->max_size()))
         autoboost::container::throw_bad_alloc();

      if(Version == 1 && count == 1){
         typedef container_detail::shared_node_pool
            <sizeof(T), NodesPerBlock> shared_pool_t;
         typedef container_detail::singleton_default<shared_pool_t> singleton_t;
         return pointer(static_cast<T*>(singleton_t::instance().allocate_node()));
      }
      else{
         void *ret = dlmalloc_malloc(count*sizeof(T));
         if(AUTOBOOST_UNLIKELY(!ret))
            autoboost::container::throw_bad_alloc();
         return static_cast<pointer>(ret);
      }
   }

   //!Deallocate allocated memory.
   //!Never throws
   void deallocate(const pointer &ptr, size_type count) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      (void)count;
      if(Version == 1 && count == 1){
         typedef container_detail::shared_node_pool
            <sizeof(T), NodesPerBlock> shared_pool_t;
         typedef container_detail::singleton_default<shared_pool_t> singleton_t;
         singleton_t::instance().deallocate_node(ptr);
      }
      else{
         dlmalloc_free(ptr);
      }
   }

   //!Deallocates all free blocks of the pool
   static void deallocate_free_blocks() AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      typedef container_detail::shared_node_pool
         <sizeof(T), NodesPerBlock> shared_pool_t;
      typedef container_detail::singleton_default<shared_pool_t> singleton_t;
      singleton_t::instance().deallocate_free_blocks();
   }

   pointer allocation_command
      (allocation_type command, size_type limit_size, size_type &prefer_in_recvd_out_size, pointer &reuse)
   {
      AUTOBOOST_STATIC_ASSERT(( Version > 1 ));
      pointer ret = this->priv_allocation_command(command, limit_size, prefer_in_recvd_out_size, reuse);
      if(AUTOBOOST_UNLIKELY(!ret && !(command & AUTOBOOST_CONTAINER_NOTHROW_ALLOCATION)))
         autoboost::container::throw_bad_alloc();
      return ret;
   }

   //!Returns maximum the number of objects the previously allocated memory
   //!pointed by p can hold.
   size_type size(pointer p) const AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      AUTOBOOST_STATIC_ASSERT(( Version > 1 ));
      return dlmalloc_size(p);
   }

   //!Allocates just one object. Memory allocated with this function
   //!must be deallocated only with deallocate_one().
   //!Throws bad_alloc if there is no enough memory
   pointer allocate_one()
   {
      AUTOBOOST_STATIC_ASSERT(( Version > 1 ));
      typedef container_detail::shared_node_pool
         <sizeof(T), NodesPerBlock> shared_pool_t;
      typedef container_detail::singleton_default<shared_pool_t> singleton_t;
      return (pointer)singleton_t::instance().allocate_node();
   }

   //!Allocates many elements of size == 1.
   //!Elements must be individually deallocated with deallocate_one()
   void allocate_individual(std::size_t num_elements, multiallocation_chain &chain)
   {
      AUTOBOOST_STATIC_ASSERT(( Version > 1 ));
      typedef container_detail::shared_node_pool
         <sizeof(T), NodesPerBlock> shared_pool_t;
      typedef container_detail::singleton_default<shared_pool_t> singleton_t;
      typename shared_pool_t::multiallocation_chain ch;
      singleton_t::instance().allocate_nodes(num_elements, ch);
      chain.incorporate_after(chain.before_begin(), (T*)&*ch.begin(), (T*)&*ch.last(), ch.size());
   }

   //!Deallocates memory previously allocated with allocate_one().
   //!You should never use deallocate_one to deallocate memory allocated
   //!with other functions different from allocate_one(). Never throws
   void deallocate_one(pointer p) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      AUTOBOOST_STATIC_ASSERT(( Version > 1 ));
      typedef container_detail::shared_node_pool
         <sizeof(T), NodesPerBlock> shared_pool_t;
      typedef container_detail::singleton_default<shared_pool_t> singleton_t;
      singleton_t::instance().deallocate_node(p);
   }

   void deallocate_individual(multiallocation_chain &chain) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      AUTOBOOST_STATIC_ASSERT(( Version > 1 ));
      typedef container_detail::shared_node_pool
         <sizeof(T), NodesPerBlock> shared_pool_t;
      typedef container_detail::singleton_default<shared_pool_t> singleton_t;
      typename shared_pool_t::multiallocation_chain ch(&*chain.begin(), &*chain.last(), chain.size());
      singleton_t::instance().deallocate_nodes(ch);
   }

   //!Allocates many elements of size elem_size.
   //!Elements must be individually deallocated with deallocate()
   void allocate_many(size_type elem_size, std::size_t n_elements, multiallocation_chain &chain)
   {
      AUTOBOOST_STATIC_ASSERT(( Version > 1 ));
      dlmalloc_memchain ch;
      AUTOBOOST_CONTAINER_MEMCHAIN_INIT(&ch);
      if(AUTOBOOST_UNLIKELY(!dlmalloc_multialloc_nodes(n_elements, elem_size*sizeof(T), DL_MULTIALLOC_DEFAULT_CONTIGUOUS, &ch))){
         autoboost::container::throw_bad_alloc();
      }
      chain.incorporate_after( chain.before_begin()
                             , (T*)AUTOBOOST_CONTAINER_MEMCHAIN_LASTMEM(&ch)
                             , (T*)AUTOBOOST_CONTAINER_MEMCHAIN_LASTMEM(&ch)
                             , AUTOBOOST_CONTAINER_MEMCHAIN_SIZE(&ch));
   }

   //!Allocates n_elements elements, each one of size elem_sizes[i]
   //!Elements must be individually deallocated with deallocate()
   void allocate_many(const size_type *elem_sizes, size_type n_elements, multiallocation_chain &chain)
   {
      AUTOBOOST_STATIC_ASSERT(( Version > 1 ));
      dlmalloc_memchain ch;
      dlmalloc_multialloc_arrays(n_elements, elem_sizes, sizeof(T), DL_MULTIALLOC_DEFAULT_CONTIGUOUS, &ch);
      if(AUTOBOOST_UNLIKELY(AUTOBOOST_CONTAINER_MEMCHAIN_EMPTY(&ch))){
         autoboost::container::throw_bad_alloc();
      }
      chain.incorporate_after( chain.before_begin()
                             , (T*)AUTOBOOST_CONTAINER_MEMCHAIN_LASTMEM(&ch)
                             , (T*)AUTOBOOST_CONTAINER_MEMCHAIN_LASTMEM(&ch)
                             , AUTOBOOST_CONTAINER_MEMCHAIN_SIZE(&ch));
   }

   void deallocate_many(multiallocation_chain &chain) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      AUTOBOOST_STATIC_ASSERT(( Version > 1 ));
      void *first = &*chain.begin();
      void *last  = &*chain.last();
      size_t num  = chain.size();
      dlmalloc_memchain ch;
      AUTOBOOST_CONTAINER_MEMCHAIN_INIT_FROM(&ch, first, last, num);
      dlmalloc_multidealloc(&ch);
   }

   //!Swaps allocators. Does not throw. If each allocator is placed in a
   //!different memory segment, the result is undefined.
   friend void swap(self_t &, self_t &) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {}

   //!An allocator always compares to true, as memory allocated with one
   //!instance can be deallocated by another instance
   friend bool operator==(const node_allocator &, const node_allocator &) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return true;   }

   //!An allocator always compares to false, as memory allocated with one
   //!instance can be deallocated by another instance
   friend bool operator!=(const node_allocator &, const node_allocator &) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return false;   }

   private:
   pointer priv_allocation_command
      (allocation_type command,   std::size_t limit_size
      ,size_type &prefer_in_recvd_out_size
      ,pointer &reuse)
   {
      std::size_t const preferred_size = prefer_in_recvd_out_size;
      dlmalloc_command_ret_t ret = {0 , 0};
      if((limit_size > this->max_size()) | (preferred_size > this->max_size())){
         return pointer();
      }
      std::size_t l_size = limit_size*sizeof(T);
      std::size_t p_size = preferred_size*sizeof(T);
      std::size_t r_size;
      {
         void* reuse_ptr_void = reuse;
         ret = dlmalloc_allocation_command(command, sizeof(T), l_size, p_size, &r_size, reuse_ptr_void);
         reuse = static_cast<T*>(reuse_ptr_void);
      }
      prefer_in_recvd_out_size = r_size/sizeof(T);
      return (pointer)ret.first;
   }
};

}  //namespace container {
}  //namespace autoboost {

#include <autoboost/container/detail/config_end.hpp>

#endif   //#ifndef AUTOBOOST_CONTAINER_POOLED_NODE_ALLOCATOR_HPP

//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2015-2015. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_CONTAINER_DETAIL_ALLOC_LIB_HPP
#define AUTOBOOST_CONTAINER_DETAIL_ALLOC_LIB_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>
#include <autoboost/container/detail/auto_link.hpp>

#include <autoboost/container/detail/alloc_lib.h>

namespace autoboost{
namespace container{

typedef autoboost_cont_command_ret_t dlmalloc_command_ret_t;
typedef autoboost_cont_memchain dlmalloc_memchain;
typedef autoboost_cont_memchain_it dlmalloc_memchain_it;
typedef autoboost_cont_malloc_stats_t dlmalloc_malloc_stats_t;

AUTOBOOST_CONTAINER_DECL size_t dlmalloc_size(const void *p);

AUTOBOOST_CONTAINER_DECL void* dlmalloc_malloc(size_t bytes);

AUTOBOOST_CONTAINER_DECL void  dlmalloc_free(void* mem);

AUTOBOOST_CONTAINER_DECL void* dlmalloc_memalign(size_t bytes, size_t alignment);

AUTOBOOST_CONTAINER_DECL int dlmalloc_multialloc_nodes
   (size_t n_elements, size_t elem_size, size_t contiguous_elements, autoboost_cont_memchain *pchain);

AUTOBOOST_CONTAINER_DECL int dlmalloc_multialloc_arrays
   (size_t n_elements, const size_t *sizes, size_t sizeof_element, size_t contiguous_elements, autoboost_cont_memchain *pchain);

AUTOBOOST_CONTAINER_DECL void dlmalloc_multidealloc(autoboost_cont_memchain *pchain);

AUTOBOOST_CONTAINER_DECL size_t dlmalloc_footprint();

AUTOBOOST_CONTAINER_DECL size_t dlmalloc_allocated_memory();

AUTOBOOST_CONTAINER_DECL size_t dlmalloc_chunksize(const void *p);

AUTOBOOST_CONTAINER_DECL int dlmalloc_all_deallocated();

AUTOBOOST_CONTAINER_DECL autoboost_cont_malloc_stats_t dlmalloc_malloc_stats();

AUTOBOOST_CONTAINER_DECL size_t dlmalloc_in_use_memory();

AUTOBOOST_CONTAINER_DECL int dlmalloc_trim(size_t pad);

AUTOBOOST_CONTAINER_DECL int dlmalloc_mallopt(int parameter_number, int parameter_value);

AUTOBOOST_CONTAINER_DECL int dlmalloc_grow(void* oldmem, size_t minbytes, size_t maxbytes, size_t *received);

AUTOBOOST_CONTAINER_DECL int dlmalloc_shrink(void* oldmem, size_t minbytes, size_t maxbytes, size_t *received, int do_commit);

AUTOBOOST_CONTAINER_DECL void* dlmalloc_alloc(size_t minbytes, size_t preferred_bytes, size_t *received_bytes);

AUTOBOOST_CONTAINER_DECL int dlmalloc_malloc_check();

AUTOBOOST_CONTAINER_DECL autoboost_cont_command_ret_t dlmalloc_allocation_command
   ( allocation_type command
   , size_t sizeof_object
   , size_t limit_objects
   , size_t preferred_objects
   , size_t *received_objects
   , void *reuse_ptr
   );

AUTOBOOST_CONTAINER_DECL int dlmalloc_mallopt(int param_number, int value);

AUTOBOOST_CONTAINER_DECL void *dlmalloc_sync_create();

AUTOBOOST_CONTAINER_DECL void dlmalloc_sync_destroy(void *sync);

AUTOBOOST_CONTAINER_DECL bool dlmalloc_sync_lock(void *sync);

AUTOBOOST_CONTAINER_DECL void dlmalloc_sync_unlock(void *sync);

AUTOBOOST_CONTAINER_DECL bool dlmalloc_global_sync_lock();

AUTOBOOST_CONTAINER_DECL void dlmalloc_global_sync_unlock();

}  //namespace container{
}  //namespace autoboost{

#include <autoboost/container/detail/config_end.hpp>

#endif   //AUTOBOOST_CONTAINER_DETAIL_ALLOC_LIB_HPP

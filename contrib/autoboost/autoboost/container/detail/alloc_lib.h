//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_CONTAINER_ALLOC_LIB_EXT_H
#define AUTOBOOST_CONTAINER_ALLOC_LIB_EXT_H

#include <stddef.h>

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4127)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!An forward iterator to traverse the elements of a memory chain container.*/
typedef struct multialloc_node_impl
{
   struct multialloc_node_impl *next_node_ptr;
} autoboost_cont_memchain_node;


/*!An forward iterator to traverse the elements of a memory chain container.*/
typedef struct multialloc_it_impl
{
   autoboost_cont_memchain_node *node_ptr;
} autoboost_cont_memchain_it;

/*!Memory chain: A container holding memory portions allocated by autoboost_cont_multialloc_nodes
   and autoboost_cont_multialloc_arrays functions.*/
typedef struct autoboost_cont_memchain_impl
{
   size_t                   num_mem;
   autoboost_cont_memchain_node  root_node;
   autoboost_cont_memchain_node *last_node_ptr;
} autoboost_cont_memchain;

/*!Advances the iterator one position so that it points to the next element in the memory chain*/
#define AUTOBOOST_CONTAINER_MEMIT_NEXT(IT)         (IT.node_ptr = IT.node_ptr->next_node_ptr)

/*!Returns the address of the memory chain currently pointed by the iterator*/
#define AUTOBOOST_CONTAINER_MEMIT_ADDR(IT)      ((void*)IT.node_ptr)

/*!Initializer for an iterator pointing to the position before the first element*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_BEFORE_BEGIN_IT(PMEMCHAIN)   { &((PMEMCHAIN)->root_node) }

/*!Initializer for an iterator pointing to the first element*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_BEGIN_IT(PMEMCHAIN)   {(PMEMCHAIN)->root_node.next_node_ptr }

/*!Initializer for an iterator pointing to the last element*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_LAST_IT(PMEMCHAIN)    {(PMEMCHAIN)->last_node_ptr }

/*!Initializer for an iterator pointing to one past the last element (end iterator)*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_END_IT(PMEMCHAIN)     {(autoboost_cont_memchain_node *)0 }

/*!True if IT is the end iterator, false otherwise*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_IS_END_IT(PMEMCHAIN, IT) (!(IT).node_ptr)

/*!The address of the first memory portion hold by the memory chain*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_FIRSTMEM(PMEMCHAIN)((void*)((PMEMCHAIN)->root_node.next_node_ptr))

/*!The address of the last memory portion hold by the memory chain*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_LASTMEM(PMEMCHAIN) ((void*)((PMEMCHAIN)->last_node_ptr))

/*!The number of memory portions hold by the memory chain*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_SIZE(PMEMCHAIN) ((PMEMCHAIN)->num_mem)

/*!Initializes the memory chain from the first memory portion, the last memory
   portion and number of portions obtained from another memory chain*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_INIT_FROM(PMEMCHAIN, FIRST, LAST, NUM)\
   (PMEMCHAIN)->last_node_ptr = (autoboost_cont_memchain_node *)(LAST), \
   (PMEMCHAIN)->root_node.next_node_ptr  = (autoboost_cont_memchain_node *)(FIRST), \
   (PMEMCHAIN)->num_mem  = (NUM);\
/**/

/*!Default initializes a memory chain. Postconditions: begin iterator is end iterator,
   the number of portions is zero.*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_INIT(PMEMCHAIN)\
   ((PMEMCHAIN)->root_node.next_node_ptr = 0, (PMEMCHAIN)->last_node_ptr = &((PMEMCHAIN)->root_node), (PMEMCHAIN)->num_mem = 0)\
/**/

/*!True if the memory chain is empty (holds no memory portions*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_EMPTY(PMEMCHAIN)\
   ((PMEMCHAIN)->num_mem == 0)\
/**/

/*!Inserts a new memory portions in the front of the chain*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_PUSH_BACK(PMEMCHAIN, MEM)\
   do{\
      autoboost_cont_memchain *____chain____ = (PMEMCHAIN);\
      autoboost_cont_memchain_node *____tmp_mem____ = (autoboost_cont_memchain_node *)(MEM);\
      ____chain____->last_node_ptr->next_node_ptr = ____tmp_mem____;\
      ____tmp_mem____->next_node_ptr = 0;\
      ____chain____->last_node_ptr = ____tmp_mem____;\
      ++____chain____->num_mem;\
   }while(0)\
/**/

/*!Inserts a new memory portions in the back of the chain*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_PUSH_FRONT(PMEMCHAIN, MEM)\
   do{\
      autoboost_cont_memchain *____chain____ = (PMEMCHAIN);\
      autoboost_cont_memchain_node *____tmp_mem____   = (autoboost_cont_memchain_node *)(MEM);\
      autoboost_cont_memchain *____root____  = &((PMEMCHAIN)->root_node);\
      if(!____chain____->root_node.next_node_ptr){\
         ____chain____->last_node_ptr = ____tmp_mem____;\
      }\
      autoboost_cont_memchain_node *____old_first____ = ____root____->next_node_ptr;\
      ____tmp_mem____->next_node_ptr = ____old_first____;\
      ____root____->next_node_ptr = ____tmp_mem____;\
      ++____chain____->num_mem;\
   }while(0)\
/**/

/*!Erases the memory portion after the portion pointed by BEFORE_IT from the memory chain*/
/*!Precondition: BEFORE_IT must be a valid iterator of the memory chain and it can't be the end iterator*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_ERASE_AFTER(PMEMCHAIN, BEFORE_IT)\
   do{\
      autoboost_cont_memchain *____chain____ = (PMEMCHAIN);\
      autoboost_cont_memchain_node *____prev_node____  = (BEFORE_IT).node_ptr;\
      autoboost_cont_memchain_node *____erase_node____ = ____prev_node____->next_node_ptr;\
      if(____chain____->last_node_ptr == ____erase_node____){\
         ____chain____->last_node_ptr = &____chain____->root_node;\
      }\
      ____prev_node____->next_node_ptr = ____erase_node____->next_node_ptr;\
      --____chain____->num_mem;\
   }while(0)\
/**/

/*!Erases the first portion from the memory chain.
   Precondition: the memory chain must not be empty*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_POP_FRONT(PMEMCHAIN)\
   do{\
      autoboost_cont_memchain *____chain____ = (PMEMCHAIN);\
      autoboost_cont_memchain_node *____prev_node____  = &____chain____->root_node;\
      autoboost_cont_memchain_node *____erase_node____ = ____prev_node____->next_node_ptr;\
      if(____chain____->last_node_ptr == ____erase_node____){\
         ____chain____->last_node_ptr = &____chain____->root_node;\
      }\
      ____prev_node____->next_node_ptr = ____erase_node____->next_node_ptr;\
      --____chain____->num_mem;\
   }while(0)\
/**/

/*!Joins two memory chains inserting the portions of the second chain at the back of the first chain*/
/*
#define AUTOBOOST_CONTAINER_MEMCHAIN_SPLICE_BACK(PMEMCHAIN, PMEMCHAIN2)\
   do{\
      autoboost_cont_memchain *____chain____  = (PMEMCHAIN);\
      autoboost_cont_memchain *____chain2____ = (PMEMCHAIN2);\
      if(!____chain2____->root_node.next_node_ptr){\
         break;\
      }\
      else if(!____chain____->first_mem){\
         ____chain____->first_mem  = ____chain2____->first_mem;\
         ____chain____->last_node_ptr = ____chain2____->last_node_ptr;\
         ____chain____->num_mem  = ____chain2____->num_mem;\
         AUTOBOOST_CONTAINER_MEMCHAIN_INIT(*____chain2____);\
      }\
      else{\
         ____chain____->last_node_ptr->next_node_ptr = ____chain2____->first_mem;\
         ____chain____->last_node_ptr = ____chain2____->last_node_ptr;\
         ____chain____->num_mem += ____chain2____->num_mem;\
      }\
   }while(0)\*/
/**/

/*!Joins two memory chains inserting the portions of the second chain at the back of the first chain*/
#define AUTOBOOST_CONTAINER_MEMCHAIN_INCORPORATE_AFTER(PMEMCHAIN, BEFORE_IT, FIRST, BEFORELAST, NUM)\
   do{\
      autoboost_cont_memchain *____chain____  = (PMEMCHAIN);\
      autoboost_cont_memchain_node *____pnode____  = (BEFORE_IT).node_ptr;\
      autoboost_cont_memchain_node *____next____   = ____pnode____->next_node_ptr;\
      autoboost_cont_memchain_node *____first____  = (autoboost_cont_memchain_node *)(FIRST);\
      autoboost_cont_memchain_node *____blast____  = (autoboost_cont_memchain_node *)(BEFORELAST);\
      size_t ____num____ = (NUM);\
      if(!____num____){\
         break;\
      }\
      if(____pnode____ == ____chain____->last_node_ptr){\
         ____chain____->last_node_ptr = ____blast____;\
      }\
      ____pnode____->next_node_ptr  = ____first____;\
      ____blast____->next_node_ptr  = ____next____;\
      ____chain____->num_mem  += ____num____;\
   }while(0)\
/**/

/*!Indicates the all elements allocated by autoboost_cont_multialloc_nodes or autoboost_cont_multialloc_arrays
   must be contiguous.*/
#define DL_MULTIALLOC_ALL_CONTIGUOUS        ((size_t)(-1))

/*!Indicates the number of contiguous elements allocated by autoboost_cont_multialloc_nodes or autoboost_cont_multialloc_arrays
   should be selected by those functions.*/
#define DL_MULTIALLOC_DEFAULT_CONTIGUOUS    ((size_t)(0))

typedef struct autoboost_cont_malloc_stats_impl
{
   size_t max_system_bytes;
   size_t system_bytes;
   size_t in_use_bytes;
} autoboost_cont_malloc_stats_t;

typedef unsigned int allocation_type;

enum
{
   // constants for allocation commands
   AUTOBOOST_CONTAINER_ALLOCATE_NEW          = 0X01,
   AUTOBOOST_CONTAINER_EXPAND_FWD            = 0X02,
   AUTOBOOST_CONTAINER_EXPAND_BWD            = 0X04,
   AUTOBOOST_CONTAINER_SHRINK_IN_PLACE       = 0X08,
   AUTOBOOST_CONTAINER_NOTHROW_ALLOCATION    = 0X10,
//   AUTOBOOST_CONTAINER_ZERO_MEMORY           = 0X20,
   AUTOBOOST_CONTAINER_TRY_SHRINK_IN_PLACE   = 0X40,
   AUTOBOOST_CONTAINER_EXPAND_BOTH           = AUTOBOOST_CONTAINER_EXPAND_FWD | AUTOBOOST_CONTAINER_EXPAND_BWD,
   AUTOBOOST_CONTAINER_EXPAND_OR_NEW         = AUTOBOOST_CONTAINER_ALLOCATE_NEW | AUTOBOOST_CONTAINER_EXPAND_BOTH
};

//#define AUTOBOOST_CONTAINERDLMALLOC__FOOTERS
#ifndef AUTOBOOST_CONTAINERDLMALLOC__FOOTERS
enum {   AUTOBOOST_CONTAINER_ALLOCATION_PAYLOAD = sizeof(size_t)   };
#else
enum {   AUTOBOOST_CONTAINER_ALLOCATION_PAYLOAD = sizeof(size_t)*2   };
#endif

typedef struct autoboost_cont_command_ret_impl
{
   void *first;
   int   second;
}autoboost_cont_command_ret_t;

size_t autoboost_cont_size(const void *p);

void* autoboost_cont_malloc(size_t bytes);

void  autoboost_cont_free(void* mem);

void* autoboost_cont_memalign(size_t bytes, size_t alignment);

int autoboost_cont_multialloc_nodes
   (size_t n_elements, size_t elem_size, size_t contiguous_elements, autoboost_cont_memchain *pchain);

int autoboost_cont_multialloc_arrays
   (size_t n_elements, const size_t *sizes, size_t sizeof_element, size_t contiguous_elements, autoboost_cont_memchain *pchain);

void autoboost_cont_multidealloc(autoboost_cont_memchain *pchain);

size_t autoboost_cont_footprint();

size_t autoboost_cont_allocated_memory();

size_t autoboost_cont_chunksize(const void *p);

int autoboost_cont_all_deallocated();

autoboost_cont_malloc_stats_t autoboost_cont_malloc_stats();

size_t autoboost_cont_in_use_memory();

int autoboost_cont_trim(size_t pad);

int autoboost_cont_mallopt(int parameter_number, int parameter_value);

int autoboost_cont_grow
   (void* oldmem, size_t minbytes, size_t maxbytes, size_t *received);

int autoboost_cont_shrink
   (void* oldmem, size_t minbytes, size_t maxbytes, size_t *received, int do_commit);

void* autoboost_cont_alloc
   (size_t minbytes, size_t preferred_bytes, size_t *received_bytes);

int autoboost_cont_malloc_check();

autoboost_cont_command_ret_t autoboost_cont_allocation_command
   ( allocation_type command
   , size_t sizeof_object
   , size_t limit_objects
   , size_t preferred_objects
   , size_t *received_objects
   , void *reuse_ptr
   );

void *autoboost_cont_sync_create();

void autoboost_cont_sync_destroy(void *sync);

int autoboost_cont_sync_lock(void *sync);

void autoboost_cont_sync_unlock(void *sync);

int autoboost_cont_global_sync_lock();

void autoboost_cont_global_sync_unlock();

#ifdef __cplusplus
}  //extern "C" {
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif


#endif   //#define AUTOBOOST_CONTAINERDLMALLOC__EXT_H

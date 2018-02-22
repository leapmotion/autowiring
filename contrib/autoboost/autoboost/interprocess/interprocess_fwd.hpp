//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_FWD_HPP
#define AUTOBOOST_INTERPROCESS_FWD_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#ifndef AUTOBOOST_CSTDINT_HPP
#  include <autoboost/cstdint.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/interprocess/detail/std_fwd.hpp>

//! \file
//! This header file forward declares the basic interprocess types:
//!   - autoboost::interprocess::offset_ptr;
//!   - autoboost::interprocess::permissions;
//!   - autoboost::interprocess::mapped_region;
//!   - autoboost::interprocess::file_mapping;
//!   - autoboost::interprocess::shared_memory_object;
//!   - autoboost::interprocess::windows_shared_memory;
//!   - autoboost::interprocess::xsi_shared_memory;
//!
//! The following synchronization mechanisms and locks:
//!   - autoboost::interprocess::null_mutex;
//!   - autoboost::interprocess::interprocess_mutex;
//!   - autoboost::interprocess::interprocess_recursive_mutex;
//!   - autoboost::interprocess::interprocess_semaphore;
//!   - autoboost::interprocess::named_mutex;
//!   - autoboost::interprocess::named_recursive_mutex;
//!   - autoboost::interprocess::named_semaphore;
//!   - autoboost::interprocess::interprocess_sharable_mutex;
//!   - autoboost::interprocess::interprocess_condition;
//!   - autoboost::interprocess::scoped_lock;
//!   - autoboost::interprocess::sharable_lock;
//!   - autoboost::interprocess::upgradable_lock;
//!
//! The following mutex families:
//!   - autoboost::interprocess::mutex_family;
//!   - autoboost::interprocess::null_mutex_family;
//!
//! The following allocators:
//!   - autoboost::interprocess::allocator;
//!   - autoboost::interprocess::node_allocator;
//!   - autoboost::interprocess::private_node_allocator;
//!   - autoboost::interprocess::cached_node_allocator;
//!   - autoboost::interprocess::adaptive_pool;
//!   - autoboost::interprocess::private_adaptive_pool;
//!   - autoboost::interprocess::cached_adaptive_pool;
//!
//! The following allocation algorithms:
//!   - autoboost::interprocess::simple_seq_fit;
//!   - autoboost::interprocess::rbtree_best_fit;
//!
//! The following index types:
//!   - autoboost::interprocess::flat_map_index;
//!   - autoboost::interprocess::iset_index;
//!   - autoboost::interprocess::iunordered_set_index;
//!   - autoboost::interprocess::map_index;
//!   - autoboost::interprocess::null_index;
//!   - autoboost::interprocess::unordered_map_index;
//!
//! The following managed memory types:
//!   - autoboost::interprocess::segment_manager;
//!   - autoboost::interprocess::basic_managed_external_buffer
//!   - autoboost::interprocess::managed_external_buffer
//!   - autoboost::interprocess::wmanaged_external_buffer
//!   - autoboost::interprocess::basic_managed_shared_memory
//!   - autoboost::interprocess::managed_shared_memory
//!   - autoboost::interprocess::wmanaged_shared_memory
//!   - autoboost::interprocess::basic_managed_windows_shared_memory
//!   - autoboost::interprocess::managed_windows_shared_memory
//!   - autoboost::interprocess::wmanaged_windows_shared_memory
//!   - autoboost::interprocess::basic_managed_xsi_shared_memory
//!   - autoboost::interprocess::managed_xsi_shared_memory
//!   - autoboost::interprocess::wmanaged_xsi_shared_memory
//!   - autoboost::interprocess::fixed_managed_shared_memory
//!   - autoboost::interprocess::wfixed_managed_shared_memory
//!   - autoboost::interprocess::basic_managed_heap_memory
//!   - autoboost::interprocess::managed_heap_memory
//!   - autoboost::interprocess::wmanaged_heap_memory
//!   - autoboost::interprocess::basic_managed_mapped_file
//!   - autoboost::interprocess::managed_mapped_file
//!   - autoboost::interprocess::wmanaged_mapped_file
//!
//! The following exception types:
//!   - autoboost::interprocess::interprocess_exception
//!   - autoboost::interprocess::lock_exception
//!   - autoboost::interprocess::bad_alloc
//!
//! The following stream types:
//!   - autoboost::interprocess::basic_bufferbuf
//!   - autoboost::interprocess::basic_ibufferstream
//!   - autoboost::interprocess::basic_obufferstream
//!   - autoboost::interprocess::basic_bufferstream
//!   - autoboost::interprocess::basic_vectorbuf
//!   - autoboost::interprocess::basic_ivectorstream
//!   - autoboost::interprocess::basic_ovectorstream
//!   - autoboost::interprocess::basic_vectorstream
//!
//! The following smart pointer types:
//!   - autoboost::interprocess::scoped_ptr
//!   - autoboost::interprocess::intrusive_ptr
//!   - autoboost::interprocess::shared_ptr
//!   - autoboost::interprocess::weak_ptr
//!
//! The following interprocess communication types:
//!   - autoboost::interprocess::message_queue_t;
//!   - autoboost::interprocess::message_queue;

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/interprocess/detail/workaround.hpp>

#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

#include <cstddef>

//////////////////////////////////////////////////////////////////////////////
//                        Standard predeclarations
//////////////////////////////////////////////////////////////////////////////

namespace autoboost{  namespace intrusive{ }  }
namespace autoboost{  namespace interprocess{ namespace bi = autoboost::intrusive; }  }

namespace autoboost { namespace interprocess {

//////////////////////////////////////////////////////////////////////////////
//                            permissions
//////////////////////////////////////////////////////////////////////////////

class permissions;

//////////////////////////////////////////////////////////////////////////////
//                            shared_memory
//////////////////////////////////////////////////////////////////////////////

class shared_memory_object;

#if defined (AUTOBOOST_INTERPROCESS_WINDOWS)
class windows_shared_memory;
#endif   //#if defined (AUTOBOOST_INTERPROCESS_WINDOWS)

#if defined(AUTOBOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS)
class xsi_shared_memory;
#endif   //#if defined (AUTOBOOST_INTERPROCESS_WINDOWS)

//////////////////////////////////////////////////////////////////////////////
//              file mapping / mapped region
//////////////////////////////////////////////////////////////////////////////

class file_mapping;
class mapped_region;

//////////////////////////////////////////////////////////////////////////////
//                               Mutexes
//////////////////////////////////////////////////////////////////////////////

class null_mutex;

class interprocess_mutex;
class interprocess_recursive_mutex;

class named_mutex;
class named_recursive_mutex;

class interprocess_semaphore;
class named_semaphore;

//////////////////////////////////////////////////////////////////////////////
//                         Mutex families
//////////////////////////////////////////////////////////////////////////////

struct mutex_family;
struct null_mutex_family;

//////////////////////////////////////////////////////////////////////////////
//                   Other synchronization classes
//////////////////////////////////////////////////////////////////////////////

class interprocess_sharable_mutex;
class interprocess_condition;

//////////////////////////////////////////////////////////////////////////////
//                              Locks
//////////////////////////////////////////////////////////////////////////////

template <class Mutex>
class scoped_lock;

template <class SharableMutex>
class sharable_lock;

template <class UpgradableMutex>
class upgradable_lock;

//////////////////////////////////////////////////////////////////////////////
//                      STL compatible allocators
//////////////////////////////////////////////////////////////////////////////

template<class T, class SegmentManager>
class allocator;

template<class T, class SegmentManager, std::size_t NodesPerBlock = 64>
class node_allocator;

template<class T, class SegmentManager, std::size_t NodesPerBlock = 64>
class private_node_allocator;

template<class T, class SegmentManager, std::size_t NodesPerBlock = 64>
class cached_node_allocator;

template< class T, class SegmentManager, std::size_t NodesPerBlock = 64
        , std::size_t MaxFreeBlocks = 2, unsigned char OverheadPercent = 5 >
class adaptive_pool;

template< class T, class SegmentManager, std::size_t NodesPerBlock = 64
        , std::size_t MaxFreeBlocks = 2, unsigned char OverheadPercent = 5 >
class private_adaptive_pool;

template< class T, class SegmentManager, std::size_t NodesPerBlock = 64
        , std::size_t MaxFreeBlocks = 2, unsigned char OverheadPercent = 5 >
class cached_adaptive_pool;


//////////////////////////////////////////////////////////////////////////////
//                            offset_ptr
//////////////////////////////////////////////////////////////////////////////

static const std::size_t offset_type_alignment = 0;

#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
#  ifdef AUTOBOOST_HAS_INTPTR_T
      using ::autoboost::uintptr_t;
#  else
      typedef std::size_t uintptr_t;
#  endif
#endif

template < class T, class DifferenceType = std::ptrdiff_t
         , class OffsetType = uintptr_t, std::size_t Alignment = offset_type_alignment>
class offset_ptr;

//////////////////////////////////////////////////////////////////////////////
//                    Memory allocation algorithms
//////////////////////////////////////////////////////////////////////////////

//Single segment memory allocation algorithms
template<class MutexFamily, class VoidMutex = offset_ptr<void> >
class simple_seq_fit;

template<class MutexFamily, class VoidMutex = offset_ptr<void>, std::size_t MemAlignment = 0>
class rbtree_best_fit;

//////////////////////////////////////////////////////////////////////////////
//                         Index Types
//////////////////////////////////////////////////////////////////////////////

template<class IndexConfig> class flat_map_index;
template<class IndexConfig> class iset_index;
template<class IndexConfig> class iunordered_set_index;
template<class IndexConfig> class map_index;
template<class IndexConfig> class null_index;
template<class IndexConfig> class unordered_map_index;

//////////////////////////////////////////////////////////////////////////////
//                         Segment manager
//////////////////////////////////////////////////////////////////////////////

template <class CharType
         ,class MemoryAlgorithm
         ,template<class IndexConfig> class IndexType>
class segment_manager;

//////////////////////////////////////////////////////////////////////////////
//                  External buffer managed memory classes
//////////////////////////////////////////////////////////////////////////////

template <class CharType
         ,class MemoryAlgorithm
         ,template<class IndexConfig> class IndexType>
class basic_managed_external_buffer;

typedef basic_managed_external_buffer
   <char
   ,rbtree_best_fit<null_mutex_family>
   ,iset_index>
managed_external_buffer;

typedef basic_managed_external_buffer
   <wchar_t
   ,rbtree_best_fit<null_mutex_family>
   ,iset_index>
wmanaged_external_buffer;

//////////////////////////////////////////////////////////////////////////////
//                      managed memory classes
//////////////////////////////////////////////////////////////////////////////

template <class CharType
         ,class MemoryAlgorithm
         ,template<class IndexConfig> class IndexType>
class basic_managed_shared_memory;

typedef basic_managed_shared_memory
   <char
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
managed_shared_memory;

typedef basic_managed_shared_memory
   <wchar_t
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
wmanaged_shared_memory;


//////////////////////////////////////////////////////////////////////////////
//                      Windows shared memory managed memory classes
//////////////////////////////////////////////////////////////////////////////

#if defined (AUTOBOOST_INTERPROCESS_WINDOWS)

template <class CharType
         ,class MemoryAlgorithm
         ,template<class IndexConfig> class IndexType>
class basic_managed_windows_shared_memory;

typedef basic_managed_windows_shared_memory
   <char
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
managed_windows_shared_memory;

typedef basic_managed_windows_shared_memory
   <wchar_t
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
wmanaged_windows_shared_memory;

#endif   //#if defined (AUTOBOOST_INTERPROCESS_WINDOWS)

#if defined(AUTOBOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS)

template <class CharType
         ,class MemoryAlgorithm
         ,template<class IndexConfig> class IndexType>
class basic_managed_xsi_shared_memory;

typedef basic_managed_xsi_shared_memory
   <char
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
managed_xsi_shared_memory;

typedef basic_managed_xsi_shared_memory
   <wchar_t
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
wmanaged_xsi_shared_memory;

#endif //#if defined(AUTOBOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS)

//////////////////////////////////////////////////////////////////////////////
//                      Fixed address shared memory
//////////////////////////////////////////////////////////////////////////////

typedef basic_managed_shared_memory
   <char
   ,rbtree_best_fit<mutex_family, void*>
   ,iset_index>
fixed_managed_shared_memory;

typedef basic_managed_shared_memory
   <wchar_t
   ,rbtree_best_fit<mutex_family, void*>
   ,iset_index>
wfixed_managed_shared_memory;

//////////////////////////////////////////////////////////////////////////////
//                      Heap memory managed memory classes
//////////////////////////////////////////////////////////////////////////////

template
   <class CharType
   ,class MemoryAlgorithm
   ,template<class IndexConfig> class IndexType>
class basic_managed_heap_memory;

typedef basic_managed_heap_memory
   <char
   ,rbtree_best_fit<null_mutex_family>
   ,iset_index>
managed_heap_memory;

typedef basic_managed_heap_memory
   <wchar_t
   ,rbtree_best_fit<null_mutex_family>
   ,iset_index>
wmanaged_heap_memory;

//////////////////////////////////////////////////////////////////////////////
//                         Mapped file managed memory classes
//////////////////////////////////////////////////////////////////////////////

template
   <class CharType
   ,class MemoryAlgorithm
   ,template<class IndexConfig> class IndexType>
class basic_managed_mapped_file;

typedef basic_managed_mapped_file
   <char
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
managed_mapped_file;

typedef basic_managed_mapped_file
   <wchar_t
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
wmanaged_mapped_file;

//////////////////////////////////////////////////////////////////////////////
//                            Exceptions
//////////////////////////////////////////////////////////////////////////////

class interprocess_exception;
class lock_exception;
class bad_alloc;

//////////////////////////////////////////////////////////////////////////////
//                            Bufferstream
//////////////////////////////////////////////////////////////////////////////

//bufferstream
template <class CharT
         ,class CharTraits = std::char_traits<CharT> >
class basic_bufferbuf;

template <class CharT
         ,class CharTraits = std::char_traits<CharT> >
class basic_ibufferstream;

template <class CharT
         ,class CharTraits = std::char_traits<CharT> >
class basic_obufferstream;

template <class CharT
         ,class CharTraits = std::char_traits<CharT> >
class basic_bufferstream;

//////////////////////////////////////////////////////////////////////////////
//                            Vectorstream
//////////////////////////////////////////////////////////////////////////////

template <class CharVector
         ,class CharTraits = std::char_traits<typename CharVector::value_type> >
class basic_vectorbuf;

template <class CharVector
         ,class CharTraits = std::char_traits<typename CharVector::value_type> >
class basic_ivectorstream;

template <class CharVector
         ,class CharTraits = std::char_traits<typename CharVector::value_type> >
class basic_ovectorstream;

template <class CharVector
         ,class CharTraits = std::char_traits<typename CharVector::value_type> >
class basic_vectorstream;

//////////////////////////////////////////////////////////////////////////////
//                             Smart pointers
//////////////////////////////////////////////////////////////////////////////

template<class T, class Deleter>
class scoped_ptr;

template<class T, class VoidPointer>
class intrusive_ptr;

template<class T, class VoidAllocator, class Deleter>
class shared_ptr;

template<class T, class VoidAllocator, class Deleter>
class weak_ptr;

//////////////////////////////////////////////////////////////////////////////
//                                  IPC
//////////////////////////////////////////////////////////////////////////////

template<class VoidPointer>
class message_queue_t;

typedef message_queue_t<offset_ptr<void> > message_queue;

}}  //namespace autoboost { namespace interprocess {

#endif   //#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

#include <autoboost/interprocess/detail/config_end.hpp>

#endif //#ifndef AUTOBOOST_INTERPROCESS_FWD_HPP

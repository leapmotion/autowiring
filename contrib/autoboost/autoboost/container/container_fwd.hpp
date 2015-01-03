//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_CONTAINER_FWD_HPP
#define AUTOBOOST_CONTAINER_CONTAINER_FWD_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

//! \file
//! This header file forward declares the following containers:
//!   - autoboost::container::vector
//!   - autoboost::container::stable_vector
//!   - autoboost::container::static_vector
//!   - autoboost::container::slist
//!   - autoboost::container::list
//!   - autoboost::container::set
//!   - autoboost::container::multiset
//!   - autoboost::container::map
//!   - autoboost::container::multimap
//!   - autoboost::container::flat_set
//!   - autoboost::container::flat_multiset
//!   - autoboost::container::flat_map
//!   - autoboost::container::flat_multimap
//!   - autoboost::container::basic_string
//!   - autoboost::container::string
//!   - autoboost::container::wstring
//!
//! It forward declares the following allocators:
//!   - autoboost::container::allocator
//!   - autoboost::container::node_allocator
//!   - autoboost::container::adaptive_pool
//!
//! And finally it defines the following types

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

//Std forward declarations
#ifndef AUTOBOOST_CONTAINER_DETAIL_STD_FWD_HPP
   #include <autoboost/container/detail/std_fwd.hpp>
#endif

namespace autoboost{
namespace intrusive{
   //Create namespace to avoid compilation errors
}}

namespace autoboost{ namespace container{ namespace container_detail{
   namespace bi = autoboost::intrusive;
}}}

#include <cstddef>

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

//////////////////////////////////////////////////////////////////////////////
//                             Containers
//////////////////////////////////////////////////////////////////////////////

namespace autoboost {
namespace container {

//! Enumeration used to configure ordered associative containers
//! with a concrete tree implementation.
enum tree_type_enum
{
   red_black_tree,
   avl_tree,
   scapegoat_tree,
   splay_tree
};

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

template <class T
         ,class Allocator = std::allocator<T> >
class vector;

template <class T
         ,class Allocator = std::allocator<T> >
class stable_vector;

template <class T, std::size_t Capacity>
class static_vector;

template <class T
         ,class Allocator = std::allocator<T> >
class deque;

template <class T
         ,class Allocator = std::allocator<T> >
class list;

template <class T
         ,class Allocator = std::allocator<T> >
class slist;

template<tree_type_enum TreeType, bool OptimizeSize>
struct tree_opt;

typedef tree_opt<red_black_tree, true> tree_assoc_defaults;

template <class Key
         ,class Compare  = std::less<Key>
         ,class Allocator = std::allocator<Key>
         ,class Options = tree_assoc_defaults >
class set;

template <class Key
         ,class Compare  = std::less<Key>
         ,class Allocator = std::allocator<Key>
         ,class Options = tree_assoc_defaults >
class multiset;

template <class Key
         ,class T
         ,class Compare  = std::less<Key>
         ,class Allocator = std::allocator<std::pair<const Key, T> >
         ,class Options = tree_assoc_defaults >
class map;

template <class Key
         ,class T
         ,class Compare  = std::less<Key>
         ,class Allocator = std::allocator<std::pair<const Key, T> >
         ,class Options = tree_assoc_defaults >
class multimap;

template <class Key
         ,class Compare  = std::less<Key>
         ,class Allocator = std::allocator<Key> >
class flat_set;

template <class Key
         ,class Compare  = std::less<Key>
         ,class Allocator = std::allocator<Key> >
class flat_multiset;

template <class Key
         ,class T
         ,class Compare  = std::less<Key>
         ,class Allocator = std::allocator<std::pair<Key, T> > >
class flat_map;

template <class Key
         ,class T
         ,class Compare  = std::less<Key>
         ,class Allocator = std::allocator<std::pair<Key, T> > >
class flat_multimap;

template <class CharT
         ,class Traits = std::char_traits<CharT>
         ,class Allocator  = std::allocator<CharT> >
class basic_string;

typedef basic_string
   <char
   ,std::char_traits<char>
   ,std::allocator<char> >
string;

typedef basic_string
   <wchar_t
   ,std::char_traits<wchar_t>
   ,std::allocator<wchar_t> >
wstring;

static const std::size_t ADP_nodes_per_block    = 256u;
static const std::size_t ADP_max_free_blocks    = 2u;
static const std::size_t ADP_overhead_percent   = 1u;
static const std::size_t ADP_only_alignment     = 0u;

template < class T
         , std::size_t NodesPerBlock   = ADP_nodes_per_block
         , std::size_t MaxFreeBlocks   = ADP_max_free_blocks
         , std::size_t OverheadPercent = ADP_overhead_percent
         , unsigned Version = 2
         >
class adaptive_pool;

template < class T
         , unsigned Version = 2
         , unsigned int AllocationDisableMask = 0>
class allocator;

static const std::size_t NodeAlloc_nodes_per_block = 256u;

template
   < class T
   , std::size_t NodesPerBlock = NodeAlloc_nodes_per_block
   , std::size_t Version = 2>
class node_allocator;

#else

//! Default options for tree-based associative containers
//!   - tree_type<red_black_tree>
//!   - optimize_size<true>
typedef implementation_defined tree_assoc_defaults;

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

//! Type used to tag that the input range is
//! guaranteed to be ordered
struct ordered_range_t
{};

//! Value used to tag that the input range is
//! guaranteed to be ordered
static const ordered_range_t ordered_range = ordered_range_t();

//! Type used to tag that the input range is
//! guaranteed to be ordered and unique
struct ordered_unique_range_t
   : public ordered_range_t
{};

//! Value used to tag that the input range is
//! guaranteed to be ordered and unique
static const ordered_unique_range_t ordered_unique_range = ordered_unique_range_t();

//! Type used to tag that the inserted values
//! should be default initialized
struct default_init_t
{};

//! Value used to tag that the inserted values
//! should be default initialized
static const default_init_t default_init = default_init_t();
#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

//! Type used to tag that the inserted values
//! should be value initialized
struct value_init_t
{};

//! Value used to tag that the inserted values
//! should be value initialized
static const value_init_t value_init = value_init_t();

namespace container_detail_really_deep_namespace {

//Otherwise, gcc issues a warning of previously defined
//anonymous_instance and unique_instance
struct dummy
{
   dummy()
   {
      (void)ordered_range;
      (void)ordered_unique_range;
      (void)default_init;
   }
};

}  //detail_really_deep_namespace {


#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

}}  //namespace autoboost { namespace container {

#endif //#ifndef AUTOBOOST_CONTAINER_CONTAINER_FWD_HPP

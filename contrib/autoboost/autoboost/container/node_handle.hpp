//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2016-2016. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_NODE_HANDLE_HPP
#define AUTOBOOST_CONTAINER_NODE_HANDLE_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/container/detail/placement_new.hpp>
#include <autoboost/container/detail/to_raw_pointer.hpp>
#include <autoboost/container/allocator_traits.hpp>
#include <autoboost/container/detail/mpl.hpp>

#include <autoboost/move/utility_core.hpp>
#include <autoboost/move/adl_move_swap.hpp>

#include <autoboost/type_traits/aligned_storage.hpp>


//!\file

namespace autoboost {
namespace container {

///@cond

template<class Value, class KeyMapped, bool keymapped_is_void = container_detail::is_same<KeyMapped, void>::value>
struct node_handle_keymapped_traits
{
   typedef Value key_type;
   typedef Value mapped_type;
};

template<class Value, class KeyMapped>
struct node_handle_keymapped_traits<Value, KeyMapped, false>
{
   typedef typename KeyMapped::key_type      key_type;
   typedef typename KeyMapped::mapped_type   mapped_type;
};

///@endcond

//! A node_handle is an object that accepts ownership of a single element from an associative container.
//! It may be used to transfer that ownership to another container with compatible nodes. Containers
//! with compatible nodes have the same node handle type. Elements may be transferred in either direction
//! between container types in the same row:.
//!
//! Container types with compatible nodes
//!
//! map<K, T, C1, A> <-> map<K, T, C2, A>
//!
//! map<K, T, C1, A> <-> multimap<K, T, C2, A>
//!
//! set<K, C1, A> <-> set<K, C2, A>
//!
//! set<K, C1, A> <-> multiset<K, C2, A>
//!
//! If a node handle is not empty, then it contains an allocator that is equal to the allocator of the container
//! when the element was extracted. If a node handle is empty, it contains no allocator.
template <class NodeType, class Value, class Allocator, class KeyMapped = void>
class node_handle
{
   typedef node_handle_keymapped_traits<Value, KeyMapped> keymapped_t;

   public:
   typedef Value                                                  value_type;
   typedef typename keymapped_t::key_type                         key_type;
   typedef typename keymapped_t::mapped_type                      mapped_type;
   typedef Allocator                                              allocator_type;
   typedef NodeType                                               container_node_type;

   ///@cond
   private:
   AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE(node_handle)

   typedef allocator_traits<allocator_type>                                   ator_traits;
   typedef typename ator_traits::template portable_rebind_alloc
      <container_node_type>::type                                             nallocator_type;
   typedef allocator_traits<nallocator_type>                                  node_ator_traits;
   typedef typename node_ator_traits::pointer                                 node_pointer;
   typedef ::autoboost::aligned_storage
      <sizeof(allocator_type), autoboost::alignment_of<nallocator_type>::value>   nalloc_storage_t;

   node_pointer      m_ptr;
   nalloc_storage_t  m_nalloc_storage;

   void move_construct_alloc(nallocator_type &al)
   {  ::new(m_nalloc_storage.address(), autoboost_container_new_t()) allocator_type(::autoboost::move(al));   }

   void destroy_node()
   {
      node_ator_traits::destroy(this->node_alloc(), container_detail::to_raw_pointer(m_ptr));
      node_ator_traits::deallocate(this->node_alloc(), m_ptr, 1u);
   }

   template<class OtherNodeHandle>
   void move_construct_end(OtherNodeHandle &nh)
   {
      if(m_ptr){
         ::new (m_nalloc_storage.address(), autoboost_container_new_t()) allocator_type(::autoboost::move(nh.node_alloc()));
         nh.destroy_alloc();
         nh.get_node_pointer() = node_pointer();
      }
      AUTOBOOST_ASSERT(nh.empty());
   }

   public:

   void destroy_alloc()
   {  static_cast<allocator_type*>(m_nalloc_storage.address())->~allocator_type();  }

   node_pointer &get_node_pointer()
   {  return m_ptr;  }

   nallocator_type &node_alloc()
   {  return *static_cast<nallocator_type*>(m_nalloc_storage.address()); }

   const nallocator_type &node_alloc() const
   {  return *static_cast<const nallocator_type*>(m_nalloc_storage.address()); }

   node_pointer release()
   {
      node_pointer p(m_ptr);
      m_ptr = node_pointer();
      if(p)
         this->destroy_alloc();
      return p;
   }

   ///@endcond

   public:
   //! <b>Effects</b>: Initializes m_ptr to nullptr.
   //!
   //! <b>Postcondition</b>: this->empty()
   AUTOBOOST_CXX14_CONSTEXPR node_handle() AUTOBOOST_NOEXCEPT
      :  m_ptr(), m_nalloc_storage()
   {  AUTOBOOST_ASSERT(this->empty()); }

   //! <b>Effects</b>: Constructs a node_handle object initializing internal pointer with p.
   //!   If p != nullptr copy constructs internal allocator al.
   node_handle(node_pointer p, const nallocator_type &al) AUTOBOOST_NOEXCEPT
      :  m_ptr(p), m_nalloc_storage()
   {
      if(m_ptr){
         ::new (m_nalloc_storage.address(), autoboost_container_new_t()) nallocator_type(al);
      }
   }

   //! <b>Effects</b>: Constructs a node_handle object initializing internal pointer with a related nh's internal pointer
   //!   and assigns nullptr to the later. If nh's internal pointer was not nullptr, move constructs internal
   //!   allocator with nh's internal allocator and destroy nh's internal allocator.
   //!
   //! <b>Postcondition</b>: nh.empty()
   //!
   //! <b>Note</b>: Two node_handle's are related if only one of KeyMapped template parameter
   //!   of a node handle is void.
   template<class KeyMapped2>
   node_handle( AUTOBOOST_RV_REF_BEG node_handle<NodeType, Value, Allocator, KeyMapped2> AUTOBOOST_RV_REF_END nh
               , typename container_detail::enable_if_c
                  < ((unsigned)container_detail::is_same<KeyMapped,  void>::value +
                     (unsigned)container_detail::is_same<KeyMapped2, void>::value) == 1u
                  >::type* = 0)
      :  m_ptr(nh.get_node_pointer()), m_nalloc_storage()
   {  this->move_construct_end(nh);  }

   //! <b>Effects</b>: Constructs a node_handle object initializing internal pointer with nh's internal pointer
   //!   and assigns nullptr to the later. If nh's internal pointer was not nullptr, move constructs internal
   //!   allocator with nh's internal allocator and destroy nh's internal allocator.
   //!
   //! <b>Postcondition</b>: nh.empty()
   node_handle (AUTOBOOST_RV_REF(node_handle) nh) AUTOBOOST_NOEXCEPT
      : m_ptr(nh.m_ptr), m_nalloc_storage()
   {  this->move_construct_end(nh);  }

   //! <b>Effects</b>: If !this->empty(), destroys the value_type subobject in the container_node_type object
   //!   pointed to by c by calling allocator_traits<impl_defined>::destroy, then deallocates m_ptr by calling
   //!   ator_traits::rebind_traits<container_node_type>::deallocate.
   ~node_handle () AUTOBOOST_NOEXCEPT
   {
      if(!this->empty()){
         this->destroy_node();
         this->destroy_alloc();
      }
   }

   //! <b>Requires</b>: Either this->empty(), or ator_traits::propagate_on_container_move_assignment is true, or
   //!   node_alloc() == nh.node_alloc().
   //!
   //! <b>Effects</b>: If m_ptr != nullptr, destroys the value_type subobject in the container_node_type object
   //!   pointed to by m_ptr by calling ator_traits::destroy, then deallocates m_ptr by calling ator_-
   //!   traits::rebind_traits<container_node_type>::deallocate. Assigns nh.m_ptr to m_ptr. If this->empty()
   //!   or ator_traits::propagate_on_container_move_assignment is true, move assigns nh.node_alloc() to
   //!   node_alloc(). Assigns nullptr to nh.m_ptr and assigns nullopt to nh.node_alloc().
   //! <b>Returns</b>: *this.
   //!
   //! <b>Throws</b>: Nothing.
   node_handle & operator=(AUTOBOOST_RV_REF(node_handle) nh)
   {
      AUTOBOOST_ASSERT(this->empty() || nh.empty() || ator_traits::propagate_on_container_move_assignment::value
                   || ator_traits::equal(node_alloc(), nh.node_alloc()));

      bool const was_this_non_null = !this->empty();
      bool const was_nh_non_null   = !nh.empty();

      if(was_nh_non_null){
         if(was_this_non_null){
            this->destroy_node();
            if(ator_traits::propagate_on_container_move_assignment::value){
               this->node_alloc() = ::autoboost::move(nh.node_alloc());
            }
         }
         else{
            this->move_construct_alloc(nh.node_alloc());
         }
         m_ptr = nh.m_ptr;
         nh.m_ptr = node_pointer();
         nh.destroy_alloc();
      }
      else if(was_this_non_null){
         this->destroy_node();
         this->destroy_alloc();
         m_ptr = node_pointer();
      }
      return *this;
   }

   //! <b>Requires</b>: empty() == false.
   //!
   //! <b>Returns</b>: A reference to the value_type subobject in the container_node_type object pointed to by m_ptr
   //!
   //! <b>Throws</b>: Nothing.
   value_type& value() const AUTOBOOST_NOEXCEPT
   {
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<KeyMapped, void>::value));
      AUTOBOOST_ASSERT(!empty());
      return m_ptr->get_data();
   }

   //! <b>Requires</b>: empty() == false.
   //!
   //! <b>Returns</b>: A non-const reference to the key_type member of the value_type subobject in the
   //!   container_node_type object pointed to by m_ptr.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Requires</b>: Modifying the key through the returned reference is permitted.
   key_type& key() const AUTOBOOST_NOEXCEPT
   {
      AUTOBOOST_STATIC_ASSERT((!container_detail::is_same<KeyMapped, void>::value));
      AUTOBOOST_ASSERT(!empty());
      return const_cast<key_type &>(KeyMapped().key_of_value(m_ptr->get_data()));
   }

   //! <b>Requires</b>: empty() == false.
   //!
   //! <b>Returns</b>: A reference to the mapped_type member of the value_type subobject
   //!   in the container_node_type object pointed to by m_ptr
   //!
   //! <b>Throws</b>: Nothing.
   mapped_type& mapped() const AUTOBOOST_NOEXCEPT
   {
      AUTOBOOST_STATIC_ASSERT((!container_detail::is_same<KeyMapped, void>::value));
      AUTOBOOST_ASSERT(!empty());
      return KeyMapped().mapped_of_value(m_ptr->get_data());
   }

   //! <b>Requires</b>: empty() == false.
   //!
   //! <b>Returns</b>: A copy of the internally hold allocator.
   //!
   //! <b>Throws</b>: Nothing.
   allocator_type get_allocator() const
   {
      AUTOBOOST_ASSERT(!empty());
      return this->node_alloc();
   }

   //! <b>Returns</b>: m_ptr != nullptr.
   //!
   #ifdef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   AUTOBOOST_CONTAINER_FORCEINLINE explicit operator bool
   #else
   private: struct bool_conversion {int for_bool; int for_arg(); }; typedef int bool_conversion::* explicit_bool_arg;
   public: AUTOBOOST_CONTAINER_FORCEINLINE operator explicit_bool_arg
   #endif
      ()const AUTOBOOST_NOEXCEPT
   {  return m_ptr ? &bool_conversion::for_bool  : explicit_bool_arg(0);  }

   //! <b>Returns</b>: m_ptr == nullptr.
   //!
   bool empty() const AUTOBOOST_NOEXCEPT
   {
      return !this->m_ptr;
   }

   //! <b>Requires</b>: this->empty(), or nh.empty(), or ator_traits::propagate_on_container_swap is true, or
   //!   node_alloc() == nh.node_alloc().
   //!
   //! <b>Effects</b>: Calls swap(m_ptr, nh.m_ptr). If this->empty(), or nh.empty(), or ator_traits::propagate_on_-
   //!   container_swap is true calls swap(node_alloc(), nh.node_alloc()).
   void swap(node_handle &nh)
      AUTOBOOST_NOEXCEPT_IF(ator_traits::propagate_on_container_swap::value || ator_traits::is_always_equal::value)
   {
      AUTOBOOST_ASSERT(this->empty() || nh.empty() || ator_traits::propagate_on_container_swap::value
                   || ator_traits::equal(node_alloc(), nh.node_alloc()));

      bool const was_this_non_null = !this->empty();
      bool const was_nh_non_null   = !nh.empty();

      if(was_nh_non_null){
         if(was_this_non_null){
            if(ator_traits::propagate_on_container_swap::value){
               ::autoboost::adl_move_swap(this->node_alloc(), nh.node_alloc());
            }
         }
         else{
            this->move_construct_alloc(nh.node_alloc());
            nh.destroy_alloc();
         }
      }
      else if(was_this_non_null){
         nh.move_construct_alloc(this->node_alloc());
         nh.destroy_alloc();
      }
      ::autoboost::adl_move_swap(m_ptr, nh.m_ptr);
   }

   //! <b>Effects</b>: x.swap(y).
   //!
   friend void swap(node_handle & x, node_handle & y) AUTOBOOST_NOEXCEPT_IF(AUTOBOOST_NOEXCEPT(x.swap(y)))
   {  x.swap(y);  }
};

//! A class template used to describe the results of inserting a
//! Container::node_type in a Container with unique keys.
//! Includes at least the following non-static public data members:
//!
//! <ul><li>bool inserted</li>;
//! <li>Iterator position</li>;
//! <li>NodeType node</li></ul>
//!
//! This type is MoveConstructible, MoveAssignable, DefaultConstructible,
//! Destructible, and lvalues of that type are swappable
template<class Iterator, class NodeType>
struct insert_return_type_base
{
   private:
   AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE(insert_return_type_base)

   public:
   insert_return_type_base()
      : inserted(false), position(), node()
   {}

   insert_return_type_base(AUTOBOOST_RV_REF(insert_return_type_base) other)
      : inserted(other.inserted), position(other.position), node(autoboost::move(other.node))
   {}

   template<class RelatedIt, class RelatedNode>
   insert_return_type_base(bool insert, RelatedIt it, AUTOBOOST_RV_REF(RelatedNode) node)
      : inserted(insert), position(it), node(autoboost::move(node))
   {}

   insert_return_type_base & operator=(AUTOBOOST_RV_REF(insert_return_type_base) other)
   {
      inserted = other.inserted;
      position = other.position;
      node = autoboost::move(other.node);
      return *this;
   }

   bool  inserted;
   Iterator position;
   NodeType node;
};

}  //namespace container {
}  //namespace autoboost {

#include <autoboost/container/detail/config_end.hpp>

#endif   //AUTOBOOST_CONTAINER_NODE_HANDLE_HPP

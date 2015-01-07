/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_INTRUSIVE_AVLTREE_HPP
#define AUTOBOOST_INTRUSIVE_AVLTREE_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/intrusive/detail/config_begin.hpp>
#include <autoboost/intrusive/intrusive_fwd.hpp>
#include <cstddef>
#include <functional>
#include <utility>

#include <autoboost/static_assert.hpp>
#include <autoboost/intrusive/avl_set_hook.hpp>
#include <autoboost/intrusive/detail/avltree_node.hpp>
#include <autoboost/intrusive/bstree.hpp>
#include <autoboost/intrusive/detail/tree_node.hpp>
#include <autoboost/intrusive/detail/ebo_functor_holder.hpp>
#include <autoboost/intrusive/detail/mpl.hpp>
#include <autoboost/intrusive/pointer_traits.hpp>
#include <autoboost/intrusive/detail/get_value_traits.hpp>
#include <autoboost/intrusive/avltree_algorithms.hpp>
#include <autoboost/intrusive/link_mode.hpp>
#include <autoboost/move/utility_core.hpp>

namespace autoboost {
namespace intrusive {

/// @cond

struct default_avltree_hook_applier
{  template <class T> struct apply{ typedef typename T::default_avltree_hook type;  };  };

template<>
struct is_default_hook_tag<default_avltree_hook_applier>
{  static const bool value = true;  };

struct avltree_defaults
{
   typedef default_avltree_hook_applier proto_value_traits;
   static const bool constant_time_size = true;
   typedef std::size_t size_type;
   typedef void compare;
   typedef void header_holder_type;
};

/// @endcond

//! The class template avltree is an intrusive AVL tree container, that
//! is used to construct intrusive avl_set and avl_multiset containers.
//! The no-throw guarantee holds only, if the value_compare object
//! doesn't throw.
//!
//! The template parameter \c T is the type to be managed by the container.
//! The user can specify additional options and if no options are provided
//! default options are used.
//!
//! The container supports the following options:
//! \c base_hook<>/member_hook<>/value_traits<>,
//! \c constant_time_size<>, \c size_type<> and
//! \c compare<>.
#if defined(AUTOBOOST_INTRUSIVE_DOXYGEN_INVOKED)
template<class T, class ...Options>
#else
template<class ValueTraits, class VoidOrKeyComp, class SizeType, bool ConstantTimeSize, typename HeaderHolder>
#endif
class avltree_impl
   /// @cond
   :  public bstree_impl<ValueTraits, VoidOrKeyComp, SizeType, ConstantTimeSize, AvlTreeAlgorithms, HeaderHolder>
   /// @endcond
{
   public:
   typedef ValueTraits value_traits;
   /// @cond
   typedef bstree_impl< ValueTraits, VoidOrKeyComp, SizeType
                      , ConstantTimeSize, AvlTreeAlgorithms
                      , HeaderHolder>                               tree_type;
   typedef tree_type                                                 implementation_defined;
   /// @endcond

   typedef typename implementation_defined::pointer                  pointer;
   typedef typename implementation_defined::const_pointer            const_pointer;
   typedef typename implementation_defined::value_type               value_type;
   typedef typename implementation_defined::key_type                 key_type;
   typedef typename implementation_defined::reference                reference;
   typedef typename implementation_defined::const_reference          const_reference;
   typedef typename implementation_defined::difference_type          difference_type;
   typedef typename implementation_defined::size_type                size_type;
   typedef typename implementation_defined::value_compare            value_compare;
   typedef typename implementation_defined::key_compare              key_compare;
   typedef typename implementation_defined::iterator                 iterator;
   typedef typename implementation_defined::const_iterator           const_iterator;
   typedef typename implementation_defined::reverse_iterator         reverse_iterator;
   typedef typename implementation_defined::const_reverse_iterator   const_reverse_iterator;
   typedef typename implementation_defined::node_traits              node_traits;
   typedef typename implementation_defined::node                     node;
   typedef typename implementation_defined::node_ptr                 node_ptr;
   typedef typename implementation_defined::const_node_ptr           const_node_ptr;
   typedef typename implementation_defined::node_algorithms          node_algorithms;

   static const bool constant_time_size = implementation_defined::constant_time_size;
   /// @cond
   private:

   //noncopyable
   AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE(avltree_impl)

   /// @endcond

   public:

   typedef typename implementation_defined::insert_commit_data insert_commit_data;


   //! @copydoc ::autoboost::intrusive::bstree::bstree(const value_compare &,const value_traits &)
   explicit avltree_impl( const value_compare &cmp = value_compare()
                       , const value_traits &v_traits = value_traits())
      :  tree_type(cmp, v_traits)
   {}

   //! @copydoc ::autoboost::intrusive::bstree::bstree(bool,Iterator,Iterator,const value_compare &,const value_traits &)
   template<class Iterator>
   avltree_impl( bool unique, Iterator b, Iterator e
              , const value_compare &cmp     = value_compare()
              , const value_traits &v_traits = value_traits())
      : tree_type(unique, b, e, cmp, v_traits)
   {}

   //! @copydoc ::autoboost::intrusive::bstree::bstree(bstree &&)
   avltree_impl(AUTOBOOST_RV_REF(avltree_impl) x)
      :  tree_type(::autoboost::move(static_cast<tree_type&>(x)))
   {}

   //! @copydoc ::autoboost::intrusive::bstree::operator=(bstree &&)
   avltree_impl& operator=(AUTOBOOST_RV_REF(avltree_impl) x)
   {  return static_cast<avltree_impl&>(tree_type::operator=(::autoboost::move(static_cast<tree_type&>(x)))); }

   #ifdef AUTOBOOST_INTRUSIVE_DOXYGEN_INVOKED

   //! @copydoc ::autoboost::intrusive::bstree::~bstree()
   ~avltree_impl();

   //! @copydoc ::autoboost::intrusive::bstree::begin()
   iterator begin();

   //! @copydoc ::autoboost::intrusive::bstree::begin()const
   const_iterator begin() const;

   //! @copydoc ::autoboost::intrusive::bstree::cbegin()const
   const_iterator cbegin() const;

   //! @copydoc ::autoboost::intrusive::bstree::end()
   iterator end();

   //! @copydoc ::autoboost::intrusive::bstree::end()const
   const_iterator end() const;

   //! @copydoc ::autoboost::intrusive::bstree::cend()const
   const_iterator cend() const;

   //! @copydoc ::autoboost::intrusive::bstree::rbegin()
   reverse_iterator rbegin();

   //! @copydoc ::autoboost::intrusive::bstree::rbegin()const
   const_reverse_iterator rbegin() const;

   //! @copydoc ::autoboost::intrusive::bstree::crbegin()const
   const_reverse_iterator crbegin() const;

   //! @copydoc ::autoboost::intrusive::bstree::rend()
   reverse_iterator rend();

   //! @copydoc ::autoboost::intrusive::bstree::rend()const
   const_reverse_iterator rend() const;

   //! @copydoc ::autoboost::intrusive::bstree::crend()const
   const_reverse_iterator crend() const;

   //! @copydoc ::autoboost::intrusive::bstree::container_from_end_iterator(iterator)
   static avltree_impl &container_from_end_iterator(iterator end_iterator);

   //! @copydoc ::autoboost::intrusive::bstree::container_from_end_iterator(const_iterator)
   static const avltree_impl &container_from_end_iterator(const_iterator end_iterator);

   //! @copydoc ::autoboost::intrusive::bstree::container_from_iterator(iterator)
   static avltree_impl &container_from_iterator(iterator it);

   //! @copydoc ::autoboost::intrusive::bstree::container_from_iterator(const_iterator)
   static const avltree_impl &container_from_iterator(const_iterator it);

   //! @copydoc ::autoboost::intrusive::bstree::key_comp()const
   key_compare key_comp() const;

   //! @copydoc ::autoboost::intrusive::bstree::value_comp()const
   value_compare value_comp() const;

   //! @copydoc ::autoboost::intrusive::bstree::empty()const
   bool empty() const;

   //! @copydoc ::autoboost::intrusive::bstree::size()const
   size_type size() const;

   //! @copydoc ::autoboost::intrusive::bstree::swap
   void swap(avltree_impl& other);

   //! @copydoc ::autoboost::intrusive::bstree::clone_from
   template <class Cloner, class Disposer>
   void clone_from(const avltree_impl &src, Cloner cloner, Disposer disposer);

   //! @copydoc ::autoboost::intrusive::bstree::insert_equal(reference)
   iterator insert_equal(reference value);

   //! @copydoc ::autoboost::intrusive::bstree::insert_equal(const_iterator,reference)
   iterator insert_equal(const_iterator hint, reference value);

   //! @copydoc ::autoboost::intrusive::bstree::insert_equal(Iterator,Iterator)
   template<class Iterator>
   void insert_equal(Iterator b, Iterator e);

   //! @copydoc ::autoboost::intrusive::bstree::insert_unique(reference)
   std::pair<iterator, bool> insert_unique(reference value);

   //! @copydoc ::autoboost::intrusive::bstree::insert_unique(const_iterator,reference)
   iterator insert_unique(const_iterator hint, reference value);

   //! @copydoc ::autoboost::intrusive::bstree::insert_unique_check(const KeyType&,KeyValueCompare,insert_commit_data&)
   template<class KeyType, class KeyValueCompare>
   std::pair<iterator, bool> insert_unique_check
      (const KeyType &key, KeyValueCompare key_value_comp, insert_commit_data &commit_data);

   //! @copydoc ::autoboost::intrusive::bstree::insert_unique_check(const_iterator,const KeyType&,KeyValueCompare,insert_commit_data&)
   template<class KeyType, class KeyValueCompare>
   std::pair<iterator, bool> insert_unique_check
      (const_iterator hint, const KeyType &key
      ,KeyValueCompare key_value_comp, insert_commit_data &commit_data);

   //! @copydoc ::autoboost::intrusive::bstree::insert_unique_commit
   iterator insert_unique_commit(reference value, const insert_commit_data &commit_data);

   //! @copydoc ::autoboost::intrusive::bstree::insert_unique(Iterator,Iterator)
   template<class Iterator>
   void insert_unique(Iterator b, Iterator e);

   //! @copydoc ::autoboost::intrusive::bstree::insert_before
   iterator insert_before(const_iterator pos, reference value);

   //! @copydoc ::autoboost::intrusive::bstree::push_back
   void push_back(reference value);

   //! @copydoc ::autoboost::intrusive::bstree::push_front
   void push_front(reference value);

   //! @copydoc ::autoboost::intrusive::bstree::erase(const_iterator)
   iterator erase(const_iterator i);

   //! @copydoc ::autoboost::intrusive::bstree::erase(const_iterator,const_iterator)
   iterator erase(const_iterator b, const_iterator e);

   //! @copydoc ::autoboost::intrusive::bstree::erase(const_reference)
   size_type erase(const_reference value);

   //! @copydoc ::autoboost::intrusive::bstree::erase(const KeyType&,KeyValueCompare)
   template<class KeyType, class KeyValueCompare>
   size_type erase(const KeyType& key, KeyValueCompare comp);

   //! @copydoc ::autoboost::intrusive::bstree::erase_and_dispose(const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator i, Disposer disposer);

   //! @copydoc ::autoboost::intrusive::bstree::erase_and_dispose(const_iterator,const_iterator,Disposer)
   template<class Disposer>
   iterator erase_and_dispose(const_iterator b, const_iterator e, Disposer disposer);

   //! @copydoc ::autoboost::intrusive::bstree::erase_and_dispose(const_reference, Disposer)
   template<class Disposer>
   size_type erase_and_dispose(const_reference value, Disposer disposer);

   //! @copydoc ::autoboost::intrusive::bstree::erase_and_dispose(const KeyType&,KeyValueCompare,Disposer)
   template<class KeyType, class KeyValueCompare, class Disposer>
   size_type erase_and_dispose(const KeyType& key, KeyValueCompare comp, Disposer disposer);

   //! @copydoc ::autoboost::intrusive::bstree::clear
   void clear();

   //! @copydoc ::autoboost::intrusive::bstree::clear_and_dispose
   template<class Disposer>
   void clear_and_dispose(Disposer disposer);

   //! @copydoc ::autoboost::intrusive::bstree::count(const_reference)const
   size_type count(const_reference value) const;

   //! @copydoc ::autoboost::intrusive::bstree::count(const KeyType&,KeyValueCompare)const
   template<class KeyType, class KeyValueCompare>
   size_type count(const KeyType& key, KeyValueCompare comp) const;

   //! @copydoc ::autoboost::intrusive::bstree::lower_bound(const_reference)
   iterator lower_bound(const_reference value);
   
   //! @copydoc ::autoboost::intrusive::bstree::lower_bound(const KeyType&,KeyValueCompare)
   template<class KeyType, class KeyValueCompare>
   iterator lower_bound(const KeyType& key, KeyValueCompare comp);

   //! @copydoc ::autoboost::intrusive::bstree::lower_bound(const_reference)const
   const_iterator lower_bound(const_reference value) const;

   //! @copydoc ::autoboost::intrusive::bstree::lower_bound(const KeyType&,KeyValueCompare)const
   template<class KeyType, class KeyValueCompare>
   const_iterator lower_bound(const KeyType& key, KeyValueCompare comp) const;

   //! @copydoc ::autoboost::intrusive::bstree::upper_bound(const_reference)
   iterator upper_bound(const_reference value);

   //! @copydoc ::autoboost::intrusive::bstree::upper_bound(const KeyType&,KeyValueCompare)
   template<class KeyType, class KeyValueCompare>
   iterator upper_bound(const KeyType& key, KeyValueCompare comp);

   //! @copydoc ::autoboost::intrusive::bstree::upper_bound(const_reference)const
   const_iterator upper_bound(const_reference value) const;

   //! @copydoc ::autoboost::intrusive::bstree::upper_bound(const KeyType&,KeyValueCompare)const
   template<class KeyType, class KeyValueCompare>
   const_iterator upper_bound(const KeyType& key, KeyValueCompare comp) const;

   //! @copydoc ::autoboost::intrusive::bstree::find(const_reference)
   iterator find(const_reference value);

   //! @copydoc ::autoboost::intrusive::bstree::find(const KeyType&,KeyValueCompare)
   template<class KeyType, class KeyValueCompare>
   iterator find(const KeyType& key, KeyValueCompare comp);

   //! @copydoc ::autoboost::intrusive::bstree::find(const_reference)const
   const_iterator find(const_reference value) const;

   //! @copydoc ::autoboost::intrusive::bstree::find(const KeyType&,KeyValueCompare)const
   template<class KeyType, class KeyValueCompare>
   const_iterator find(const KeyType& key, KeyValueCompare comp) const;

   //! @copydoc ::autoboost::intrusive::bstree::equal_range(const_reference)
   std::pair<iterator,iterator> equal_range(const_reference value);

   //! @copydoc ::autoboost::intrusive::bstree::equal_range(const KeyType&,KeyValueCompare)
   template<class KeyType, class KeyValueCompare>
   std::pair<iterator,iterator> equal_range(const KeyType& key, KeyValueCompare comp);

   //! @copydoc ::autoboost::intrusive::bstree::equal_range(const_reference)const
   std::pair<const_iterator, const_iterator>
      equal_range(const_reference value) const;

   //! @copydoc ::autoboost::intrusive::bstree::equal_range(const KeyType&,KeyValueCompare)const
   template<class KeyType, class KeyValueCompare>
   std::pair<const_iterator, const_iterator>
      equal_range(const KeyType& key, KeyValueCompare comp) const;

   //! @copydoc ::autoboost::intrusive::bstree::bounded_range(const_reference,const_reference,bool,bool)
   std::pair<iterator,iterator> bounded_range
      (const_reference lower_value, const_reference upper_value, bool left_closed, bool right_closed);

   //! @copydoc ::autoboost::intrusive::bstree::bounded_range(const KeyType&,const KeyType&,KeyValueCompare,bool,bool)
   template<class KeyType, class KeyValueCompare>
   std::pair<iterator,iterator> bounded_range
      (const KeyType& lower_key, const KeyType& upper_key, KeyValueCompare comp, bool left_closed, bool right_closed);

   //! @copydoc ::autoboost::intrusive::bstree::bounded_range(const_reference,const_reference,bool,bool)const
   std::pair<const_iterator, const_iterator>
      bounded_range(const_reference lower_value, const_reference upper_value, bool left_closed, bool right_closed) const;

   //! @copydoc ::autoboost::intrusive::bstree::bounded_range(const KeyType&,const KeyType&,KeyValueCompare,bool,bool)const
   template<class KeyType, class KeyValueCompare>
   std::pair<const_iterator, const_iterator> bounded_range
         (const KeyType& lower_key, const KeyType& upper_key, KeyValueCompare comp, bool left_closed, bool right_closed) const;

   //! @copydoc ::autoboost::intrusive::bstree::s_iterator_to(reference)
   static iterator s_iterator_to(reference value);

   //! @copydoc ::autoboost::intrusive::bstree::s_iterator_to(const_reference)
   static const_iterator s_iterator_to(const_reference value);

   //! @copydoc ::autoboost::intrusive::bstree::iterator_to(reference)
   iterator iterator_to(reference value);

   //! @copydoc ::autoboost::intrusive::bstree::iterator_to(const_reference)const
   const_iterator iterator_to(const_reference value) const;

   //! @copydoc ::autoboost::intrusive::bstree::init_node(reference)
   static void init_node(reference value);

   //! @copydoc ::autoboost::intrusive::bstree::unlink_leftmost_without_rebalance
   pointer unlink_leftmost_without_rebalance();

   //! @copydoc ::autoboost::intrusive::bstree::replace_node
   void replace_node(iterator replace_this, reference with_this);

   //! @copydoc ::autoboost::intrusive::bstree::remove_node
   void remove_node(reference value);
   #endif   //#ifdef AUTOBOOST_INTRUSIVE_DOXYGEN_INVOKED
};

#if defined(AUTOBOOST_INTRUSIVE_DOXYGEN_INVOKED)

template<class T, class ...Options>
bool operator< (const avltree_impl<T, Options...> &x, const avltree_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator==(const avltree_impl<T, Options...> &x, const avltree_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator!= (const avltree_impl<T, Options...> &x, const avltree_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator>(const avltree_impl<T, Options...> &x, const avltree_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator<=(const avltree_impl<T, Options...> &x, const avltree_impl<T, Options...> &y);

template<class T, class ...Options>
bool operator>=(const avltree_impl<T, Options...> &x, const avltree_impl<T, Options...> &y);

template<class T, class ...Options>
void swap(avltree_impl<T, Options...> &x, avltree_impl<T, Options...> &y);

#endif   //#if defined(AUTOBOOST_INTRUSIVE_DOXYGEN_INVOKED)

//! Helper metafunction to define a \c avltree that yields to the same type when the
//! same options (either explicitly or implicitly) are used.
#if defined(AUTOBOOST_INTRUSIVE_DOXYGEN_INVOKED) || defined(AUTOBOOST_INTRUSIVE_VARIADIC_TEMPLATES)
template<class T, class ...Options>
#else
template<class T, class O1 = void, class O2 = void
                , class O3 = void, class O4 = void
                , class O5 = void>
#endif
struct make_avltree
{
   /// @cond
   typedef typename pack_options
      < avltree_defaults,
      #if !defined(AUTOBOOST_INTRUSIVE_VARIADIC_TEMPLATES)
      O1, O2, O3, O4, O5
      #else
      Options...
      #endif
      >::type packed_options;

   typedef typename detail::get_value_traits
      <T, typename packed_options::proto_value_traits>::type value_traits;
   typedef typename detail::get_header_holder_type
      < value_traits, typename packed_options::header_holder_type >::type header_holder_type;

   typedef avltree_impl
         < value_traits
         , typename packed_options::compare
         , typename packed_options::size_type
         , packed_options::constant_time_size
         , header_holder_type
         > implementation_defined;
   /// @endcond
   typedef implementation_defined type;
};


#ifndef AUTOBOOST_INTRUSIVE_DOXYGEN_INVOKED

#if !defined(AUTOBOOST_INTRUSIVE_VARIADIC_TEMPLATES)
template<class T, class O1, class O2, class O3, class O4, class O5>
#else
template<class T, class ...Options>
#endif
class avltree
   :  public make_avltree<T,
      #if !defined(AUTOBOOST_INTRUSIVE_VARIADIC_TEMPLATES)
      O1, O2, O3, O4, O5
      #else
      Options...
      #endif
      >::type
{
   typedef typename make_avltree
      <T,
      #if !defined(AUTOBOOST_INTRUSIVE_VARIADIC_TEMPLATES)
      O1, O2, O3, O4, O5
      #else
      Options...
      #endif
      >::type   Base;
   AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE(avltree)

   public:
   typedef typename Base::value_compare      value_compare;
   typedef typename Base::value_traits       value_traits;
   typedef typename Base::iterator           iterator;
   typedef typename Base::const_iterator     const_iterator;
   typedef typename Base::reverse_iterator           reverse_iterator;
   typedef typename Base::const_reverse_iterator     const_reverse_iterator;

   //Assert if passed value traits are compatible with the type
   AUTOBOOST_STATIC_ASSERT((detail::is_same<typename value_traits::value_type, T>::value));

   explicit avltree( const value_compare &cmp = value_compare()
                   , const value_traits &v_traits = value_traits())
      :  Base(cmp, v_traits)
   {}

   template<class Iterator>
   avltree( bool unique, Iterator b, Iterator e
         , const value_compare &cmp = value_compare()
         , const value_traits &v_traits = value_traits())
      :  Base(unique, b, e, cmp, v_traits)
   {}

   avltree(AUTOBOOST_RV_REF(avltree) x)
      :  Base(::autoboost::move(static_cast<Base&>(x)))
   {}

   avltree& operator=(AUTOBOOST_RV_REF(avltree) x)
   {  return static_cast<avltree &>(this->Base::operator=(::autoboost::move(static_cast<Base&>(x))));  }

   static avltree &container_from_end_iterator(iterator end_iterator)
   {  return static_cast<avltree &>(Base::container_from_end_iterator(end_iterator));   }

   static const avltree &container_from_end_iterator(const_iterator end_iterator)
   {  return static_cast<const avltree &>(Base::container_from_end_iterator(end_iterator));   }

   static avltree &container_from_iterator(iterator it)
   {  return static_cast<avltree &>(Base::container_from_iterator(it));   }

   static const avltree &container_from_iterator(const_iterator it)
   {  return static_cast<const avltree &>(Base::container_from_iterator(it));   }
};

#endif

} //namespace intrusive
} //namespace autoboost

#include <autoboost/intrusive/detail/config_end.hpp>

#endif //AUTOBOOST_INTRUSIVE_AVLTREE_HPP

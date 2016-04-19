//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_CONTAINER_FLAT_MAP_HPP
#define AUTOBOOST_CONTAINER_FLAT_MAP_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>
// container
#include <autoboost/container/allocator_traits.hpp>
#include <autoboost/container/container_fwd.hpp>
#include <autoboost/container/new_allocator.hpp> //new_allocator
#include <autoboost/container/throw_exception.hpp>
// container/detail
#include <autoboost/container/detail/flat_tree.hpp>
#include <autoboost/container/detail/type_traits.hpp>
#include <autoboost/container/detail/mpl.hpp>
#include <autoboost/container/detail/algorithm.hpp> //equal()
// move
#include <autoboost/move/utility_core.hpp>
#include <autoboost/move/traits.hpp>
// move/detail
#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <autoboost/move/detail/fwd_macros.hpp>
#endif
#include <autoboost/move/detail/move_helpers.hpp>
// intrusive
#include <autoboost/intrusive/detail/minimal_pair_header.hpp>      //pair
#include <autoboost/intrusive/detail/minimal_less_equal_header.hpp>//less, equal
//others
#include <autoboost/core/no_exceptions_support.hpp>

#if !defined(AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST)
#include <initializer_list>
#endif

namespace autoboost {
namespace container {

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

namespace container_detail{

template<class D, class S>
static D &force(const S &s)
{  return *const_cast<D*>((reinterpret_cast<const D*>(&s))); }

template<class D, class S>
static D force_copy(S s)
{
   D *vp = reinterpret_cast<D *>(&s);
   return D(*vp);
}

}  //namespace container_detail{

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

//! A flat_map is a kind of associative container that supports unique keys (contains at
//! most one of each key value) and provides for fast retrieval of values of another
//! type T based on the keys. The flat_map class supports random-access iterators.
//!
//! A flat_map satisfies all of the requirements of a container and of a reversible
//! container and of an associative container. A flat_map also provides
//! most operations described for unique keys. For a
//! flat_map<Key,T> the key_type is Key and the value_type is std::pair<Key,T>
//! (unlike std::map<Key, T> which value_type is std::pair<<b>const</b> Key, T>).
//!
//! Compare is the ordering function for Keys (e.g. <i>std::less<Key></i>).
//!
//! Allocator is the allocator to allocate the value_types
//! (e.g. <i>allocator< std::pair<Key, T> ></i>).
//!
//! flat_map is similar to std::map but it's implemented like an ordered vector.
//! This means that inserting a new element into a flat_map invalidates
//! previous iterators and references
//!
//! Erasing an element invalidates iterators and references
//! pointing to elements that come after (their keys are bigger) the erased element.
//!
//! This container provides random-access iterators.
//!
//! \tparam Key is the key_type of the map
//! \tparam Value is the <code>mapped_type</code>
//! \tparam Compare is the ordering function for Keys (e.g. <i>std::less<Key></i>).
//! \tparam Allocator is the allocator to allocate the <code>value_type</code>s
//!   (e.g. <i>allocator< std::pair<Key, T> > </i>).
#ifdef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
template <class Key, class T, class Compare = std::less<Key>, class Allocator = new_allocator< std::pair< Key, T> > >
#else
template <class Key, class T, class Compare, class Allocator>
#endif
class flat_map
{
   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   private:
   AUTOBOOST_COPYABLE_AND_MOVABLE(flat_map)
   //This is the tree that we should store if pair was movable
   typedef container_detail::flat_tree<Key,
                           std::pair<Key, T>,
                           container_detail::select1st< std::pair<Key, T> >,
                           Compare,
                           Allocator> tree_t;

   //This is the real tree stored here. It's based on a movable pair
   typedef container_detail::flat_tree<Key,
                           container_detail::pair<Key, T>,
                           container_detail::select1st<container_detail::pair<Key, T> >,
                           Compare,
                           typename allocator_traits<Allocator>::template portable_rebind_alloc
                              <container_detail::pair<Key, T> >::type> impl_tree_t;
   impl_tree_t m_flat_tree;  // flat tree representing flat_map

   typedef typename impl_tree_t::value_type              impl_value_type;
   typedef typename impl_tree_t::const_iterator          impl_const_iterator;
   typedef typename impl_tree_t::iterator                impl_iterator;
   typedef typename impl_tree_t::allocator_type          impl_allocator_type;
   typedef container_detail::flat_tree_value_compare
      < Compare
      , container_detail::select1st< std::pair<Key, T> >
      , std::pair<Key, T> >                                                         value_compare_impl;
   typedef typename container_detail::get_flat_tree_iterators
         <typename allocator_traits<Allocator>::pointer>::iterator                  iterator_impl;
   typedef typename container_detail::get_flat_tree_iterators
      <typename allocator_traits<Allocator>::pointer>::const_iterator               const_iterator_impl;
   typedef typename container_detail::get_flat_tree_iterators
         <typename allocator_traits<Allocator>::pointer>::reverse_iterator          reverse_iterator_impl;
   typedef typename container_detail::get_flat_tree_iterators
         <typename allocator_traits<Allocator>::pointer>::const_reverse_iterator    const_reverse_iterator_impl;
   public:
   typedef typename impl_tree_t::stored_allocator_type   impl_stored_allocator_type;
   private:
   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

   public:

   //////////////////////////////////////////////
   //
   //                    types
   //
   //////////////////////////////////////////////
   typedef Key                                                                      key_type;
   typedef T                                                                        mapped_type;
   typedef std::pair<Key, T>                                                        value_type;
   typedef ::autoboost::container::allocator_traits<Allocator>                          allocator_traits_type;
   typedef typename autoboost::container::allocator_traits<Allocator>::pointer          pointer;
   typedef typename autoboost::container::allocator_traits<Allocator>::const_pointer    const_pointer;
   typedef typename autoboost::container::allocator_traits<Allocator>::reference        reference;
   typedef typename autoboost::container::allocator_traits<Allocator>::const_reference  const_reference;
   typedef typename autoboost::container::allocator_traits<Allocator>::size_type        size_type;
   typedef typename autoboost::container::allocator_traits<Allocator>::difference_type  difference_type;
   typedef Allocator                                                                allocator_type;
   typedef AUTOBOOST_CONTAINER_IMPDEF(Allocator)                                        stored_allocator_type;
   typedef AUTOBOOST_CONTAINER_IMPDEF(value_compare_impl)                               value_compare;
   typedef Compare                                                                  key_compare;
   typedef AUTOBOOST_CONTAINER_IMPDEF(iterator_impl)                                    iterator;
   typedef AUTOBOOST_CONTAINER_IMPDEF(const_iterator_impl)                              const_iterator;
   typedef AUTOBOOST_CONTAINER_IMPDEF(reverse_iterator_impl)                            reverse_iterator;
   typedef AUTOBOOST_CONTAINER_IMPDEF(const_reverse_iterator_impl)                      const_reverse_iterator;
   typedef AUTOBOOST_CONTAINER_IMPDEF(impl_value_type)                                  movable_value_type;

   public:
   //////////////////////////////////////////////
   //
   //          construct/copy/destroy
   //
   //////////////////////////////////////////////

   //! <b>Effects</b>: Default constructs an empty flat_map.
   //!
   //! <b>Complexity</b>: Constant.
   flat_map()
      : m_flat_tree()
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_map using the specified
   //! comparison object and allocator.
   //!
   //! <b>Complexity</b>: Constant.
   explicit flat_map(const Compare& comp, const allocator_type& a = allocator_type())
      : m_flat_tree(comp, container_detail::force<impl_allocator_type>(a))
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_map using the specified allocator.
   //!
   //! <b>Complexity</b>: Constant.
   explicit flat_map(const allocator_type& a)
      : m_flat_tree(container_detail::force<impl_allocator_type>(a))
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_map using the specified comparison object and
   //! allocator, and inserts elements from the range [first ,last ).
   //!
   //! <b>Complexity</b>: Linear in N if the range [first ,last ) is already sorted using
   //! comp and otherwise N logN, where N is last - first.
   template <class InputIterator>
   flat_map(InputIterator first, InputIterator last, const Compare& comp = Compare(),
         const allocator_type& a = allocator_type())
      : m_flat_tree(true, first, last, comp, container_detail::force<impl_allocator_type>(a))
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_map using the specified
   //! allocator, and inserts elements from the range [first ,last ).
   //!
   //! <b>Complexity</b>: Linear in N if the range [first ,last ) is already sorted using
   //! comp and otherwise N logN, where N is last - first.
   template <class InputIterator>
   flat_map(InputIterator first, InputIterator last, const allocator_type& a)
      : m_flat_tree(true, first, last, Compare(), container_detail::force<impl_allocator_type>(a))
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_map using the specified comparison object and
   //! allocator, and inserts elements from the ordered unique range [first ,last). This function
   //! is more efficient than the normal range creation for ordered ranges.
   //!
   //! <b>Requires</b>: [first ,last) must be ordered according to the predicate and must be
   //! unique values.
   //!
   //! <b>Complexity</b>: Linear in N.
   //!
   //! <b>Note</b>: Non-standard extension.
   template <class InputIterator>
   flat_map( ordered_unique_range_t, InputIterator first, InputIterator last
           , const Compare& comp = Compare(), const allocator_type& a = allocator_type())
      : m_flat_tree(ordered_range, first, last, comp, a)
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

#if !defined(AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST)
   //! <b>Effects</b>: Constructs an empty flat_map using the specified comparison object and
   //! allocator, and inserts elements from the range [il.begin() ,il.end()).
   //!
   //! <b>Complexity</b>: Linear in N if the range [il.begin(), il.end()) is already sorted using
   //! comp and otherwise N logN, where N is last - first.
   flat_map(std::initializer_list<value_type> il, const Compare& comp = Compare(),
          const allocator_type& a = allocator_type())
     : m_flat_tree(true, il.begin(), il.end(), comp, container_detail::force<impl_allocator_type>(a))
   {
       //A type must be std::pair<Key, T>
       AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_map using the specified
   //! allocator, and inserts elements from the range [il.begin() ,il.end()).
   //!
   //! <b>Complexity</b>: Linear in N if the range [il.begin(), il.end()) is already sorted using
   //! comp and otherwise N logN, where N is last - first.
   flat_map(std::initializer_list<value_type> il, const allocator_type& a)
     : m_flat_tree(true, il.begin(), il.end(), Compare(), container_detail::force<impl_allocator_type>(a))
   {
       //A type must be std::pair<Key, T>
       AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_map using the specified comparison object and
   //! allocator, and inserts elements from the ordered unique range [il.begin(), il.end()). This function
   //! is more efficient than the normal range creation for ordered ranges.
   //!
   //! <b>Requires</b>: [il.begin(), il.end()) must be ordered according to the predicate and must be
   //! unique values.
   //!
   //! <b>Complexity</b>: Linear in N.
   //!
   //! <b>Note</b>: Non-standard extension.
   flat_map(ordered_unique_range_t, std::initializer_list<value_type> il, const Compare& comp = Compare(),
          const allocator_type& a = allocator_type())
     : m_flat_tree(ordered_range, il.begin(), il.end(), comp, a)
   {
       //A type must be std::pair<Key, T>
       AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }
#endif

   //! <b>Effects</b>: Copy constructs a flat_map.
   //!
   //! <b>Complexity</b>: Linear in x.size().
   flat_map(const flat_map& x)
      : m_flat_tree(x.m_flat_tree)
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Move constructs a flat_map.
   //!   Constructs *this using x's resources.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Postcondition</b>: x is emptied.
   flat_map(AUTOBOOST_RV_REF(flat_map) x)
      : m_flat_tree(autoboost::move(x.m_flat_tree))
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Copy constructs a flat_map using the specified allocator.
   //!
   //! <b>Complexity</b>: Linear in x.size().
   flat_map(const flat_map& x, const allocator_type &a)
      : m_flat_tree(x.m_flat_tree, a)
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Move constructs a flat_map using the specified allocator.
   //!   Constructs *this using x's resources.
   //!
   //! <b>Complexity</b>: Constant if x.get_allocator() == a, linear otherwise.
   flat_map(AUTOBOOST_RV_REF(flat_map) x, const allocator_type &a)
      : m_flat_tree(autoboost::move(x.m_flat_tree), a)
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Makes *this a copy of x.
   //!
   //! <b>Complexity</b>: Linear in x.size().
   flat_map& operator=(AUTOBOOST_COPY_ASSIGN_REF(flat_map) x)
   {  m_flat_tree = x.m_flat_tree;   return *this;  }

   //! <b>Effects</b>: Move constructs a flat_map.
   //!   Constructs *this using x's resources.
   //!
   //! <b>Throws</b>: If allocator_traits_type::propagate_on_container_move_assignment
   //!   is false and (allocation throws or value_type's move constructor throws)
   //!
   //! <b>Complexity</b>: Constant if allocator_traits_type::
   //!   propagate_on_container_move_assignment is true or
   //!   this->get>allocator() == x.get_allocator(). Linear otherwise.
   flat_map& operator=(AUTOBOOST_RV_REF(flat_map) x)
      AUTOBOOST_NOEXCEPT_IF(  allocator_traits_type::is_always_equal::value
                                 && autoboost::container::container_detail::is_nothrow_move_assignable<Compare>::value )
   {  m_flat_tree = autoboost::move(x.m_flat_tree);   return *this;  }

#if !defined(AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST)
   //! <b>Effects</b>: Assign elements from il to *this
   flat_map& operator=(std::initializer_list<value_type> il)
   {
      this->clear();
      this->insert(il.begin(), il.end());
      return *this;
   }
#endif

   //! <b>Effects</b>: Returns a copy of the allocator that
   //!   was passed to the object's constructor.
   //!
   //! <b>Complexity</b>: Constant.
   allocator_type get_allocator() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<allocator_type>(m_flat_tree.get_allocator()); }

   //! <b>Effects</b>: Returns a reference to the internal allocator.
   //!
   //! <b>Throws</b>: Nothing
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Note</b>: Non-standard extension.
   stored_allocator_type &get_stored_allocator() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force<stored_allocator_type>(m_flat_tree.get_stored_allocator()); }

   //! <b>Effects</b>: Returns a reference to the internal allocator.
   //!
   //! <b>Throws</b>: Nothing
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Note</b>: Non-standard extension.
   const stored_allocator_type &get_stored_allocator() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force<stored_allocator_type>(m_flat_tree.get_stored_allocator()); }

   //////////////////////////////////////////////
   //
   //                iterators
   //
   //////////////////////////////////////////////

   //! <b>Effects</b>: Returns an iterator to the first element contained in the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   iterator begin() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<iterator>(m_flat_tree.begin()); }

   //! <b>Effects</b>: Returns a const_iterator to the first element contained in the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_iterator begin() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_iterator>(m_flat_tree.begin()); }

   //! <b>Effects</b>: Returns an iterator to the end of the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   iterator end() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<iterator>(m_flat_tree.end()); }

   //! <b>Effects</b>: Returns a const_iterator to the end of the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_iterator end() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_iterator>(m_flat_tree.end()); }

   //! <b>Effects</b>: Returns a reverse_iterator pointing to the beginning
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   reverse_iterator rbegin() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<reverse_iterator>(m_flat_tree.rbegin()); }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the beginning
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator rbegin() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_reverse_iterator>(m_flat_tree.rbegin()); }

   //! <b>Effects</b>: Returns a reverse_iterator pointing to the end
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   reverse_iterator rend() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<reverse_iterator>(m_flat_tree.rend()); }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the end
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator rend() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_reverse_iterator>(m_flat_tree.rend()); }

   //! <b>Effects</b>: Returns a const_iterator to the first element contained in the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_iterator cbegin() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_iterator>(m_flat_tree.cbegin()); }

   //! <b>Effects</b>: Returns a const_iterator to the end of the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_iterator cend() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_iterator>(m_flat_tree.cend()); }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the beginning
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator crbegin() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_reverse_iterator>(m_flat_tree.crbegin()); }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the end
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator crend() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_reverse_iterator>(m_flat_tree.crend()); }

   //////////////////////////////////////////////
   //
   //                capacity
   //
   //////////////////////////////////////////////

   //! <b>Effects</b>: Returns true if the container contains no elements.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   bool empty() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return m_flat_tree.empty(); }

   //! <b>Effects</b>: Returns the number of the elements contained in the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   size_type size() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return m_flat_tree.size(); }

   //! <b>Effects</b>: Returns the largest possible size of the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   size_type max_size() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return m_flat_tree.max_size(); }

   //! <b>Effects</b>: Number of elements for which memory has been allocated.
   //!   capacity() is always greater than or equal to size().
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   size_type capacity() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return m_flat_tree.capacity(); }

   //! <b>Effects</b>: If n is less than or equal to capacity(), this call has no
   //!   effect. Otherwise, it is a request for allocation of additional memory.
   //!   If the request is successful, then capacity() is greater than or equal to
   //!   n; otherwise, capacity() is unchanged. In either case, size() is unchanged.
   //!
   //! <b>Throws</b>: If memory allocation allocation throws or T's copy constructor throws.
   //!
   //! <b>Note</b>: If capacity() is less than "cnt", iterators and references to
   //!   to values might be invalidated.
   void reserve(size_type cnt)
      { m_flat_tree.reserve(cnt);   }

   //! <b>Effects</b>: Tries to deallocate the excess of memory created
   //    with previous allocations. The size of the vector is unchanged
   //!
   //! <b>Throws</b>: If memory allocation throws, or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to size().
   void shrink_to_fit()
      { m_flat_tree.shrink_to_fit(); }

   //////////////////////////////////////////////
   //
   //               element access
   //
   //////////////////////////////////////////////

   #if defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
   //! Effects: If there is no key equivalent to x in the flat_map, inserts
   //!   value_type(x, T()) into the flat_map.
   //!
   //! Returns: A reference to the mapped_type corresponding to x in *this.
   //!
   //! Complexity: Logarithmic.
   mapped_type &operator[](const key_type& k);

   //! Effects: If there is no key equivalent to x in the flat_map, inserts
   //! value_type(move(x), T()) into the flat_map (the key is move-constructed)
   //!
   //! Returns: A reference to the mapped_type corresponding to x in *this.
   //!
   //! Complexity: Logarithmic.
   mapped_type &operator[](key_type &&k) ;
   #elif defined(AUTOBOOST_MOVE_HELPERS_RETURN_SFINAE_BROKEN)
      //in compilers like GCC 3.4, we can't catch temporaries
      mapped_type& operator[](const key_type &k)         {  return this->priv_subscript(k);  }
      mapped_type& operator[](AUTOBOOST_RV_REF(key_type) k)  {  return this->priv_subscript(::autoboost::move(k));  }
   #else
      AUTOBOOST_MOVE_CONVERSION_AWARE_CATCH( operator[] , key_type, mapped_type&, this->priv_subscript)
   #endif

   //! @copydoc ::autoboost::container::flat_set::nth(size_type)
   iterator nth(size_type n) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return container_detail::force_copy<iterator>(m_flat_tree.nth(n));  }

   //! @copydoc ::autoboost::container::flat_set::nth(size_type) const
   const_iterator nth(size_type n) const AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return container_detail::force_copy<iterator>(m_flat_tree.nth(n));  }

   //! @copydoc ::autoboost::container::flat_set::index_of(iterator)
   size_type index_of(iterator p) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return m_flat_tree.index_of(container_detail::force_copy<impl_iterator>(p));  }

   //! @copydoc ::autoboost::container::flat_set::index_of(const_iterator) const
   size_type index_of(const_iterator p) const AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return m_flat_tree.index_of(container_detail::force_copy<impl_const_iterator>(p));  }

   //! Returns: A reference to the element whose key is equivalent to x.
   //!
   //! Throws: An exception object of type out_of_range if no such element is present.
   //!
   //! Complexity: logarithmic.
   T& at(const key_type& k)
   {
      iterator i = this->find(k);
      if(i == this->end()){
         throw_out_of_range("flat_map::at key not found");
      }
      return i->second;
   }

   //! Returns: A reference to the element whose key is equivalent to x.
   //!
   //! Throws: An exception object of type out_of_range if no such element is present.
   //!
   //! Complexity: logarithmic.
   const T& at(const key_type& k) const
   {
      const_iterator i = this->find(k);
      if(i == this->end()){
         throw_out_of_range("flat_map::at key not found");
      }
      return i->second;
   }

   //////////////////////////////////////////////
   //
   //                modifiers
   //
   //////////////////////////////////////////////

   #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)

   //! <b>Effects</b>: Inserts an object x of type T constructed with
   //!   std::forward<Args>(args)... if and only if there is no element in the container
   //!   with key equivalent to the key of x.
   //!
   //! <b>Returns</b>: The bool component of the returned pair is true if and only
   //!   if the insertion takes place, and the iterator component of the pair
   //!   points to the element with key equivalent to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   template <class... Args>
   std::pair<iterator,bool> emplace(AUTOBOOST_FWD_REF(Args)... args)
   {  return container_detail::force_copy< std::pair<iterator, bool> >(m_flat_tree.emplace_unique(autoboost::forward<Args>(args)...)); }

   //! <b>Effects</b>: Inserts an object of type T constructed with
   //!   std::forward<Args>(args)... in the container if and only if there is
   //!   no element in the container with key equivalent to the key of x.
   //!   p is a hint pointing to where the insert should start to search.
   //!
   //! <b>Returns</b>: An iterator pointing to the element with key equivalent
   //!   to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time (constant if x is inserted
   //!   right before p) plus insertion linear to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   template <class... Args>
   iterator emplace_hint(const_iterator hint, AUTOBOOST_FWD_REF(Args)... args)
   {
      return container_detail::force_copy<iterator>
         (m_flat_tree.emplace_hint_unique( container_detail::force_copy<impl_const_iterator>(hint)
                                         , autoboost::forward<Args>(args)...));
   }

   #else // !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   #define AUTOBOOST_CONTAINER_FLAT_MAP_EMPLACE_CODE(N) \
   AUTOBOOST_MOVE_TMPL_LT##N AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_GT##N \
   std::pair<iterator,bool> emplace(AUTOBOOST_MOVE_UREF##N)\
   {\
      return container_detail::force_copy< std::pair<iterator, bool> >\
         (m_flat_tree.emplace_unique(AUTOBOOST_MOVE_FWD##N));\
   }\
   \
   AUTOBOOST_MOVE_TMPL_LT##N AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_GT##N \
   iterator emplace_hint(const_iterator hint AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_UREF##N)\
   {\
      return container_detail::force_copy<iterator>(m_flat_tree.emplace_hint_unique\
         (container_detail::force_copy<impl_const_iterator>(hint) AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_FWD##N));\
   }\
   //
   AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_CONTAINER_FLAT_MAP_EMPLACE_CODE)
   #undef AUTOBOOST_CONTAINER_FLAT_MAP_EMPLACE_CODE

   #endif   // !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   //! <b>Effects</b>: Inserts x if and only if there is no element in the container
   //!   with key equivalent to the key of x.
   //!
   //! <b>Returns</b>: The bool component of the returned pair is true if and only
   //!   if the insertion takes place, and the iterator component of the pair
   //!   points to the element with key equivalent to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   std::pair<iterator,bool> insert(const value_type& x)
   {  return container_detail::force_copy<std::pair<iterator,bool> >(
         m_flat_tree.insert_unique(container_detail::force<impl_value_type>(x))); }

   //! <b>Effects</b>: Inserts a new value_type move constructed from the pair if and
   //! only if there is no element in the container with key equivalent to the key of x.
   //!
   //! <b>Returns</b>: The bool component of the returned pair is true if and only
   //!   if the insertion takes place, and the iterator component of the pair
   //!   points to the element with key equivalent to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   std::pair<iterator,bool> insert(AUTOBOOST_RV_REF(value_type) x)
   {  return container_detail::force_copy<std::pair<iterator,bool> >(
      m_flat_tree.insert_unique(autoboost::move(container_detail::force<impl_value_type>(x)))); }

   //! <b>Effects</b>: Inserts a new value_type move constructed from the pair if and
   //! only if there is no element in the container with key equivalent to the key of x.
   //!
   //! <b>Returns</b>: The bool component of the returned pair is true if and only
   //!   if the insertion takes place, and the iterator component of the pair
   //!   points to the element with key equivalent to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   std::pair<iterator,bool> insert(AUTOBOOST_RV_REF(movable_value_type) x)
   {
      return container_detail::force_copy<std::pair<iterator,bool> >
      (m_flat_tree.insert_unique(autoboost::move(x)));
   }

   //! <b>Effects</b>: Inserts a copy of x in the container if and only if there is
   //!   no element in the container with key equivalent to the key of x.
   //!   p is a hint pointing to where the insert should start to search.
   //!
   //! <b>Returns</b>: An iterator pointing to the element with key equivalent
   //!   to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time (constant if x is inserted
   //!   right before p) plus insertion linear to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   iterator insert(const_iterator p, const value_type& x)
   {
      return container_detail::force_copy<iterator>(
         m_flat_tree.insert_unique( container_detail::force_copy<impl_const_iterator>(p)
                                  , container_detail::force<impl_value_type>(x)));
   }

   //! <b>Effects</b>: Inserts an element move constructed from x in the container.
   //!   p is a hint pointing to where the insert should start to search.
   //!
   //! <b>Returns</b>: An iterator pointing to the element with key equivalent to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time (constant if x is inserted
   //!   right before p) plus insertion linear to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   iterator insert(const_iterator p, AUTOBOOST_RV_REF(value_type) x)
   {
      return container_detail::force_copy<iterator>
         (m_flat_tree.insert_unique( container_detail::force_copy<impl_const_iterator>(p)
                                   , autoboost::move(container_detail::force<impl_value_type>(x))));
   }

   //! <b>Effects</b>: Inserts an element move constructed from x in the container.
   //!   p is a hint pointing to where the insert should start to search.
   //!
   //! <b>Returns</b>: An iterator pointing to the element with key equivalent to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time (constant if x is inserted
   //!   right before p) plus insertion linear to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   iterator insert(const_iterator p, AUTOBOOST_RV_REF(movable_value_type) x)
   {
      return container_detail::force_copy<iterator>(
         m_flat_tree.insert_unique(container_detail::force_copy<impl_const_iterator>(p), autoboost::move(x)));
   }

   //! <b>Requires</b>: first, last are not iterators into *this.
   //!
   //! <b>Effects</b>: inserts each element from the range [first,last) if and only
   //!   if there is no element with key equivalent to the key of that element.
   //!
   //! <b>Complexity</b>: At most N log(size()+N) (N is the distance from first to last)
   //!   search time plus N*size() insertion time.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   template <class InputIterator>
   void insert(InputIterator first, InputIterator last)
   {  m_flat_tree.insert_unique(first, last);  }

   //! <b>Requires</b>: first, last are not iterators into *this.
   //!
   //! <b>Requires</b>: [first ,last) must be ordered according to the predicate and must be
   //! unique values.
   //!
   //! <b>Effects</b>: inserts each element from the range [first,last) if and only
   //!   if there is no element with key equivalent to the key of that element. This
   //!   function is more efficient than the normal range creation for ordered ranges.
   //!
   //! <b>Complexity</b>: At most N log(size()+N) (N is the distance from first to last)
   //!   search time plus N*size() insertion time.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   //!
   //! <b>Note</b>: Non-standard extension.
   template <class InputIterator>
   void insert(ordered_unique_range_t, InputIterator first, InputIterator last)
      {  m_flat_tree.insert_unique(ordered_unique_range, first, last); }

#if !defined(AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST)
   //! <b>Effects</b>: inserts each element from the range [il.begin(), il.end()) if and only
   //!   if there is no element with key equivalent to the key of that element.
   //!
   //! <b>Complexity</b>: At most N log(size()+N) (N is the distance from il.first() to il.end())
   //!   search time plus N*size() insertion time.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   void insert(std::initializer_list<value_type> il)
   {  m_flat_tree.insert_unique(il.begin(), il.end());  }

   //! <b>Requires</b>: [il.begin(), il.end()) must be ordered according to the predicate and must be
   //! unique values.
   //!
   //! <b>Effects</b>: inserts each element from the range [il.begin(), il.end()) if and only
   //!   if there is no element with key equivalent to the key of that element. This
   //!   function is more efficient than the normal range creation for ordered ranges.
   //!
   //! <b>Complexity</b>: At most N log(size()+N) (N is the distance from first to last)
   //!   search time plus N*size() insertion time.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   //!
   //! <b>Note</b>: Non-standard extension.
   void insert(ordered_unique_range_t, std::initializer_list<value_type> il)
   {  m_flat_tree.insert_unique(ordered_unique_range, il.begin(), il.end()); }
#endif

   //! <b>Effects</b>: Erases the element pointed to by p.
   //!
   //! <b>Returns</b>: Returns an iterator pointing to the element immediately
   //!   following q prior to the element being erased. If no such element exists,
   //!   returns end().
   //!
   //! <b>Complexity</b>: Linear to the elements with keys bigger than p
   //!
   //! <b>Note</b>: Invalidates elements with keys
   //!   not less than the erased element.
   iterator erase(const_iterator p)
   {
      return container_detail::force_copy<iterator>
         (m_flat_tree.erase(container_detail::force_copy<impl_const_iterator>(p)));
   }

   //! <b>Effects</b>: Erases all elements in the container with key equivalent to x.
   //!
   //! <b>Returns</b>: Returns the number of erased elements.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus erasure time
   //!   linear to the elements with bigger keys.
   size_type erase(const key_type& x)
      { return m_flat_tree.erase(x); }

   //! <b>Effects</b>: Erases all the elements in the range [first, last).
   //!
   //! <b>Returns</b>: Returns last.
   //!
   //! <b>Complexity</b>: size()*N where N is the distance from first to last.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus erasure time
   //!   linear to the elements with bigger keys.
   iterator erase(const_iterator first, const_iterator last)
   {
      return container_detail::force_copy<iterator>(
         m_flat_tree.erase( container_detail::force_copy<impl_const_iterator>(first)
                          , container_detail::force_copy<impl_const_iterator>(last)));
   }

   //! <b>Effects</b>: Swaps the contents of *this and x.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   void swap(flat_map& x)
      AUTOBOOST_NOEXCEPT_IF(  allocator_traits_type::is_always_equal::value
                                 && autoboost::container::container_detail::is_nothrow_swappable<Compare>::value )
   { m_flat_tree.swap(x.m_flat_tree); }

   //! <b>Effects</b>: erase(a.begin(),a.end()).
   //!
   //! <b>Postcondition</b>: size() == 0.
   //!
   //! <b>Complexity</b>: linear in size().
   void clear() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { m_flat_tree.clear(); }

   //////////////////////////////////////////////
   //
   //                observers
   //
   //////////////////////////////////////////////

   //! <b>Effects</b>: Returns the comparison object out
   //!   of which a was constructed.
   //!
   //! <b>Complexity</b>: Constant.
   key_compare key_comp() const
      { return container_detail::force_copy<key_compare>(m_flat_tree.key_comp()); }

   //! <b>Effects</b>: Returns an object of value_compare constructed out
   //!   of the comparison object.
   //!
   //! <b>Complexity</b>: Constant.
   value_compare value_comp() const
      { return value_compare(container_detail::force_copy<key_compare>(m_flat_tree.key_comp())); }

   //////////////////////////////////////////////
   //
   //              map operations
   //
   //////////////////////////////////////////////

   //! <b>Returns</b>: An iterator pointing to an element with the key
   //!   equivalent to x, or end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic.
   iterator find(const key_type& x)
      { return container_detail::force_copy<iterator>(m_flat_tree.find(x)); }

   //! <b>Returns</b>: A const_iterator pointing to an element with the key
   //!   equivalent to x, or end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic.
   const_iterator find(const key_type& x) const
      { return container_detail::force_copy<const_iterator>(m_flat_tree.find(x)); }

   //! <b>Returns</b>: The number of elements with key equivalent to x.
   //!
   //! <b>Complexity</b>: log(size())+count(k)
   size_type count(const key_type& x) const
      {  return static_cast<size_type>(m_flat_tree.find(x) != m_flat_tree.end());  }

   //! <b>Returns</b>: An iterator pointing to the first element with key not less
   //!   than k, or a.end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic.
   iterator lower_bound(const key_type& x)
      {  return container_detail::force_copy<iterator>(m_flat_tree.lower_bound(x)); }

   //! <b>Returns</b>: A const iterator pointing to the first element with key not
   //!   less than k, or a.end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic.
   const_iterator lower_bound(const key_type& x) const
      {  return container_detail::force_copy<const_iterator>(m_flat_tree.lower_bound(x)); }

   //! <b>Returns</b>: An iterator pointing to the first element with key not less
   //!   than x, or end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic.
   iterator upper_bound(const key_type& x)
      {  return container_detail::force_copy<iterator>(m_flat_tree.upper_bound(x)); }

   //! <b>Returns</b>: A const iterator pointing to the first element with key not
   //!   less than x, or end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic.
   const_iterator upper_bound(const key_type& x) const
      {  return container_detail::force_copy<const_iterator>(m_flat_tree.upper_bound(x)); }

   //! <b>Effects</b>: Equivalent to std::make_pair(this->lower_bound(k), this->upper_bound(k)).
   //!
   //! <b>Complexity</b>: Logarithmic.
   std::pair<iterator,iterator> equal_range(const key_type& x)
      {  return container_detail::force_copy<std::pair<iterator,iterator> >(m_flat_tree.lower_bound_range(x)); }

   //! <b>Effects</b>: Equivalent to std::make_pair(this->lower_bound(k), this->upper_bound(k)).
   //!
   //! <b>Complexity</b>: Logarithmic.
   std::pair<const_iterator,const_iterator> equal_range(const key_type& x) const
      {  return container_detail::force_copy<std::pair<const_iterator,const_iterator> >(m_flat_tree.lower_bound_range(x)); }

   //! <b>Effects</b>: Returns true if x and y are equal
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator==(const flat_map& x, const flat_map& y)
   {  return x.size() == y.size() && ::autoboost::container::algo_equal(x.begin(), x.end(), y.begin());  }

   //! <b>Effects</b>: Returns true if x and y are unequal
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator!=(const flat_map& x, const flat_map& y)
   {  return !(x == y); }

   //! <b>Effects</b>: Returns true if x is less than y
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator<(const flat_map& x, const flat_map& y)
   {  return ::autoboost::container::algo_lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());  }

   //! <b>Effects</b>: Returns true if x is greater than y
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator>(const flat_map& x, const flat_map& y)
   {  return y < x;  }

   //! <b>Effects</b>: Returns true if x is equal or less than y
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator<=(const flat_map& x, const flat_map& y)
   {  return !(y < x);  }

   //! <b>Effects</b>: Returns true if x is equal or greater than y
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator>=(const flat_map& x, const flat_map& y)
   {  return !(x < y);  }

   //! <b>Effects</b>: x.swap(y)
   //!
   //! <b>Complexity</b>: Constant.
   friend void swap(flat_map& x, flat_map& y)
   {  x.swap(y);  }

   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   private:
   mapped_type &priv_subscript(const key_type& k)
   {
      iterator i = lower_bound(k);
      // i->first is greater than or equivalent to k.
      if (i == end() || key_comp()(k, (*i).first)){
         container_detail::value_init<mapped_type> m;
         i = insert(i, impl_value_type(k, ::autoboost::move(m.m_t)));
      }
      return (*i).second;
   }
   mapped_type &priv_subscript(AUTOBOOST_RV_REF(key_type) mk)
   {
      key_type &k = mk;
      iterator i = lower_bound(k);
      // i->first is greater than or equivalent to k.
      if (i == end() || key_comp()(k, (*i).first)){
         container_detail::value_init<mapped_type> m;
         i = insert(i, impl_value_type(autoboost::move(k), ::autoboost::move(m.m_t)));
      }
      return (*i).second;
   }
   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
};

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

}  //namespace container {

//!has_trivial_destructor_after_move<> == true_type
//!specialization for optimizations
template <class Key, class T, class Compare, class Allocator>
struct has_trivial_destructor_after_move<autoboost::container::flat_map<Key, T, Compare, Allocator> >
{
   typedef typename ::autoboost::container::allocator_traits<Allocator>::pointer pointer;
   static const bool value = ::autoboost::has_trivial_destructor_after_move<Allocator>::value &&
                             ::autoboost::has_trivial_destructor_after_move<pointer>::value &&
                             ::autoboost::has_trivial_destructor_after_move<Compare>::value;
};

namespace container {

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

//! A flat_multimap is a kind of associative container that supports equivalent keys
//! (possibly containing multiple copies of the same key value) and provides for
//! fast retrieval of values of another type T based on the keys. The flat_multimap
//! class supports random-access iterators.
//!
//! A flat_multimap satisfies all of the requirements of a container and of a reversible
//! container and of an associative container. For a
//! flat_multimap<Key,T> the key_type is Key and the value_type is std::pair<Key,T>
//! (unlike std::multimap<Key, T> which value_type is std::pair<<b>const</b> Key, T>).
//!
//! Compare is the ordering function for Keys (e.g. <i>std::less<Key></i>).
//!
//! Allocator is the allocator to allocate the value_types
//! (e.g. <i>allocator< std::pair<Key, T> ></i>).
//!
//! flat_multimap is similar to std::multimap but it's implemented like an ordered vector.
//! This means that inserting a new element into a flat_map invalidates
//! previous iterators and references
//!
//! Erasing an element invalidates iterators and references
//! pointing to elements that come after (their keys are bigger) the erased element.
//!
//! This container provides random-access iterators.
//!
//! \tparam Key is the key_type of the map
//! \tparam Value is the <code>mapped_type</code>
//! \tparam Compare is the ordering function for Keys (e.g. <i>std::less<Key></i>).
//! \tparam Allocator is the allocator to allocate the <code>value_type</code>s
//!   (e.g. <i>allocator< std::pair<Key, T> > </i>).
#ifdef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
template <class Key, class T, class Compare = std::less<Key>, class Allocator = new_allocator< std::pair< Key, T> > >
#else
template <class Key, class T, class Compare, class Allocator>
#endif
class flat_multimap
{
   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   private:
   AUTOBOOST_COPYABLE_AND_MOVABLE(flat_multimap)
   typedef container_detail::flat_tree<Key,
                           std::pair<Key, T>,
                           container_detail::select1st< std::pair<Key, T> >,
                           Compare,
                           Allocator> tree_t;
   //This is the real tree stored here. It's based on a movable pair
   typedef container_detail::flat_tree<Key,
                           container_detail::pair<Key, T>,
                           container_detail::select1st<container_detail::pair<Key, T> >,
                           Compare,
                           typename allocator_traits<Allocator>::template portable_rebind_alloc
                              <container_detail::pair<Key, T> >::type> impl_tree_t;
   impl_tree_t m_flat_tree;  // flat tree representing flat_map

   typedef typename impl_tree_t::value_type              impl_value_type;
   typedef typename impl_tree_t::const_iterator          impl_const_iterator;
   typedef typename impl_tree_t::iterator                impl_iterator;
   typedef typename impl_tree_t::allocator_type          impl_allocator_type;
   typedef container_detail::flat_tree_value_compare
      < Compare
      , container_detail::select1st< std::pair<Key, T> >
      , std::pair<Key, T> >                                                         value_compare_impl;
   typedef typename container_detail::get_flat_tree_iterators
         <typename allocator_traits<Allocator>::pointer>::iterator                  iterator_impl;
   typedef typename container_detail::get_flat_tree_iterators
      <typename allocator_traits<Allocator>::pointer>::const_iterator               const_iterator_impl;
   typedef typename container_detail::get_flat_tree_iterators
         <typename allocator_traits<Allocator>::pointer>::reverse_iterator          reverse_iterator_impl;
   typedef typename container_detail::get_flat_tree_iterators
         <typename allocator_traits<Allocator>::pointer>::const_reverse_iterator    const_reverse_iterator_impl;
   public:
   typedef typename impl_tree_t::stored_allocator_type   impl_stored_allocator_type;
   private:
   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

   public:

   //////////////////////////////////////////////
   //
   //                    types
   //
   //////////////////////////////////////////////
   typedef Key                                                                      key_type;
   typedef T                                                                        mapped_type;
   typedef std::pair<Key, T>                                                        value_type;
   typedef ::autoboost::container::allocator_traits<Allocator>                          allocator_traits_type;
   typedef typename autoboost::container::allocator_traits<Allocator>::pointer          pointer;
   typedef typename autoboost::container::allocator_traits<Allocator>::const_pointer    const_pointer;
   typedef typename autoboost::container::allocator_traits<Allocator>::reference        reference;
   typedef typename autoboost::container::allocator_traits<Allocator>::const_reference  const_reference;
   typedef typename autoboost::container::allocator_traits<Allocator>::size_type        size_type;
   typedef typename autoboost::container::allocator_traits<Allocator>::difference_type  difference_type;
   typedef Allocator                                                                allocator_type;
   typedef AUTOBOOST_CONTAINER_IMPDEF(Allocator)                                        stored_allocator_type;
   typedef AUTOBOOST_CONTAINER_IMPDEF(value_compare_impl)                               value_compare;
   typedef Compare                                                                  key_compare;
   typedef AUTOBOOST_CONTAINER_IMPDEF(iterator_impl)                                    iterator;
   typedef AUTOBOOST_CONTAINER_IMPDEF(const_iterator_impl)                              const_iterator;
   typedef AUTOBOOST_CONTAINER_IMPDEF(reverse_iterator_impl)                            reverse_iterator;
   typedef AUTOBOOST_CONTAINER_IMPDEF(const_reverse_iterator_impl)                      const_reverse_iterator;
   typedef AUTOBOOST_CONTAINER_IMPDEF(impl_value_type)                                  movable_value_type;

   //////////////////////////////////////////////
   //
   //          construct/copy/destroy
   //
   //////////////////////////////////////////////

   //! <b>Effects</b>: Default constructs an empty flat_map.
   //!
   //! <b>Complexity</b>: Constant.
   flat_multimap()
      : m_flat_tree()
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_multimap using the specified comparison
   //!   object and allocator.
   //!
   //! <b>Complexity</b>: Constant.
   explicit flat_multimap(const Compare& comp,
                          const allocator_type& a = allocator_type())
      : m_flat_tree(comp, container_detail::force<impl_allocator_type>(a))
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_multimap using the specified allocator.
   //!
   //! <b>Complexity</b>: Constant.
   explicit flat_multimap(const allocator_type& a)
      : m_flat_tree(container_detail::force<impl_allocator_type>(a))
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_multimap using the specified comparison object
   //!   and allocator, and inserts elements from the range [first ,last ).
   //!
   //! <b>Complexity</b>: Linear in N if the range [first ,last ) is already sorted using
   //! comp and otherwise N logN, where N is last - first.
   template <class InputIterator>
   flat_multimap(InputIterator first, InputIterator last,
            const Compare& comp        = Compare(),
            const allocator_type& a = allocator_type())
      : m_flat_tree(false, first, last, comp, container_detail::force<impl_allocator_type>(a))
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_multimap using the specified
   //!   allocator, and inserts elements from the range [first ,last ).
   //!
   //! <b>Complexity</b>: Linear in N if the range [first ,last ) is already sorted using
   //! comp and otherwise N logN, where N is last - first.
   template <class InputIterator>
   flat_multimap(InputIterator first, InputIterator last, const allocator_type& a)
      : m_flat_tree(false, first, last, Compare(), container_detail::force<impl_allocator_type>(a))
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_multimap using the specified comparison object and
   //! allocator, and inserts elements from the ordered range [first ,last). This function
   //! is more efficient than the normal range creation for ordered ranges.
   //!
   //! <b>Requires</b>: [first ,last) must be ordered according to the predicate.
   //!
   //! <b>Complexity</b>: Linear in N.
   //!
   //! <b>Note</b>: Non-standard extension.
   template <class InputIterator>
   flat_multimap(ordered_range_t, InputIterator first, InputIterator last,
            const Compare& comp        = Compare(),
            const allocator_type& a = allocator_type())
      : m_flat_tree(ordered_range, first, last, comp, a)
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

#if !defined(AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST)
   //! <b>Effects</b>: Constructs an empty flat_map using the specified comparison object and
   //! allocator, and inserts elements from the range [il.begin(), il.end()).
   //!
   //! <b>Complexity</b>: Linear in N if the range [il.begin(), il.end()) is already sorted using
   //! comp and otherwise N logN, where N is last - first.
   flat_multimap(std::initializer_list<value_type> il, const Compare& comp = Compare(), const allocator_type& a = allocator_type())
      : m_flat_tree(false, il.begin(), il.end(), comp, container_detail::force<impl_allocator_type>(a))
   {
       //A type must be std::pair<Key, T>
       AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_map using the specified
   //! allocator, and inserts elements from the range [il.begin(), il.end()).
   //!
   //! <b>Complexity</b>: Linear in N if the range [il.begin(), il.end()) is already sorted using
   //! comp and otherwise N logN, where N is last - first.
   flat_multimap(std::initializer_list<value_type> il, const allocator_type& a)
      : m_flat_tree(false, il.begin(), il.end(), Compare(), container_detail::force<impl_allocator_type>(a))
   {
       //A type must be std::pair<Key, T>
       AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Constructs an empty flat_multimap using the specified comparison object and
   //! allocator, and inserts elements from the ordered range [il.begin(), il.end()). This function
   //! is more efficient than the normal range creation for ordered ranges.
   //!
   //! <b>Requires</b>: [il.begin(), il.end()) must be ordered according to the predicate.
   //!
   //! <b>Complexity</b>: Linear in N.
   //!
   //! <b>Note</b>: Non-standard extension.
   flat_multimap(ordered_range_t, std::initializer_list<value_type> il, const Compare& comp = Compare(),
                 const allocator_type& a = allocator_type())
      : m_flat_tree(ordered_range, il.begin(), il.end(), comp, a)
   {
       //A type must be std::pair<Key, T>
       AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }
#endif

   //! <b>Effects</b>: Copy constructs a flat_multimap.
   //!
   //! <b>Complexity</b>: Linear in x.size().
   flat_multimap(const flat_multimap& x)
      : m_flat_tree(x.m_flat_tree)
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Move constructs a flat_multimap. Constructs *this using x's resources.
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Postcondition</b>: x is emptied.
   flat_multimap(AUTOBOOST_RV_REF(flat_multimap) x)
      : m_flat_tree(autoboost::move(x.m_flat_tree))
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Copy constructs a flat_multimap using the specified allocator.
   //!
   //! <b>Complexity</b>: Linear in x.size().
   flat_multimap(const flat_multimap& x, const allocator_type &a)
      : m_flat_tree(x.m_flat_tree, a)
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Move constructs a flat_multimap using the specified allocator.
   //!                 Constructs *this using x's resources.
   //!
   //! <b>Complexity</b>: Constant if a == x.get_allocator(), linear otherwise.
   flat_multimap(AUTOBOOST_RV_REF(flat_multimap) x, const allocator_type &a)
      : m_flat_tree(autoboost::move(x.m_flat_tree), a)
   {
      //A type must be std::pair<Key, T>
      AUTOBOOST_STATIC_ASSERT((container_detail::is_same<std::pair<Key, T>, typename Allocator::value_type>::value));
   }

   //! <b>Effects</b>: Makes *this a copy of x.
   //!
   //! <b>Complexity</b>: Linear in x.size().
   flat_multimap& operator=(AUTOBOOST_COPY_ASSIGN_REF(flat_multimap) x)
      {  m_flat_tree = x.m_flat_tree;   return *this;  }

   //! <b>Effects</b>: this->swap(x.get()).
   //!
   //! <b>Complexity</b>: Constant.
   flat_multimap& operator=(AUTOBOOST_RV_REF(flat_multimap) x)
      AUTOBOOST_NOEXCEPT_IF(  allocator_traits_type::is_always_equal::value
                                 && autoboost::container::container_detail::is_nothrow_move_assignable<Compare>::value )
      {  m_flat_tree = autoboost::move(x.m_flat_tree);   return *this;  }

#if !defined(AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST)
   //! <b>Effects</b>: Assign content of il to *this
   //!
   //! <b>Complexity</b>: Linear in il.size().
   flat_multimap& operator=(std::initializer_list<value_type> il)
   {
      this->clear();
      this->insert(il.begin(), il.end());
      return *this;
   }
#endif

   //! <b>Effects</b>: Returns a copy of the allocator that
   //!   was passed to the object's constructor.
   //!
   //! <b>Complexity</b>: Constant.
   allocator_type get_allocator() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<allocator_type>(m_flat_tree.get_allocator()); }

   //! <b>Effects</b>: Returns a reference to the internal allocator.
   //!
   //! <b>Throws</b>: Nothing
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Note</b>: Non-standard extension.
   stored_allocator_type &get_stored_allocator() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force<stored_allocator_type>(m_flat_tree.get_stored_allocator()); }

   //! <b>Effects</b>: Returns a reference to the internal allocator.
   //!
   //! <b>Throws</b>: Nothing
   //!
   //! <b>Complexity</b>: Constant.
   //!
   //! <b>Note</b>: Non-standard extension.
   const stored_allocator_type &get_stored_allocator() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force<stored_allocator_type>(m_flat_tree.get_stored_allocator()); }

   //////////////////////////////////////////////
   //
   //                iterators
   //
   //////////////////////////////////////////////

   //! <b>Effects</b>: Returns an iterator to the first element contained in the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   iterator begin() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<iterator>(m_flat_tree.begin()); }

   //! <b>Effects</b>: Returns a const_iterator to the first element contained in the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_iterator begin() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_iterator>(m_flat_tree.begin()); }

   //! <b>Effects</b>: Returns an iterator to the end of the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   iterator end() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<iterator>(m_flat_tree.end()); }

   //! <b>Effects</b>: Returns a const_iterator to the end of the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_iterator end() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_iterator>(m_flat_tree.end()); }

   //! <b>Effects</b>: Returns a reverse_iterator pointing to the beginning
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   reverse_iterator rbegin() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<reverse_iterator>(m_flat_tree.rbegin()); }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the beginning
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator rbegin() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_reverse_iterator>(m_flat_tree.rbegin()); }

   //! <b>Effects</b>: Returns a reverse_iterator pointing to the end
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   reverse_iterator rend() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<reverse_iterator>(m_flat_tree.rend()); }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the end
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator rend() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_reverse_iterator>(m_flat_tree.rend()); }

   //! <b>Effects</b>: Returns a const_iterator to the first element contained in the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_iterator cbegin() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_iterator>(m_flat_tree.cbegin()); }

   //! <b>Effects</b>: Returns a const_iterator to the end of the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_iterator cend() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_iterator>(m_flat_tree.cend()); }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the beginning
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator crbegin() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_reverse_iterator>(m_flat_tree.crbegin()); }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the end
   //! of the reversed container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator crend() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return container_detail::force_copy<const_reverse_iterator>(m_flat_tree.crend()); }

   //////////////////////////////////////////////
   //
   //                capacity
   //
   //////////////////////////////////////////////

   //! <b>Effects</b>: Returns true if the container contains no elements.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   bool empty() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return m_flat_tree.empty(); }

   //! <b>Effects</b>: Returns the number of the elements contained in the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   size_type size() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return m_flat_tree.size(); }

   //! <b>Effects</b>: Returns the largest possible size of the container.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   size_type max_size() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return m_flat_tree.max_size(); }

   //! <b>Effects</b>: Number of elements for which memory has been allocated.
   //!   capacity() is always greater than or equal to size().
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   size_type capacity() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { return m_flat_tree.capacity(); }

   //! <b>Effects</b>: If n is less than or equal to capacity(), this call has no
   //!   effect. Otherwise, it is a request for allocation of additional memory.
   //!   If the request is successful, then capacity() is greater than or equal to
   //!   n; otherwise, capacity() is unchanged. In either case, size() is unchanged.
   //!
   //! <b>Throws</b>: If memory allocation allocation throws or T's copy constructor throws.
   //!
   //! <b>Note</b>: If capacity() is less than "cnt", iterators and references to
   //!   to values might be invalidated.
   void reserve(size_type cnt)
      { m_flat_tree.reserve(cnt);   }

   //! <b>Effects</b>: Tries to deallocate the excess of memory created
   //    with previous allocations. The size of the vector is unchanged
   //!
   //! <b>Throws</b>: If memory allocation throws, or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to size().
   void shrink_to_fit()
      { m_flat_tree.shrink_to_fit(); }

   //! @copydoc ::autoboost::container::flat_set::nth(size_type)
   iterator nth(size_type n) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return container_detail::force_copy<iterator>(m_flat_tree.nth(n));  }

   //! @copydoc ::autoboost::container::flat_set::nth(size_type) const
   const_iterator nth(size_type n) const AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return container_detail::force_copy<iterator>(m_flat_tree.nth(n));  }

   //! @copydoc ::autoboost::container::flat_set::index_of(iterator)
   size_type index_of(iterator p) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return m_flat_tree.index_of(container_detail::force_copy<impl_iterator>(p));  }

   //! @copydoc ::autoboost::container::flat_set::index_of(const_iterator) const
   size_type index_of(const_iterator p) const AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return m_flat_tree.index_of(container_detail::force_copy<impl_const_iterator>(p));  }

   #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)

   //! <b>Effects</b>: Inserts an object of type T constructed with
   //!   std::forward<Args>(args)... and returns the iterator pointing to the
   //!   newly inserted element.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   template <class... Args>
   iterator emplace(AUTOBOOST_FWD_REF(Args)... args)
   {  return container_detail::force_copy<iterator>(m_flat_tree.emplace_equal(autoboost::forward<Args>(args)...)); }

   //! <b>Effects</b>: Inserts an object of type T constructed with
   //!   std::forward<Args>(args)... in the container.
   //!   p is a hint pointing to where the insert should start to search.
   //!
   //! <b>Returns</b>: An iterator pointing to the element with key equivalent
   //!   to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time (constant time if the value
   //!   is to be inserted before p) plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   template <class... Args>
   iterator emplace_hint(const_iterator hint, AUTOBOOST_FWD_REF(Args)... args)
   {
      return container_detail::force_copy<iterator>(m_flat_tree.emplace_hint_equal
         (container_detail::force_copy<impl_const_iterator>(hint), autoboost::forward<Args>(args)...));
   }

   #else // !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   #define AUTOBOOST_CONTAINER_FLAT_MULTIMAP_EMPLACE_CODE(N) \
   AUTOBOOST_MOVE_TMPL_LT##N AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_GT##N \
   iterator emplace(AUTOBOOST_MOVE_UREF##N)\
   {  return container_detail::force_copy<iterator>(m_flat_tree.emplace_equal(AUTOBOOST_MOVE_FWD##N));  }\
   \
   AUTOBOOST_MOVE_TMPL_LT##N AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_GT##N \
   iterator emplace_hint(const_iterator hint AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_UREF##N)\
   {\
      return container_detail::force_copy<iterator>(m_flat_tree.emplace_hint_equal\
         (container_detail::force_copy<impl_const_iterator>(hint) AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_FWD##N));\
   }\
   //
   AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_CONTAINER_FLAT_MULTIMAP_EMPLACE_CODE)
   #undef AUTOBOOST_CONTAINER_FLAT_MULTIMAP_EMPLACE_CODE

   #endif   // !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   //! <b>Effects</b>: Inserts x and returns the iterator pointing to the
   //!   newly inserted element.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   iterator insert(const value_type& x)
   {
      return container_detail::force_copy<iterator>(
         m_flat_tree.insert_equal(container_detail::force<impl_value_type>(x)));
   }

   //! <b>Effects</b>: Inserts a new value move-constructed from x and returns
   //!   the iterator pointing to the newly inserted element.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   iterator insert(AUTOBOOST_RV_REF(value_type) x)
   { return container_detail::force_copy<iterator>(m_flat_tree.insert_equal(autoboost::move(x))); }

   //! <b>Effects</b>: Inserts a new value move-constructed from x and returns
   //!   the iterator pointing to the newly inserted element.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   iterator insert(AUTOBOOST_RV_REF(impl_value_type) x)
      { return container_detail::force_copy<iterator>(m_flat_tree.insert_equal(autoboost::move(x))); }

   //! <b>Effects</b>: Inserts a copy of x in the container.
   //!   p is a hint pointing to where the insert should start to search.
   //!
   //! <b>Returns</b>: An iterator pointing to the element with key equivalent
   //!   to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time (constant time if the value
   //!   is to be inserted before p) plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   iterator insert(const_iterator p, const value_type& x)
   {
      return container_detail::force_copy<iterator>
         (m_flat_tree.insert_equal( container_detail::force_copy<impl_const_iterator>(p)
                                  , container_detail::force<impl_value_type>(x)));
   }

   //! <b>Effects</b>: Inserts a value move constructed from x in the container.
   //!   p is a hint pointing to where the insert should start to search.
   //!
   //! <b>Returns</b>: An iterator pointing to the element with key equivalent
   //!   to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time (constant time if the value
   //!   is to be inserted before p) plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   iterator insert(const_iterator p, AUTOBOOST_RV_REF(value_type) x)
   {
      return container_detail::force_copy<iterator>
         (m_flat_tree.insert_equal(container_detail::force_copy<impl_const_iterator>(p)
                                  , autoboost::move(x)));
   }

   //! <b>Effects</b>: Inserts a value move constructed from x in the container.
   //!   p is a hint pointing to where the insert should start to search.
   //!
   //! <b>Returns</b>: An iterator pointing to the element with key equivalent
   //!   to the key of x.
   //!
   //! <b>Complexity</b>: Logarithmic search time (constant time if the value
   //!   is to be inserted before p) plus linear insertion
   //!   to the elements with bigger keys than x.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   iterator insert(const_iterator p, AUTOBOOST_RV_REF(impl_value_type) x)
   {
      return container_detail::force_copy<iterator>(
         m_flat_tree.insert_equal(container_detail::force_copy<impl_const_iterator>(p), autoboost::move(x)));
   }

   //! <b>Requires</b>: first, last are not iterators into *this.
   //!
   //! <b>Effects</b>: inserts each element from the range [first,last) .
   //!
   //! <b>Complexity</b>: At most N log(size()+N) (N is the distance from first to last)
   //!   search time plus N*size() insertion time.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   template <class InputIterator>
   void insert(InputIterator first, InputIterator last)
      {  m_flat_tree.insert_equal(first, last); }

   //! <b>Requires</b>: first, last are not iterators into *this.
   //!
   //! <b>Requires</b>: [first ,last) must be ordered according to the predicate.
   //!
   //! <b>Effects</b>: inserts each element from the range [first,last) if and only
   //!   if there is no element with key equivalent to the key of that element. This
   //!   function is more efficient than the normal range creation for ordered ranges.
   //!
   //! <b>Complexity</b>: At most N log(size()+N) (N is the distance from first to last)
   //!   search time plus N*size() insertion time.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   //!
   //! <b>Note</b>: Non-standard extension.
   template <class InputIterator>
   void insert(ordered_range_t, InputIterator first, InputIterator last)
      {  m_flat_tree.insert_equal(ordered_range, first, last); }

#if !defined(AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST)
   //! <b>Effects</b>: inserts each element from the range [il.begin(), il.end()) .
   //!
   //! <b>Complexity</b>: At most N log(size()+N) (N is the distance from first to last)
   //!   search time plus N*size() insertion time.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   void insert(std::initializer_list<value_type> il)
   {  m_flat_tree.insert_equal(il.begin(), il.end()); }

   //! <b>Requires</b>: [il.begin(), il.end()) must be ordered according to the predicate.
   //!
   //! <b>Effects</b>: inserts each element from the range [il.begin(), il.end()) if and only
   //!   if there is no element with key equivalent to the key of that element. This
   //!   function is more efficient than the normal range creation for ordered ranges.
   //!
   //! <b>Complexity</b>: At most N log(size()+N) (N is the distance from first to last)
   //!   search time plus N*size() insertion time.
   //!
   //! <b>Note</b>: If an element is inserted it might invalidate elements.
   //!
   //! <b>Note</b>: Non-standard extension.
   void insert(ordered_range_t, std::initializer_list<value_type> il)
   {  m_flat_tree.insert_equal(ordered_range, il.begin(), il.end());  }
#endif

   //! <b>Effects</b>: Erases the element pointed to by p.
   //!
   //! <b>Returns</b>: Returns an iterator pointing to the element immediately
   //!   following q prior to the element being erased. If no such element exists,
   //!   returns end().
   //!
   //! <b>Complexity</b>: Linear to the elements with keys bigger than p
   //!
   //! <b>Note</b>: Invalidates elements with keys
   //!   not less than the erased element.
   iterator erase(const_iterator p)
   {
      return container_detail::force_copy<iterator>(
         m_flat_tree.erase(container_detail::force_copy<impl_const_iterator>(p)));
   }

   //! <b>Effects</b>: Erases all elements in the container with key equivalent to x.
   //!
   //! <b>Returns</b>: Returns the number of erased elements.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus erasure time
   //!   linear to the elements with bigger keys.
   size_type erase(const key_type& x)
      { return m_flat_tree.erase(x); }

   //! <b>Effects</b>: Erases all the elements in the range [first, last).
   //!
   //! <b>Returns</b>: Returns last.
   //!
   //! <b>Complexity</b>: size()*N where N is the distance from first to last.
   //!
   //! <b>Complexity</b>: Logarithmic search time plus erasure time
   //!   linear to the elements with bigger keys.
   iterator erase(const_iterator first, const_iterator last)
   {
      return container_detail::force_copy<iterator>
         (m_flat_tree.erase( container_detail::force_copy<impl_const_iterator>(first)
                           , container_detail::force_copy<impl_const_iterator>(last)));
   }

   //! <b>Effects</b>: Swaps the contents of *this and x.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   void swap(flat_multimap& x)
      AUTOBOOST_NOEXCEPT_IF(  allocator_traits_type::is_always_equal::value
                                 && autoboost::container::container_detail::is_nothrow_swappable<Compare>::value )
   { m_flat_tree.swap(x.m_flat_tree); }

   //! <b>Effects</b>: erase(a.begin(),a.end()).
   //!
   //! <b>Postcondition</b>: size() == 0.
   //!
   //! <b>Complexity</b>: linear in size().
   void clear() AUTOBOOST_NOEXCEPT_OR_NOTHROW
      { m_flat_tree.clear(); }

   //////////////////////////////////////////////
   //
   //                observers
   //
   //////////////////////////////////////////////

   //! <b>Effects</b>: Returns the comparison object out
   //!   of which a was constructed.
   //!
   //! <b>Complexity</b>: Constant.
   key_compare key_comp() const
      { return container_detail::force_copy<key_compare>(m_flat_tree.key_comp()); }

   //! <b>Effects</b>: Returns an object of value_compare constructed out
   //!   of the comparison object.
   //!
   //! <b>Complexity</b>: Constant.
   value_compare value_comp() const
      { return value_compare(container_detail::force_copy<key_compare>(m_flat_tree.key_comp())); }

   //////////////////////////////////////////////
   //
   //              map operations
   //
   //////////////////////////////////////////////

   //! <b>Returns</b>: An iterator pointing to an element with the key
   //!   equivalent to x, or end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic.
   iterator find(const key_type& x)
      { return container_detail::force_copy<iterator>(m_flat_tree.find(x)); }

   //! <b>Returns</b>: An const_iterator pointing to an element with the key
   //!   equivalent to x, or end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic.
   const_iterator find(const key_type& x) const
      { return container_detail::force_copy<const_iterator>(m_flat_tree.find(x)); }

   //! <b>Returns</b>: The number of elements with key equivalent to x.
   //!
   //! <b>Complexity</b>: log(size())+count(k)
   size_type count(const key_type& x) const
      { return m_flat_tree.count(x); }

   //! <b>Returns</b>: An iterator pointing to the first element with key not less
   //!   than k, or a.end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic
   iterator lower_bound(const key_type& x)
      {  return container_detail::force_copy<iterator>(m_flat_tree.lower_bound(x)); }

   //! <b>Returns</b>: A const iterator pointing to the first element with key
   //!   not less than k, or a.end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic
   const_iterator lower_bound(const key_type& x) const
      {  return container_detail::force_copy<const_iterator>(m_flat_tree.lower_bound(x));  }

   //! <b>Returns</b>: An iterator pointing to the first element with key not less
   //!   than x, or end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic
   iterator upper_bound(const key_type& x)
      {return container_detail::force_copy<iterator>(m_flat_tree.upper_bound(x)); }

   //! <b>Returns</b>: A const iterator pointing to the first element with key
   //!   not less than x, or end() if such an element is not found.
   //!
   //! <b>Complexity</b>: Logarithmic
   const_iterator upper_bound(const key_type& x) const
      {  return container_detail::force_copy<const_iterator>(m_flat_tree.upper_bound(x)); }

   //! <b>Effects</b>: Equivalent to std::make_pair(this->lower_bound(k), this->upper_bound(k)).
   //!
   //! <b>Complexity</b>: Logarithmic
   std::pair<iterator,iterator> equal_range(const key_type& x)
      {  return container_detail::force_copy<std::pair<iterator,iterator> >(m_flat_tree.equal_range(x));   }

   //! <b>Effects</b>: Equivalent to std::make_pair(this->lower_bound(k), this->upper_bound(k)).
   //!
   //! <b>Complexity</b>: Logarithmic
   std::pair<const_iterator,const_iterator> equal_range(const key_type& x) const
      {  return container_detail::force_copy<std::pair<const_iterator,const_iterator> >(m_flat_tree.equal_range(x));   }

   //! <b>Effects</b>: Returns true if x and y are equal
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator==(const flat_multimap& x, const flat_multimap& y)
   {  return x.size() == y.size() && ::autoboost::container::algo_equal(x.begin(), x.end(), y.begin());  }

   //! <b>Effects</b>: Returns true if x and y are unequal
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator!=(const flat_multimap& x, const flat_multimap& y)
   {  return !(x == y); }

   //! <b>Effects</b>: Returns true if x is less than y
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator<(const flat_multimap& x, const flat_multimap& y)
   {  return ::autoboost::container::algo_lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());  }

   //! <b>Effects</b>: Returns true if x is greater than y
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator>(const flat_multimap& x, const flat_multimap& y)
   {  return y < x;  }

   //! <b>Effects</b>: Returns true if x is equal or less than y
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator<=(const flat_multimap& x, const flat_multimap& y)
   {  return !(y < x);  }

   //! <b>Effects</b>: Returns true if x is equal or greater than y
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the container.
   friend bool operator>=(const flat_multimap& x, const flat_multimap& y)
   {  return !(x < y);  }

   //! <b>Effects</b>: x.swap(y)
   //!
   //! <b>Complexity</b>: Constant.
   friend void swap(flat_multimap& x, flat_multimap& y)
   {  x.swap(y);  }
};

}}

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

namespace autoboost {

//!has_trivial_destructor_after_move<> == true_type
//!specialization for optimizations
template <class Key, class T, class Compare, class Allocator>
struct has_trivial_destructor_after_move< autoboost::container::flat_multimap<Key, T, Compare, Allocator> >
{
   typedef typename ::autoboost::container::allocator_traits<Allocator>::pointer pointer;
   static const bool value = ::autoboost::has_trivial_destructor_after_move<Allocator>::value &&
                             ::autoboost::has_trivial_destructor_after_move<pointer>::value &&
                             ::autoboost::has_trivial_destructor_after_move<Compare>::value;
};

}  //namespace autoboost {

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

#include <autoboost/container/detail/config_end.hpp>

#endif   // AUTOBOOST_CONTAINER_FLAT_MAP_HPP

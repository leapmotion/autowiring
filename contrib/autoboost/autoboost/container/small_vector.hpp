//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2015-2015. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_CONTAINER_SMALL_VECTOR_HPP
#define AUTOBOOST_CONTAINER_CONTAINER_SMALL_VECTOR_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>

// container
#include <autoboost/container/container_fwd.hpp>
#include <autoboost/container/vector.hpp>
#include <autoboost/container/allocator_traits.hpp>
#include <autoboost/container/new_allocator.hpp> //new_allocator
// container/detail
#include <autoboost/container/detail/type_traits.hpp>
#include <autoboost/container/detail/version_type.hpp>

//move
#include <autoboost/move/adl_move_swap.hpp>
#include <autoboost/move/iterator.hpp>

//move/detail
#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <autoboost/move/detail/fwd_macros.hpp>
#endif

//std
#if !defined(AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST)
#include <initializer_list>   //for std::initializer_list
#endif

namespace autoboost {
namespace container {

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

template <class T, class Allocator = new_allocator<T> >
class small_vector_base;

#endif

//! A non-standard allocator used to implement `small_vector`.
//! Users should never use it directly. It is described here
//! for documentation purposes.
//!
//! This allocator inherits from a standard-conforming allocator
//! and forwards member functions to the standard allocator except
//! when internal storage is being used as memory source.
//!
//! This allocator is a "partially_propagable" allocator and
//! defines `is_partially_propagable` as true_type.
//!
//! A partially propagable allocator means that not all storage
//! allocatod by an instance of `small_vector_allocator` can be
//! deallocated by another instance of this type, even if both
//! instances compare equal or an instance is propagated to another
//! one using the copy/move constructor or assignment. The storage that
//! can never be propagated is identified by `storage_is_unpropagable(p)`.
//!
//! `autoboost::container::vector` supports partially propagable allocators
//! fallbacking to deep copy/swap/move operations when internal storage
//! is being used to store vector elements.
//!
//! `small_vector_allocator` assumes that will be instantiated as
//! `autoboost::container::vector< T, small_vector_allocator<Allocator> >`
//! and internal storage can be obtained downcasting that vector
//! to `small_vector_base<T>`.
template<class Allocator>
class small_vector_allocator
   : public Allocator
{
   typedef unsigned int allocation_type;
   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   private:

   AUTOBOOST_COPYABLE_AND_MOVABLE(small_vector_allocator)

   const Allocator &as_base() const
   {  return static_cast<const Allocator&>(*this);  }

   Allocator &as_base()
   {  return static_cast<Allocator&>(*this);  }

   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

   public:
   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   typedef allocator_traits<Allocator> allocator_traits_type;
   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

   typedef typename allocator_traits<Allocator>::value_type          value_type;
   typedef typename allocator_traits<Allocator>::pointer             pointer;
   typedef typename allocator_traits<Allocator>::const_pointer       const_pointer;
   typedef typename allocator_traits<Allocator>::reference           reference;
   typedef typename allocator_traits<Allocator>::const_reference     const_reference;
   typedef typename allocator_traits<Allocator>::size_type           size_type;
   typedef typename allocator_traits<Allocator>::difference_type     difference_type;
   typedef typename allocator_traits<Allocator>::void_pointer        void_pointer;
   typedef typename allocator_traits<Allocator>::const_void_pointer  const_void_pointer;

   typedef typename allocator_traits<Allocator>::propagate_on_container_copy_assignment   propagate_on_container_copy_assignment;
   typedef typename allocator_traits<Allocator>::propagate_on_container_move_assignment   propagate_on_container_move_assignment;
   typedef typename allocator_traits<Allocator>::propagate_on_container_swap              propagate_on_container_swap;
   //! An integral constant with member `value == false`
   typedef AUTOBOOST_CONTAINER_IMPDEF(container_detail::bool_<false>)                         is_always_equal;
   //! An integral constant with member `value == true`
   typedef AUTOBOOST_CONTAINER_IMPDEF(container_detail::bool_<true>)                          is_partially_propagable;

   AUTOBOOST_CONTAINER_DOCIGN(typedef container_detail::version_type<small_vector_allocator AUTOBOOST_CONTAINER_I 1>  version;)

   //!Obtains an small_vector_allocator that allocates
   //!objects of type T2
   template<class T2>
   struct rebind
   {
      typedef typename allocator_traits<Allocator>::template rebind_alloc<T2>::type other;
   };

   #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
      //!Constructor from arbitrary arguments
      template<class ...Args>
      explicit small_vector_allocator(AUTOBOOST_FWD_REF(Args) ...args)
         : Allocator(::autoboost::forward<Args>(args)...)
      {}
   #else
      #define AUTOBOOST_CONTAINER_SMALL_VECTOR_ALLOCATOR_CTOR_CODE(N) \
      AUTOBOOST_MOVE_TMPL_LT##N AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_GT##N \
      explicit small_vector_allocator(AUTOBOOST_MOVE_UREF##N)\
         : Allocator(AUTOBOOST_MOVE_FWD##N)\
      {}\
      //
      AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_CONTAINER_SMALL_VECTOR_ALLOCATOR_CTOR_CODE)
      #undef AUTOBOOST_CONTAINER_SMALL_VECTOR_ALLOCATOR_CTOR_CODE
   #endif

   //!Constructor from other small_vector_allocator.
   //!Never throws
   small_vector_allocator(const small_vector_allocator &other) AUTOBOOST_NOEXCEPT_OR_NOTHROW
      : Allocator(other.as_base())
   {}

   //!Move constructor from small_vector_allocator.
   //!Never throws
   small_vector_allocator(AUTOBOOST_RV_REF(small_vector_allocator) other) AUTOBOOST_NOEXCEPT_OR_NOTHROW
      : Allocator(::autoboost::move(other.as_base()))
   {}

   //!Constructor from related small_vector_allocator.
   //!Never throws
   template<class OtherAllocator>
   small_vector_allocator(const small_vector_allocator<OtherAllocator> &other) AUTOBOOST_NOEXCEPT_OR_NOTHROW
      : Allocator(other.as_base())
   {}

   //!Move constructor from related small_vector_allocator.
   //!Never throws
   template<class OtherAllocator>
   small_vector_allocator(AUTOBOOST_RV_REF(small_vector_allocator<OtherAllocator>) other) AUTOBOOST_NOEXCEPT_OR_NOTHROW
      : Allocator(::autoboost::move(other.as_base()))
   {}

   //!Assignment from other small_vector_allocator.
   //!Never throws
   small_vector_allocator & operator=(AUTOBOOST_COPY_ASSIGN_REF(small_vector_allocator) other) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return static_cast<small_vector_allocator&>(this->Allocator::operator=(other.as_base()));  }

   //!Move constructor from other small_vector_allocator.
   //!Never throws
   small_vector_allocator & operator=(AUTOBOOST_RV_REF(small_vector_allocator) other) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return static_cast<small_vector_allocator&>(this->Allocator::operator=(::autoboost::move(other.as_base())));  }

   //!Assignment from related small_vector_allocator.
   //!Never throws
   template<class OtherAllocator>
   small_vector_allocator & operator=(AUTOBOOST_COPY_ASSIGN_REF(small_vector_allocator<OtherAllocator>) other) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return static_cast<small_vector_allocator&>(this->Allocator::operator=(other.as_base()));  }

   //!Move assignment from related small_vector_allocator.
   //!Never throws
   template<class OtherAllocator>
   small_vector_allocator & operator=(AUTOBOOST_RV_REF(small_vector_allocator<OtherAllocator>) other) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return static_cast<small_vector_allocator&>(this->Allocator::operator=(::autoboost::move(other.as_base())));  }

   //!Allocates storage from the standard-conforming allocator
   pointer allocate(size_type count, const_void_pointer hint = const_void_pointer())
   {  return allocator_traits_type::allocate(this->as_base(), count, hint);  }

   //!Deallocates previously allocated memory.
   //!Never throws
   void deallocate(pointer ptr, size_type n) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      if(!this->is_internal_storage(ptr))
         allocator_traits_type::deallocate(this->as_base(), ptr, n);
   }

   //!Returns the maximum number of elements that could be allocated.
   //!Never throws
   size_type max_size() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return allocator_traits_type::max_size(this->as_base());   }

   small_vector_allocator select_on_container_copy_construction() const
   {  return small_vector_allocator(allocator_traits_type::select_on_container_copy_construction(this->as_base())); }

   bool storage_is_unpropagable(pointer p) const
   {  return this->is_internal_storage(p) || allocator_traits_type::storage_is_unpropagable(this->as_base(), p);  }

   //!Swaps two allocators, does nothing
   //!because this small_vector_allocator is stateless
   friend void swap(small_vector_allocator &l, small_vector_allocator &r) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  autoboost::adl_move_swap(l.as_base(), r.as_base());  }

   //!An small_vector_allocator always compares to true, as memory allocated with one
   //!instance can be deallocated by another instance (except for unpropagable storage)
   friend bool operator==(const small_vector_allocator &l, const small_vector_allocator &r) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return allocator_traits_type::equal(l.as_base(), r.as_base());  }

   //!An small_vector_allocator always compares to false, as memory allocated with one
   //!instance can be deallocated by another instance
   friend bool operator!=(const small_vector_allocator &l, const small_vector_allocator &r) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return !(l == r);   }

   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   /*
   //!An advanced function that offers in-place expansion shrink to fit and new allocation
   //!capabilities. Memory allocated with this function can only be deallocated with deallocate()
   //!or deallocate_many().
   //!This function is available only with Version == 2
   pointer allocation_command(allocation_type command,
                         size_type limit_size,
                         size_type &prefer_in_recvd_out_size,
                         pointer &reuse)
   {  return allocator_traits_type::allocation_command(command, limit_size, prefer_in_recvd_out_size, reuse);  }

   //!Returns maximum the number of objects the previously allocated memory
   //!pointed by p can hold.
   //!Memory must not have been allocated with
   //!allocate_one or allocate_individual.
   //!This function is available only with Version == 2
   size_type size(pointer p) const AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return allocator_traits_type::size(p);  }
   */
   private:
   /*
   //!Allocates just one object. Memory allocated with this function
   //!must be deallocated only with deallocate_one().
   //!Throws bad_alloc if there is no enough memory
   //!This function is available only with Version == 2
   using Allocator::allocate_one;
   using Allocator::allocate_individual;
   using Allocator::deallocate_one;
   using Allocator::deallocate_individual;
   using Allocator::allocate_many;
   using Allocator::deallocate_many;*/

   bool is_internal_storage(pointer p) const
   {  return this->internal_storage() == p;  }

   pointer internal_storage() const
   {
      typedef typename Allocator::value_type                                              value_type;
      typedef container_detail::vector_alloc_holder< small_vector_allocator<Allocator> >  vector_alloc_holder_t;
      typedef vector<value_type, small_vector_allocator<Allocator> >                      vector_base;
      typedef small_vector_base<value_type, Allocator>                                    derived_type;
      //
      const vector_alloc_holder_t &v_holder = static_cast<const vector_alloc_holder_t &>(*this);
      const vector_base &v_base = reinterpret_cast<const vector_base &>(v_holder);
      const derived_type &d_base = static_cast<const derived_type &>(v_base);
      return d_base.internal_storage();
   }
   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
};

//! This class consists of common code from all small_vector<T, N> types that don't depend on the
//! "N" template parameter. This class is non-copyable and non-destructible, so this class tipically
//! used as reference argument to functions that read or write small vectors. Since `small_vector<T, N>`
//! derives from `small_vector_base<T>`, the conversion to `small_vector_base` is implicit
//! <pre>
//!
//! //Clients can pass any small_vector<Foo, N>.
//! void read_any_small_vector_of_foo(const small_vector_base<Foo> &in_parameter);
//!
//! void modify_any_small_vector_of_foo(small_vector_base<Foo> &out_parameter);
//!
//! void some_function()
//! {
//!
//!    small_vector<Foo, 8> myvector;
//!
//!    read_any_small_vector_of_foo(myvector);   // Reads myvector
//!
//!    modify_any_small_vector_of_foo(myvector); // Modifies myvector
//!
//! }
//! </pre>
//!
//! All `autoboost::container:vector` member functions are inherited. See `vector` documentation for details.
//!
template <class T, class SecondaryAllocator>
class small_vector_base
   : public vector<T, small_vector_allocator<SecondaryAllocator> >
{
   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   public:
   //Make it public as it will be inherited by small_vector and container
   //must have this public member
   typedef typename allocator_traits<SecondaryAllocator>::pointer pointer;

   private:
   AUTOBOOST_COPYABLE_AND_MOVABLE(small_vector_base)

   friend class small_vector_allocator<SecondaryAllocator>;

   pointer internal_storage() const AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      return autoboost::intrusive::pointer_traits<pointer>::pointer_to
         (*const_cast<T*>(static_cast<const T*>(static_cast<const void*>(&m_storage_start))));
   }

   typedef vector<T, small_vector_allocator<SecondaryAllocator> > base_type;
         base_type &as_base()       { return static_cast<base_type&>(*this); }
   const base_type &as_base() const { return static_cast<const base_type&>(*this); }

   public:
   typedef typename container_detail::aligned_storage
      <sizeof(T), container_detail::alignment_of<T>::value>::type storage_type;
   typedef small_vector_allocator<SecondaryAllocator>             allocator_type;

   protected:
   typedef typename base_type::initial_capacity_t initial_capacity_t;

   explicit small_vector_base(initial_capacity_t, std::size_t initial_capacity)
      : base_type(initial_capacity_t(), this->internal_storage(), initial_capacity)
   {}

   template<class AllocFwd>
   explicit small_vector_base(initial_capacity_t, std::size_t capacity, AUTOBOOST_FWD_REF(AllocFwd) a)
      : base_type(initial_capacity_t(), this->internal_storage(), capacity, ::autoboost::forward<AllocFwd>(a))
   {}

   ~small_vector_base(){}

   using base_type::is_propagable_from;
   using base_type::steal_resources;

   private:
   //The only member
   storage_type m_storage_start;

   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

   public:
   small_vector_base& operator=(AUTOBOOST_COPY_ASSIGN_REF(small_vector_base) other)
   {  return static_cast<small_vector_base&>(this->base_type::operator=(static_cast<base_type const&>(other)));  }

   small_vector_base& operator=(AUTOBOOST_RV_REF(small_vector_base) other)
   {  return static_cast<small_vector_base&>(this->base_type::operator=(AUTOBOOST_MOVE_BASE(base_type, other))); }

   void swap(small_vector_base &other)
   {  return this->base_type::swap(other);  }
};

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

/////////////////////////////////////////////////////
//
//          small_vector_storage_calculator
//
/////////////////////////////////////////////////////
template<std::size_t Needed, std::size_t Hdr, std::size_t SSize, bool NeedsZero = (0u == Needed || Needed <= Hdr)>
struct small_vector_storage_calculator_helper
{
   static const std::size_t value = (Needed - Hdr - 1u)/SSize + 1u;
};

template<std::size_t Needed, std::size_t Hdr, std::size_t SSize>
struct small_vector_storage_calculator_helper<Needed, Hdr, SSize, true>
{
   static const std::size_t value = 0u;
};

template<class Storage, class Allocator, class T, std::size_t N>
struct small_vector_storage_calculator
{
   typedef small_vector_base<T, Allocator> svh_type;
   typedef vector<T, small_vector_allocator<Allocator> > svhb_type;
   static const std::size_t s_align = container_detail::alignment_of<Storage>::value;
   static const std::size_t s_size = sizeof(Storage);
   static const std::size_t svh_sizeof = sizeof(svh_type);
   static const std::size_t svhb_sizeof = sizeof(svhb_type);
   static const std::size_t s_start = ((svhb_sizeof-1)/s_align+1)*s_align;
   static const std::size_t header_bytes = svh_sizeof-s_start;
   static const std::size_t needed_bytes = sizeof(T)*N;
   static const std::size_t needed_extra_storages =
      small_vector_storage_calculator_helper<needed_bytes, header_bytes, s_size>::value;
};

/////////////////////////////////////////////////////
//
//          small_vector_storage_definer
//
/////////////////////////////////////////////////////
template<class Storage, std::size_t N>
struct small_vector_storage
{
   Storage m_rest_of_storage[N];
};

template<class Storage>
struct small_vector_storage<Storage, 0>
{};

template<class Allocator, std::size_t N>
struct small_vector_storage_definer
{
   typedef typename Allocator::value_type                                  value_type;
   typedef typename small_vector_base<value_type, Allocator>::storage_type storage_type;
   static const std::size_t needed_extra_storages =
      small_vector_storage_calculator<storage_type, Allocator, value_type, N>::needed_extra_storages;
   typedef small_vector_storage<storage_type, needed_extra_storages> type;
};

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

//! small_vector is a vector-like container optimized for the case when it contains few elements.
//! It contains some preallocated elements in-place, which can avoid the use of dynamic storage allocation
//! when the actual number of elements is below that preallocated threshold.
//!
//! `small_vector<T, N, Allocator>` is convertible to `small_vector_base<T, Allocator>` that is independent
//! from the preallocated element capacity, so client code does not need to be templated on that N argument.
//!
//! All `autoboost::container::vector` member functions are inherited. See `vector` documentation for details.
//!
//! \tparam T The type of object that is stored in the small_vector
//! \tparam N The number of preallocated elements stored inside small_vector. It shall be less than Allocator::max_size();
//! \tparam Allocator The allocator used for memory management when the number of elements exceeds N.
template <class T, std::size_t N, class Allocator AUTOBOOST_CONTAINER_DOCONLY(= new_allocator<T>) >
class small_vector : public small_vector_base<T, Allocator>
   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   , private small_vector_storage_definer<Allocator, N>::type
   #endif
{
   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   typedef small_vector_base<T, Allocator> base_type;
   typedef typename small_vector_storage_definer<Allocator, N>::type remaining_storage_holder;

   AUTOBOOST_COPYABLE_AND_MOVABLE(small_vector)

   typedef typename base_type::initial_capacity_t initial_capacity_t;
   typedef allocator_traits<typename base_type::allocator_type> allocator_traits_type;

   public:
   typedef small_vector_storage_calculator< typename small_vector_base<T, Allocator>
      ::storage_type, Allocator, T, N> storage_test;

   static const std::size_t needed_extra_storages =  storage_test::needed_extra_storages;
   static const std::size_t needed_bytes =  storage_test::needed_bytes;
   static const std::size_t header_bytes =  storage_test::header_bytes;
   static const std::size_t s_start =  storage_test::s_start;

   typedef typename base_type::allocator_type   allocator_type;
   typedef typename base_type::size_type        size_type;
   typedef typename base_type::value_type       value_type;

   static std::size_t internal_capacity()
   {  return (sizeof(small_vector) - storage_test::s_start)/sizeof(T);  }

   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

   public:
   small_vector()
      : base_type(initial_capacity_t(), internal_capacity())
   {}

   explicit small_vector(size_type n)
      : base_type(initial_capacity_t(), internal_capacity())
   {  this->resize(n); }

   explicit small_vector(const allocator_type &a)
      : base_type(initial_capacity_t(), internal_capacity(), a)
   {}

   small_vector(size_type n, const allocator_type &a)
      : base_type(initial_capacity_t(), internal_capacity(), a)
   {  this->resize(n); }

   small_vector(const small_vector &other)
      : base_type( initial_capacity_t(), internal_capacity()
                 , allocator_traits_type::select_on_container_copy_construction(other.get_stored_allocator()))
   {  this->assign(other.cbegin(), other.cend());  }

   small_vector(const small_vector &other, const allocator_type &a)
      : base_type(initial_capacity_t(), internal_capacity(), a)
   {  this->assign(other.cbegin(), other.cend());  }

   small_vector(AUTOBOOST_RV_REF(small_vector) other)
      : base_type(initial_capacity_t(), internal_capacity(), ::autoboost::move(other.get_stored_allocator()))
   {  this->move_construct_impl(other, other.get_stored_allocator());   }

   small_vector(AUTOBOOST_RV_REF(small_vector) other, const allocator_type &a)
      : base_type(initial_capacity_t(), internal_capacity(), a)
   {  this->move_construct_impl(other, a);   }

   #if !defined(AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST)
   small_vector(std::initializer_list<value_type> il, const allocator_type& a = allocator_type())
      : base_type(initial_capacity_t(), internal_capacity(), a)
   {
      this->assign(il.begin(), il.end());
   }
   #endif

   small_vector& operator=(AUTOBOOST_COPY_ASSIGN_REF(small_vector) other)
   {  return static_cast<small_vector&>(this->base_type::operator=(static_cast<base_type const&>(other)));  }

   small_vector& operator=(AUTOBOOST_RV_REF(small_vector) other)
   {  return static_cast<small_vector&>(this->base_type::operator=(AUTOBOOST_MOVE_BASE(base_type, other))); }

   void swap(small_vector &other)
   {  return this->base_type::swap(other);  }

   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   private:
   void move_construct_impl(small_vector &x, const allocator_type &a)
   {
      if(base_type::is_propagable_from(x.get_stored_allocator(), x.data(), a, true)){
         this->steal_resources(x);
      }
      else{
         this->assign( autoboost::make_move_iterator(container_detail::iterator_to_raw_pointer(x.begin()))
                     , autoboost::make_move_iterator(container_detail::iterator_to_raw_pointer(x.end  ()))
                     );
      }
   }
   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
};

}}

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
/*
namespace autoboost {

//!has_trivial_destructor_after_move<> == true_type
//!specialization for optimizations
template <class T, class Allocator>
struct has_trivial_destructor_after_move<autoboost::container::vector<T, Allocator> >
{
   typedef typename ::autoboost::container::allocator_traits<Allocator>::pointer pointer;
   static const bool value = ::autoboost::has_trivial_destructor_after_move<Allocator>::value &&
                             ::autoboost::has_trivial_destructor_after_move<pointer>::value;
};

}
*/
#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

#include <autoboost/container/detail/config_end.hpp>

#endif //   #ifndef  AUTOBOOST_CONTAINER_CONTAINER_SMALL_VECTOR_HPP

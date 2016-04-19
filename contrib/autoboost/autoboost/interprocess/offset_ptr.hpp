//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_OFFSET_PTR_HPP
#define AUTOBOOST_INTERPROCESS_OFFSET_PTR_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/interprocess/detail/workaround.hpp>

#include <autoboost/interprocess/interprocess_fwd.hpp>
#include <autoboost/interprocess/detail/utilities.hpp>
#include <autoboost/interprocess/detail/cast_tags.hpp>
#include <autoboost/interprocess/detail/mpl.hpp>
#include <autoboost/container/detail/type_traits.hpp>  //alignment_of, aligned_storage
#include <autoboost/assert.hpp>
#include <iosfwd>

//!\file
//!Describes a smart pointer that stores the offset between this pointer and
//!target pointee, called offset_ptr.

namespace autoboost {

#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

//Predeclarations
template <class T>
struct has_trivial_destructor;

#endif   //#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

namespace interprocess {

#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
namespace ipcdetail {

   template<class OffsetType, std::size_t OffsetAlignment>
   union offset_ptr_internal
   {
      AUTOBOOST_STATIC_ASSERT(sizeof(OffsetType) >= sizeof(uintptr_t));

      explicit offset_ptr_internal(OffsetType off)
         : m_offset(off)
      {}

      OffsetType m_offset; //Distance between this object and pointee address

      typename ::autoboost::container::container_detail::aligned_storage
         < sizeof(OffsetType)//for offset_type_alignment m_offset will be enough
         , (OffsetAlignment == offset_type_alignment) ? 1u : OffsetAlignment
         >::type alignment_helper;
   };

   //Note: using the address of a local variable to point to another address
   //is not standard conforming and this can be optimized-away by the compiler.
   //Non-inlining is a method to remain illegal but correct

   //Undef AUTOBOOST_INTERPROCESS_OFFSET_PTR_INLINE_XXX if your compiler can inline
   //this code without breaking the library

   ////////////////////////////////////////////////////////////////////////
   //
   //                      offset_ptr_to_raw_pointer
   //
   ////////////////////////////////////////////////////////////////////////
   #define AUTOBOOST_INTERPROCESS_OFFSET_PTR_BRANCHLESS_TO_PTR
   AUTOBOOST_FORCEINLINE void * offset_ptr_to_raw_pointer(const volatile void *this_ptr, uintptr_t offset)
   {
      typedef pointer_uintptr_caster<void*> caster_t;
      #ifndef AUTOBOOST_INTERPROCESS_OFFSET_PTR_BRANCHLESS_TO_PTR
         if(offset == 1){
            return 0;
         }
         else{
            return caster_t(caster_t(this_ptr).uintptr() + offset).pointer();
         }
      #else
         uintptr_t mask = offset == 1;
         --mask;
         uintptr_t target_offset = caster_t(this_ptr).uintptr() + offset;
         target_offset &= mask;
         return caster_t(target_offset).pointer();
      #endif
   }

   ////////////////////////////////////////////////////////////////////////
   //
   //                      offset_ptr_to_offset
   //
   ////////////////////////////////////////////////////////////////////////
   #define AUTOBOOST_INTERPROCESS_OFFSET_PTR_BRANCHLESS_TO_OFF
   AUTOBOOST_FORCEINLINE uintptr_t offset_ptr_to_offset(const volatile void *ptr, const volatile void *this_ptr)
   {
      typedef pointer_uintptr_caster<void*> caster_t;
      #ifndef AUTOBOOST_INTERPROCESS_OFFSET_PTR_BRANCHLESS_TO_OFF
         //offset == 1 && ptr != 0 is not legal for this pointer
         if(!ptr){
            return 1;
         }
         else{
            uintptr_t offset = caster_t(ptr).uintptr() - caster_t(this_ptr).uintptr();
            AUTOBOOST_ASSERT(offset != 1);
            return offset;
         }
      #else
         //const uintptr_t other = -uintptr_t(ptr != 0);
         //const uintptr_t offset = (caster_t(ptr).uintptr() - caster_t(this_ptr).uintptr()) & other;
         //return offset + uintptr_t(!other);
         //
         uintptr_t offset = caster_t(ptr).uintptr() - caster_t(this_ptr).uintptr();
         --offset;
         uintptr_t mask = uintptr_t(ptr == 0);
         --mask;
         offset &= mask;
         return ++offset;
      #endif
   }

   ////////////////////////////////////////////////////////////////////////
   //
   //                      offset_ptr_to_offset_from_other
   //
   ////////////////////////////////////////////////////////////////////////
   #define AUTOBOOST_INTERPROCESS_OFFSET_PTR_BRANCHLESS_TO_OFF_FROM_OTHER
   AUTOBOOST_FORCEINLINE uintptr_t offset_ptr_to_offset_from_other
      (const volatile void *this_ptr, const volatile void *other_ptr, uintptr_t other_offset)
   {
      typedef pointer_uintptr_caster<void*> caster_t;
      #ifndef AUTOBOOST_INTERPROCESS_OFFSET_PTR_BRANCHLESS_TO_OFF_FROM_OTHER
      if(other_offset == 1){
         return 1;
      }
      else{
         uintptr_t offset = caster_t(other_ptr).uintptr() - caster_t(this_ptr).uintptr() + other_offset;
         AUTOBOOST_ASSERT(offset != 1);
         return offset;
      }
      #else
      uintptr_t mask = other_offset == 1;
      --mask;
      uintptr_t offset = caster_t(other_ptr).uintptr() - caster_t(this_ptr).uintptr();
      offset &= mask;
      return offset + other_offset;

      //uintptr_t mask = -uintptr_t(other_offset != 1);
      //uintptr_t offset = caster_t(other_ptr).uintptr() - caster_t(this_ptr).uintptr();
      //offset &= mask;
      //return offset + other_offset;
      #endif
   }

   ////////////////////////////////////////////////////////////////////////
   //
   // Let's assume cast to void and cv cast don't change any target address
   //
   ////////////////////////////////////////////////////////////////////////
   template<class From, class To>
   struct offset_ptr_maintains_address
   {
      static const bool value =    ipcdetail::is_cv_same<From, To>::value
                                || ipcdetail::is_cv_same<void, To>::value
                                || ipcdetail::is_cv_same<char, To>::value
                                ;
   };

   template<class From, class To, class Ret = void>
   struct enable_if_convertible_equal_address
      : enable_if_c< is_convertible<From*, To*>::value
                     && offset_ptr_maintains_address<From, To>::value
                  , Ret>
   {};

   template<class From, class To, class Ret = void>
   struct enable_if_convertible_unequal_address
      : enable_if_c< is_convertible<From*, To*>::value
                     && !offset_ptr_maintains_address<From, To>::value
                   , Ret>
   {};

}  //namespace ipcdetail {
#endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

//!A smart pointer that stores the offset between between the pointer and the
//!the object it points. This allows offset allows special properties, since
//!the pointer is independent from the address of the pointee, if the
//!pointer and the pointee are still separated by the same offset. This feature
//!converts offset_ptr in a smart pointer that can be placed in shared memory and
//!memory mapped files mapped in different addresses in every process.
//!
//! \tparam PointedType The type of the pointee.
//! \tparam DifferenceType A signed integer type that can represent the arithmetic operations on the pointer
//! \tparam OffsetType An unsigned integer type that can represent the
//!   distance between two pointers reinterpret_cast-ed as unsigned integers. This type
//!   should be at least of the same size of std::uintptr_t. In some systems it's possible to communicate
//!   between 32 and 64 bit processes using 64 bit offsets.
//! \tparam OffsetAlignment Alignment of the OffsetType stored inside. In some systems might be necessary
//!   to align it to 64 bits in order to communicate 32 and 64 bit processes using 64 bit offsets.
//!
//!<b>Note</b>: offset_ptr uses implementation defined properties, present in most platforms, for
//!performance reasons:
//!   - Assumes that uintptr_t representation of nullptr is (uintptr_t)zero.
//!   - Assumes that incrementing a uintptr_t obtained from a pointer is equivalent
//!     to incrementing the pointer and then converting it back to uintptr_t.
template <class PointedType, class DifferenceType, class OffsetType, std::size_t OffsetAlignment>
class offset_ptr
{
   #if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
   typedef offset_ptr<PointedType, DifferenceType, OffsetType, OffsetAlignment>   self_t;
   void unspecified_bool_type_func() const {}
   typedef void (self_t::*unspecified_bool_type)() const;
   #endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

   public:
   typedef PointedType                       element_type;
   typedef PointedType *                     pointer;
   typedef typename ipcdetail::
      add_reference<PointedType>::type       reference;
   typedef typename ipcdetail::
      remove_volatile<typename ipcdetail::
         remove_const<PointedType>::type
            >::type                          value_type;
   typedef DifferenceType                    difference_type;
   typedef std::random_access_iterator_tag   iterator_category;
   typedef OffsetType                        offset_type;

   public:   //Public Functions

   //!Default constructor (null pointer).
   //!Never throws.
   AUTOBOOST_FORCEINLINE offset_ptr() AUTOBOOST_NOEXCEPT
      : internal(1)
   {}

   //!Constructor from raw pointer (allows "0" pointer conversion).
   //!Never throws.
   AUTOBOOST_FORCEINLINE offset_ptr(pointer ptr) AUTOBOOST_NOEXCEPT
      : internal(static_cast<OffsetType>(ipcdetail::offset_ptr_to_offset(ptr, this)))
   {}

   //!Constructor from other pointer.
   //!Never throws.
   template <class T>
   AUTOBOOST_FORCEINLINE offset_ptr( T *ptr
             , typename ipcdetail::enable_if< ipcdetail::is_convertible<T*, PointedType*> >::type * = 0) AUTOBOOST_NOEXCEPT
      : internal(static_cast<OffsetType>
         (ipcdetail::offset_ptr_to_offset(static_cast<PointedType*>(ptr), this)))
   {}

   //!Constructor from other offset_ptr
   //!Never throws.
   AUTOBOOST_FORCEINLINE offset_ptr(const offset_ptr& ptr) AUTOBOOST_NOEXCEPT
      : internal(static_cast<OffsetType>
         (ipcdetail::offset_ptr_to_offset_from_other(this, &ptr, ptr.internal.m_offset)))
   {}

   //!Constructor from other offset_ptr. If pointers of pointee types are
   //!convertible, offset_ptrs will be convertibles. Never throws.
   template<class T2>
   AUTOBOOST_FORCEINLINE offset_ptr( const offset_ptr<T2, DifferenceType, OffsetType, OffsetAlignment> &ptr
             #ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED
             , typename ipcdetail::enable_if_convertible_equal_address<T2, PointedType>::type* = 0
             #endif
             ) AUTOBOOST_NOEXCEPT
      : internal(static_cast<OffsetType>
         (ipcdetail::offset_ptr_to_offset_from_other(this, &ptr, ptr.get_offset())))
   {}

   #ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

   //!Constructor from other offset_ptr. If pointers of pointee types are
   //!convertible, offset_ptrs will be convertibles. Never throws.
   template<class T2>
   AUTOBOOST_FORCEINLINE offset_ptr( const offset_ptr<T2, DifferenceType, OffsetType, OffsetAlignment> &ptr
             , typename ipcdetail::enable_if_convertible_unequal_address<T2, PointedType>::type* = 0) AUTOBOOST_NOEXCEPT
      : internal(static_cast<OffsetType>
         (ipcdetail::offset_ptr_to_offset(static_cast<PointedType*>(ptr.get()), this)))
   {}

   #endif

   //!Emulates static_cast operator.
   //!Never throws.
   template<class T2, class P2, class O2, std::size_t A2>
   AUTOBOOST_FORCEINLINE offset_ptr(const offset_ptr<T2, P2, O2, A2> & r, ipcdetail::static_cast_tag) AUTOBOOST_NOEXCEPT
      : internal(static_cast<OffsetType>
         (ipcdetail::offset_ptr_to_offset(static_cast<PointedType*>(r.get()), this)))
   {}

   //!Emulates const_cast operator.
   //!Never throws.
   template<class T2, class P2, class O2, std::size_t A2>
   AUTOBOOST_FORCEINLINE offset_ptr(const offset_ptr<T2, P2, O2, A2> & r, ipcdetail::const_cast_tag) AUTOBOOST_NOEXCEPT
      : internal(static_cast<OffsetType>
         (ipcdetail::offset_ptr_to_offset(const_cast<PointedType*>(r.get()), this)))
   {}

   //!Emulates dynamic_cast operator.
   //!Never throws.
   template<class T2, class P2, class O2, std::size_t A2>
   AUTOBOOST_FORCEINLINE offset_ptr(const offset_ptr<T2, P2, O2, A2> & r, ipcdetail::dynamic_cast_tag) AUTOBOOST_NOEXCEPT
      : internal(static_cast<OffsetType>
         (ipcdetail::offset_ptr_to_offset(dynamic_cast<PointedType*>(r.get()), this)))
   {}

   //!Emulates reinterpret_cast operator.
   //!Never throws.
   template<class T2, class P2, class O2, std::size_t A2>
   AUTOBOOST_FORCEINLINE offset_ptr(const offset_ptr<T2, P2, O2, A2> & r, ipcdetail::reinterpret_cast_tag) AUTOBOOST_NOEXCEPT
      : internal(static_cast<OffsetType>
      (ipcdetail::offset_ptr_to_offset(reinterpret_cast<PointedType*>(r.get()), this)))
   {}

   //!Obtains raw pointer from offset.
   //!Never throws.
   AUTOBOOST_FORCEINLINE pointer get() const AUTOBOOST_NOEXCEPT
   {  return (pointer)ipcdetail::offset_ptr_to_raw_pointer(this, this->internal.m_offset);   }

   AUTOBOOST_FORCEINLINE offset_type get_offset() const AUTOBOOST_NOEXCEPT
   {  return this->internal.m_offset;  }

   //!Pointer-like -> operator. It can return 0 pointer.
   //!Never throws.
   AUTOBOOST_FORCEINLINE pointer operator->() const AUTOBOOST_NOEXCEPT
   {  return this->get(); }

   //!Dereferencing operator, if it is a null offset_ptr behavior
   //!   is undefined. Never throws.
   AUTOBOOST_FORCEINLINE reference operator* () const AUTOBOOST_NOEXCEPT
   {
      pointer p = this->get();
      reference r = *p;
      return r;
   }

   //!Indexing operator.
   //!Never throws.
   AUTOBOOST_FORCEINLINE reference operator[](difference_type idx) const AUTOBOOST_NOEXCEPT
   {  return this->get()[idx];  }

   //!Assignment from pointer (saves extra conversion).
   //!Never throws.
   AUTOBOOST_FORCEINLINE offset_ptr& operator= (pointer from) AUTOBOOST_NOEXCEPT
   {
      this->internal.m_offset =
         static_cast<OffsetType>(ipcdetail::offset_ptr_to_offset(from, this));
      return *this;
   }

   //!Assignment from other offset_ptr.
   //!Never throws.
   AUTOBOOST_FORCEINLINE offset_ptr& operator= (const offset_ptr & ptr) AUTOBOOST_NOEXCEPT
   {
      this->internal.m_offset =
         static_cast<OffsetType>(ipcdetail::offset_ptr_to_offset_from_other(this, &ptr, ptr.internal.m_offset));
      return *this;
   }

   //!Assignment from related offset_ptr. If pointers of pointee types
   //!   are assignable, offset_ptrs will be assignable. Never throws.
   template<class T2> AUTOBOOST_FORCEINLINE
   #ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED
   typename ipcdetail::enable_if_c
      < ipcdetail::is_convertible<T2*, PointedType*>::value, offset_ptr&>::type
   #else
   offset_ptr&
   #endif
      operator= (const offset_ptr<T2, DifferenceType, OffsetType, OffsetAlignment> &ptr) AUTOBOOST_NOEXCEPT
   {
      this->assign(ptr, ipcdetail::bool_<ipcdetail::offset_ptr_maintains_address<T2, PointedType>::value>());
      return *this;
   }

   public:

   //!offset_ptr += difference_type.
   //!Never throws.
   AUTOBOOST_FORCEINLINE offset_ptr &operator+= (difference_type offset) AUTOBOOST_NOEXCEPT
   {  this->inc_offset(offset * sizeof (PointedType));   return *this;  }

   //!offset_ptr -= difference_type.
   //!Never throws.
   AUTOBOOST_FORCEINLINE offset_ptr &operator-= (difference_type offset) AUTOBOOST_NOEXCEPT
   {  this->dec_offset(offset * sizeof (PointedType));   return *this;  }

   //!++offset_ptr.
   //!Never throws.
   AUTOBOOST_FORCEINLINE offset_ptr& operator++ (void) AUTOBOOST_NOEXCEPT
   {  this->inc_offset(sizeof (PointedType));   return *this;  }

   //!offset_ptr++.
   //!Never throws.
   AUTOBOOST_FORCEINLINE offset_ptr operator++ (int) AUTOBOOST_NOEXCEPT
   {
      offset_ptr tmp(*this);
      this->inc_offset(sizeof (PointedType));
      return tmp;
   }

   //!--offset_ptr.
   //!Never throws.
   AUTOBOOST_FORCEINLINE offset_ptr& operator-- (void) AUTOBOOST_NOEXCEPT
   {  this->dec_offset(sizeof (PointedType));   return *this;  }

   //!offset_ptr--.
   //!Never throws.
   AUTOBOOST_FORCEINLINE offset_ptr operator-- (int) AUTOBOOST_NOEXCEPT
   {
      offset_ptr tmp(*this);
      this->dec_offset(sizeof (PointedType));
      return tmp;
   }

   //!safe bool conversion operator.
   //!Never throws.
   #if defined(AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
   AUTOBOOST_FORCEINLINE operator unspecified_bool_type() const AUTOBOOST_NOEXCEPT
   {  return this->internal.m_offset != 1? &self_t::unspecified_bool_type_func : 0;   }
   #else
   explicit operator bool() const AUTOBOOST_NOEXCEPT
   {  return this->internal.m_offset != 1;  }
   #endif

   //!Not operator. Not needed in theory, but improves portability.
   //!Never throws
   AUTOBOOST_FORCEINLINE bool operator! () const AUTOBOOST_NOEXCEPT
   {  return this->internal.m_offset == 1;   }

   //!Compatibility with pointer_traits
   //!
   template <class U>
   struct rebind
   {  typedef offset_ptr<U, DifferenceType, OffsetType, OffsetAlignment> other;  };

   //!Compatibility with pointer_traits
   //!
   AUTOBOOST_FORCEINLINE static offset_ptr pointer_to(reference r) AUTOBOOST_NOEXCEPT
   { return offset_ptr(&r); }

   //!difference_type + offset_ptr
   //!operation
   AUTOBOOST_FORCEINLINE friend offset_ptr operator+(difference_type diff, offset_ptr right) AUTOBOOST_NOEXCEPT
   {  right += diff;  return right;  }

   //!offset_ptr + difference_type
   //!operation
   AUTOBOOST_FORCEINLINE friend offset_ptr operator+(offset_ptr left, difference_type diff) AUTOBOOST_NOEXCEPT
   {  left += diff;  return left; }

   //!offset_ptr - diff
   //!operation
   AUTOBOOST_FORCEINLINE friend offset_ptr operator-(offset_ptr left, difference_type diff) AUTOBOOST_NOEXCEPT
   {  left -= diff;  return left; }

   //!offset_ptr - diff
   //!operation
   AUTOBOOST_FORCEINLINE friend offset_ptr operator-(difference_type diff, offset_ptr right) AUTOBOOST_NOEXCEPT
   {  right -= diff; return right; }

   //!offset_ptr - offset_ptr
   //!operation
   AUTOBOOST_FORCEINLINE friend difference_type operator-(const offset_ptr &pt, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return difference_type(pt.get()- pt2.get());   }

   //Comparison
   AUTOBOOST_FORCEINLINE friend bool operator== (const offset_ptr &pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() == pt2.get();  }

   AUTOBOOST_FORCEINLINE friend bool operator!= (const offset_ptr &pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() != pt2.get();  }

   AUTOBOOST_FORCEINLINE friend bool operator<(const offset_ptr &pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() < pt2.get();  }

   AUTOBOOST_FORCEINLINE friend bool operator<=(const offset_ptr &pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() <= pt2.get();  }

   AUTOBOOST_FORCEINLINE friend bool operator>(const offset_ptr &pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() > pt2.get();  }

   AUTOBOOST_FORCEINLINE friend bool operator>=(const offset_ptr &pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() >= pt2.get();  }

   //Comparison to raw ptr to support literal 0
   AUTOBOOST_FORCEINLINE friend bool operator== (pointer pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1 == pt2.get();  }

   AUTOBOOST_FORCEINLINE friend bool operator!= (pointer pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1 != pt2.get();  }

   AUTOBOOST_FORCEINLINE friend bool operator<(pointer pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1 < pt2.get();  }

   AUTOBOOST_FORCEINLINE friend bool operator<=(pointer pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1 <= pt2.get();  }

   AUTOBOOST_FORCEINLINE friend bool operator>(pointer pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1 > pt2.get();  }

   AUTOBOOST_FORCEINLINE friend bool operator>=(pointer pt1, const offset_ptr &pt2) AUTOBOOST_NOEXCEPT
   {  return pt1 >= pt2.get();  }

   //Comparison
   AUTOBOOST_FORCEINLINE friend bool operator== (const offset_ptr &pt1, pointer pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() == pt2;  }

   AUTOBOOST_FORCEINLINE friend bool operator!= (const offset_ptr &pt1, pointer pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() != pt2;  }

   AUTOBOOST_FORCEINLINE friend bool operator<(const offset_ptr &pt1, pointer pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() < pt2;  }

   AUTOBOOST_FORCEINLINE friend bool operator<=(const offset_ptr &pt1, pointer pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() <= pt2;  }

   AUTOBOOST_FORCEINLINE friend bool operator>(const offset_ptr &pt1, pointer pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() > pt2;  }

   AUTOBOOST_FORCEINLINE friend bool operator>=(const offset_ptr &pt1, pointer pt2) AUTOBOOST_NOEXCEPT
   {  return pt1.get() >= pt2;  }

   AUTOBOOST_FORCEINLINE friend void swap(offset_ptr &left, offset_ptr &right) AUTOBOOST_NOEXCEPT
   {
      pointer ptr = right.get();
      right = left;
      left = ptr;
   }

   private:
   template<class T2>
   AUTOBOOST_FORCEINLINE void assign(const offset_ptr<T2, DifferenceType, OffsetType, OffsetAlignment> &ptr, ipcdetail::bool_<true>) AUTOBOOST_NOEXCEPT
   {  //no need to pointer adjustment
      this->internal.m_offset =
         static_cast<OffsetType>(ipcdetail::offset_ptr_to_offset_from_other(this, &ptr, ptr.get_offset()));
   }

   template<class T2>
   AUTOBOOST_FORCEINLINE void assign(const offset_ptr<T2, DifferenceType, OffsetType, OffsetAlignment> &ptr, ipcdetail::bool_<false>) AUTOBOOST_NOEXCEPT
   {  //we must convert to raw before calculating the offset
      this->internal.m_offset =
         static_cast<OffsetType>(ipcdetail::offset_ptr_to_offset(static_cast<PointedType*>(ptr.get()), this));
   }

   #if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)
   AUTOBOOST_FORCEINLINE void inc_offset(DifferenceType bytes) AUTOBOOST_NOEXCEPT
   {  internal.m_offset += bytes;   }

   AUTOBOOST_FORCEINLINE void dec_offset(DifferenceType bytes) AUTOBOOST_NOEXCEPT
   {  internal.m_offset -= bytes;   }

   ipcdetail::offset_ptr_internal<OffsetType, OffsetAlignment> internal;

   public:
   AUTOBOOST_FORCEINLINE const OffsetType &priv_offset() const AUTOBOOST_NOEXCEPT
   {  return internal.m_offset;   }

   AUTOBOOST_FORCEINLINE       OffsetType &priv_offset() AUTOBOOST_NOEXCEPT
   {  return internal.m_offset;   }

   #endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED
};

//!operator<<
//!for offset ptr
template<class E, class T, class W, class X, class Y, std::size_t Z>
inline std::basic_ostream<E, T> & operator<<
   (std::basic_ostream<E, T> & os, offset_ptr<W, X, Y, Z> const & p)
{  return os << p.get_offset();   }

//!operator>>
//!for offset ptr
template<class E, class T, class W, class X, class Y, std::size_t Z>
inline std::basic_istream<E, T> & operator>>
   (std::basic_istream<E, T> & is, offset_ptr<W, X, Y, Z> & p)
{  return is >> p.get_offset();  }

//!Simulation of static_cast between pointers. Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
AUTOBOOST_FORCEINLINE autoboost::interprocess::offset_ptr<T1, P1, O1, A1>
   static_pointer_cast(const autoboost::interprocess::offset_ptr<T2, P2, O2, A2> & r) AUTOBOOST_NOEXCEPT
{
   return autoboost::interprocess::offset_ptr<T1, P1, O1, A1>
            (r, autoboost::interprocess::ipcdetail::static_cast_tag());
}

//!Simulation of const_cast between pointers. Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
AUTOBOOST_FORCEINLINE autoboost::interprocess::offset_ptr<T1, P1, O1, A1>
   const_pointer_cast(const autoboost::interprocess::offset_ptr<T2, P2, O2, A2> & r) AUTOBOOST_NOEXCEPT
{
   return autoboost::interprocess::offset_ptr<T1, P1, O1, A1>
            (r, autoboost::interprocess::ipcdetail::const_cast_tag());
}

//!Simulation of dynamic_cast between pointers. Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
AUTOBOOST_FORCEINLINE autoboost::interprocess::offset_ptr<T1, P1, O1, A1>
   dynamic_pointer_cast(const autoboost::interprocess::offset_ptr<T2, P2, O2, A2> & r) AUTOBOOST_NOEXCEPT
{
   return autoboost::interprocess::offset_ptr<T1, P1, O1, A1>
            (r, autoboost::interprocess::ipcdetail::dynamic_cast_tag());
}

//!Simulation of reinterpret_cast between pointers. Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
AUTOBOOST_FORCEINLINE autoboost::interprocess::offset_ptr<T1, P1, O1, A1>
   reinterpret_pointer_cast(const autoboost::interprocess::offset_ptr<T2, P2, O2, A2> & r) AUTOBOOST_NOEXCEPT
{
   return autoboost::interprocess::offset_ptr<T1, P1, O1, A1>
            (r, autoboost::interprocess::ipcdetail::reinterpret_cast_tag());
}

}  //namespace interprocess {

#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

///has_trivial_destructor<> == true_type specialization for optimizations
template <class T, class P, class O, std::size_t A>
struct has_trivial_destructor< ::autoboost::interprocess::offset_ptr<T, P, O, A> >
{
   static const bool value = true;
};

namespace move_detail {

///has_trivial_destructor<> == true_type specialization for optimizations
template <class T, class P, class O, std::size_t A>
struct is_trivially_destructible< ::autoboost::interprocess::offset_ptr<T, P, O, A> >
{
   static const bool value = true;
};

}  //namespace move_detail {

namespace interprocess {

//!to_raw_pointer() enables autoboost::mem_fn to recognize offset_ptr.
//!Never throws.
template <class T, class P, class O, std::size_t A>
AUTOBOOST_FORCEINLINE T * to_raw_pointer(autoboost::interprocess::offset_ptr<T, P, O, A> const & p) AUTOBOOST_NOEXCEPT
{  return ipcdetail::to_raw_pointer(p);   }

}  //namespace interprocess


#endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED
}  //namespace autoboost {

#if !defined(AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED)

namespace autoboost{

//This is to support embedding a bit in the pointer
//for intrusive containers, saving space
namespace intrusive {

//Predeclaration to avoid including header
template<class VoidPointer, std::size_t N>
struct max_pointer_plus_bits;

template<std::size_t OffsetAlignment, class P, class O, std::size_t A>
struct max_pointer_plus_bits<autoboost::interprocess::offset_ptr<void, P, O, A>, OffsetAlignment>
{
   //The offset ptr can embed one bit less than the alignment since it
   //uses offset == 1 to store the null pointer.
   static const std::size_t value = ::autoboost::interprocess::ipcdetail::ls_zeros<OffsetAlignment>::value - 1;
};

//Predeclaration
template<class Pointer, std::size_t NumBits>
struct pointer_plus_bits;

template<class T, class P, class O, std::size_t A, std::size_t NumBits>
struct pointer_plus_bits<autoboost::interprocess::offset_ptr<T, P, O, A>, NumBits>
{
   typedef autoboost::interprocess::offset_ptr<T, P, O, A>      pointer;
   //Bits are stored in the lower bits of the pointer except the LSB,
   //because this bit is used to represent the null pointer.
   static const uintptr_t Mask = ((uintptr_t(1) << uintptr_t(NumBits)) - uintptr_t(1)) << uintptr_t(1);
   AUTOBOOST_STATIC_ASSERT(0 ==(Mask&1));

   //We must ALWAYS take argument "n" by reference as a copy of a null pointer
   //with a bit (e.g. offset == 3) would be incorrectly copied and interpreted as non-null.

   AUTOBOOST_FORCEINLINE static pointer get_pointer(const pointer &n) AUTOBOOST_NOEXCEPT
   {
      pointer p;
      O const tmp_off = n.priv_offset() & O(~Mask);
      p.priv_offset() = autoboost::interprocess::ipcdetail::offset_ptr_to_offset_from_other(&p, &n, tmp_off);
      return p;
   }

   AUTOBOOST_FORCEINLINE static void set_pointer(pointer &n, const pointer &p) AUTOBOOST_NOEXCEPT
   {
      AUTOBOOST_ASSERT(0 == (get_bits)(p));
      O const stored_bits = O(n.priv_offset() & Mask);
      n = p;
      n.priv_offset() |= stored_bits;
   }

   AUTOBOOST_FORCEINLINE static std::size_t get_bits(const pointer &n) AUTOBOOST_NOEXCEPT
   {
      return std::size_t((n.priv_offset() & Mask) >> 1u);
   }

   AUTOBOOST_FORCEINLINE static void set_bits(pointer &n, std::size_t const b) AUTOBOOST_NOEXCEPT
   {
      AUTOBOOST_ASSERT(b < (std::size_t(1) << NumBits));
      O tmp = n.priv_offset();
      tmp &= O(~Mask);
      tmp |= O(b << 1u);
      n.priv_offset() = tmp;
   }
};

}  //namespace intrusive

//Predeclaration
template<class T, class U>
struct pointer_to_other;

//Backwards compatibility with pointer_to_other
template <class PointedType, class DifferenceType, class OffsetType, std::size_t OffsetAlignment, class U>
struct pointer_to_other
   < ::autoboost::interprocess::offset_ptr<PointedType, DifferenceType, OffsetType, OffsetAlignment>, U >
{
   typedef ::autoboost::interprocess::offset_ptr<U, DifferenceType, OffsetType, OffsetAlignment> type;
};

}  //namespace autoboost{
#endif   //#ifndef AUTOBOOST_INTERPROCESS_DOXYGEN_INVOKED

#include <autoboost/interprocess/detail/config_end.hpp>

#endif //#ifndef AUTOBOOST_INTERPROCESS_OFFSET_PTR_HPP

//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2015.
// (C) Copyright Gennaro Prota 2003 - 2004.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_DETAIL_UTILITIES_HPP
#define AUTOBOOST_INTERPROCESS_DETAIL_UTILITIES_HPP

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
#include <autoboost/move/utility_core.hpp>
#include <autoboost/interprocess/detail/min_max.hpp>
#include <autoboost/interprocess/detail/type_traits.hpp>
#include <autoboost/interprocess/detail/mpl.hpp>
#include <autoboost/intrusive/pointer_traits.hpp>
#include <autoboost/move/utility_core.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/cstdint.hpp>
#include <climits>

namespace autoboost {
namespace interprocess {
namespace ipcdetail {

template <class T>
inline T* to_raw_pointer(T* p)
{  return p; }

template <class Pointer>
inline typename autoboost::intrusive::pointer_traits<Pointer>::element_type*
to_raw_pointer(const Pointer &p)
{  return autoboost::interprocess::ipcdetail::to_raw_pointer(p.operator->());  }

//Rounds "orig_size" by excess to round_to bytes
template<class SizeType>
inline SizeType get_rounded_size(SizeType orig_size, SizeType round_to)
{
   return ((orig_size-1)/round_to+1)*round_to;
}

//Truncates "orig_size" to a multiple of "multiple" bytes.
template<class SizeType>
inline SizeType get_truncated_size(SizeType orig_size, SizeType multiple)
{
   return orig_size/multiple*multiple;
}

//Rounds "orig_size" by excess to round_to bytes. round_to must be power of two
template<class SizeType>
inline SizeType get_rounded_size_po2(SizeType orig_size, SizeType round_to)
{
   return ((orig_size-1)&(~(round_to-1))) + round_to;
}

//Truncates "orig_size" to a multiple of "multiple" bytes. multiple must be power of two
template<class SizeType>
inline SizeType get_truncated_size_po2(SizeType orig_size, SizeType multiple)
{
   return (orig_size & (~(multiple-1)));
}

template <std::size_t OrigSize, std::size_t RoundTo>
struct ct_rounded_size
{
   AUTOBOOST_STATIC_ASSERT((RoundTo != 0));
   static const std::size_t intermediate_value = (OrigSize-1)/RoundTo+1;
   AUTOBOOST_STATIC_ASSERT(intermediate_value <= std::size_t(-1)/RoundTo);
   static const std::size_t value = intermediate_value*RoundTo;
};

// Gennaro Prota wrote this. Thanks!
template <int p, int n = 4>
struct ct_max_pow2_less
{
   static const std::size_t c = 2*n < p;

   static const std::size_t value =
         c ? (ct_max_pow2_less< c*p, 2*c*n>::value) : n;
};

template <>
struct ct_max_pow2_less<0, 0>
{
   static const std::size_t value = 0;
};

}  //namespace ipcdetail {

//!Trait class to detect if an index is a node
//!index. This allows more efficient operations
//!when deallocating named objects.
template <class Index>
struct is_node_index
{
   static const bool value = false;
};

//!Trait class to detect if an index is an intrusive
//!index. This will embed the derivation hook in each
//!allocation header, to provide memory for the intrusive
//!container.
template <class Index>
struct is_intrusive_index
{
   static const bool value = false;
};

template <typename T>
AUTOBOOST_INTERPROCESS_FORCEINLINE T* addressof(T& v)
{
  return reinterpret_cast<T*>(
       &const_cast<char&>(reinterpret_cast<const volatile char &>(v)));
}

template<class SizeType>
struct sqrt_size_type_max
{
   static const SizeType value = (SizeType(1) << (sizeof(SizeType)*(CHAR_BIT/2)))-1;
};

template<class SizeType>
inline bool multiplication_overflows(SizeType a, SizeType b)
{
   const SizeType sqrt_size_max = sqrt_size_type_max<SizeType>::value;
   return   //Fast runtime check
         (  (a | b) > sqrt_size_max &&
            //Slow division check
            b && a > SizeType(-1)/b
         );
}

template<std::size_t SztSizeOfType, class SizeType>
AUTOBOOST_INTERPROCESS_FORCEINLINE bool size_overflows(SizeType count)
{
   //Compile time-check
   AUTOBOOST_STATIC_ASSERT(SztSizeOfType <= SizeType(-1));
   //Runtime check
   return multiplication_overflows(SizeType(SztSizeOfType), count);
}

template<class RawPointer, class OffsetType>
class pointer_offset_caster;

template<class T, class OffsetType>
class pointer_offset_caster<T*, OffsetType>
{
   public:
   AUTOBOOST_INTERPROCESS_FORCEINLINE explicit pointer_offset_caster(OffsetType offset)
      : m_offset(offset)
   {}

   AUTOBOOST_INTERPROCESS_FORCEINLINE explicit pointer_offset_caster(const volatile T *p)
      : m_offset(reinterpret_cast<OffsetType>(p))
   {}

   AUTOBOOST_INTERPROCESS_FORCEINLINE OffsetType offset() const
   {   return m_offset;   }

   AUTOBOOST_INTERPROCESS_FORCEINLINE T* pointer() const
   {   return reinterpret_cast<T*>(m_offset);   }

   private:
   OffsetType m_offset;
};


template<class SizeType>
inline bool sum_overflows(SizeType a, SizeType b)
{  return SizeType(-1) - a < b;  }

//Anti-exception node eraser
template<class Cont>
class value_eraser
{
   public:
   value_eraser(Cont & cont, typename Cont::iterator it)
      : m_cont(cont), m_index_it(it), m_erase(true){}
   ~value_eraser()
   {  if(m_erase) m_cont.erase(m_index_it);  }

   AUTOBOOST_INTERPROCESS_FORCEINLINE void release() {  m_erase = false;  }

   private:
   Cont                   &m_cont;
   typename Cont::iterator m_index_it;
   bool                    m_erase;
};

}  //namespace interprocess {
}  //namespace autoboost {

#include <autoboost/interprocess/detail/config_end.hpp>

#endif   //#ifndef AUTOBOOST_INTERPROCESS_DETAIL_UTILITIES_HPP


//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_CONTAINER_DETAIL_PAIR_HPP
#define AUTOBOOST_CONTAINER_CONTAINER_DETAIL_PAIR_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>

#include <autoboost/container/detail/mpl.hpp>
#include <autoboost/container/detail/type_traits.hpp>
#include <autoboost/container/detail/mpl.hpp>
#include <autoboost/container/detail/std_fwd.hpp>
#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#  include <autoboost/container/detail/variadic_templates_tools.hpp>
#endif
#include <autoboost/move/adl_move_swap.hpp> //swap

#include <autoboost/intrusive/detail/minimal_pair_header.hpp>      //pair
#include <autoboost/move/utility_core.hpp>
#include<autoboost/move/detail/fwd_macros.hpp>

namespace autoboost {
namespace tuples {

struct null_type;

}  //namespace tuples {
}  //namespace autoboost {

#if defined(AUTOBOOST_MSVC) && (_CPPLIB_VER == 520)
//MSVC 2010 tuple marker
namespace std { namespace tr1 { struct _Nil; }}
#elif defined(AUTOBOOST_MSVC) && (_CPPLIB_VER == 540)
//MSVC 2012 tuple marker
namespace std { struct _Nil; }
#endif


namespace autoboost {
namespace container {

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

   template <int Dummy = 0>
   struct std_piecewise_construct_holder
   {
      static ::std::piecewise_construct_t *dummy;
   };

   template <int Dummy>
   ::std::piecewise_construct_t *std_piecewise_construct_holder<Dummy>::dummy =
      reinterpret_cast< ::std::piecewise_construct_t *>(0x01234);  //Avoid sanitizer errors on references to null pointers

typedef const std::piecewise_construct_t & piecewise_construct_t;

struct try_emplace_t{};

#else

//! The piecewise_construct_t struct is an empty structure type used as a unique type to
//! disambiguate used to disambiguate between different functions that take two tuple arguments.
typedef unspecified piecewise_construct_t;

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

//! A instance of type
//! piecewise_construct_t
static piecewise_construct_t piecewise_construct = AUTOBOOST_CONTAINER_DOC1ST(unspecified, *std_piecewise_construct_holder<>::dummy);

///@cond

namespace container_detail {

struct piecewise_construct_use
{
   //Avoid warnings of unused "piecewise_construct"
   piecewise_construct_use()
   {  (void)&::autoboost::container::piecewise_construct;   }
};

template <class T1, class T2>
struct pair;

template <class T>
struct is_pair
{
   static const bool value = false;
};

template <class T1, class T2>
struct is_pair< pair<T1, T2> >
{
   static const bool value = true;
};

template <class T1, class T2>
struct is_pair< std::pair<T1, T2> >
{
   static const bool value = true;
};

template <class T>
struct is_not_pair
{
   static const bool value = !is_pair<T>::value;
};

template <class T>
struct is_std_pair
{
   static const bool value = false;
};

template <class T1, class T2>
struct is_std_pair< std::pair<T1, T2> >
{
   static const bool value = true;
};

struct pair_nat;

template<typename T, typename U, typename V>
void get(T); //to enable ADL

///@endcond

template <class T1, class T2>
struct pair
{
   private:
   AUTOBOOST_COPYABLE_AND_MOVABLE(pair)

   public:
   typedef T1 first_type;
   typedef T2 second_type;

   T1 first;
   T2 second;

   //Default constructor
   pair()
      : first(), second()
   {}

   //pair copy assignment
   pair(const pair& x)
      : first(x.first), second(x.second)
   {}

   //pair move constructor
   pair(AUTOBOOST_RV_REF(pair) p)
      : first(::autoboost::move(p.first)), second(::autoboost::move(p.second))
   {}

   template <class D, class S>
   pair(const pair<D, S> &p)
      : first(p.first), second(p.second)
   {}

   template <class D, class S>
   pair(AUTOBOOST_RV_REF_BEG pair<D, S> AUTOBOOST_RV_REF_END p)
      : first(::autoboost::move(p.first)), second(::autoboost::move(p.second))
   {}

   //pair from two values
   pair(const T1 &t1, const T2 &t2)
      : first(t1)
      , second(t2)
   {}

   template<class U, class V>
   pair(AUTOBOOST_FWD_REF(U) u, AUTOBOOST_FWD_REF(V) v)
      : first(::autoboost::forward<U>(u))
      , second(::autoboost::forward<V>(v))
   {}

   //And now compatibility with std::pair
   pair(const std::pair<T1, T2>& x)
      : first(x.first), second(x.second)
   {}

   template <class D, class S>
   pair(const std::pair<D, S>& p)
      : first(p.first), second(p.second)
   {}

   pair(AUTOBOOST_RV_REF_BEG std::pair<T1, T2> AUTOBOOST_RV_REF_END p)
      : first(::autoboost::move(p.first)), second(::autoboost::move(p.second))
   {}

   template <class D, class S>
   pair(AUTOBOOST_RV_REF_BEG std::pair<D, S> AUTOBOOST_RV_REF_END p)
      : first(::autoboost::move(p.first)), second(::autoboost::move(p.second))
   {}

   #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
   template< class KeyType, class ...Args>
   pair(try_emplace_t, AUTOBOOST_FWD_REF(KeyType) k, Args && ...args)
      : first(autoboost::forward<KeyType>(k)), second(::autoboost::forward<Args>(args)...)\
   {}
   #else

   //piecewise construction from autoboost::tuple
   #define AUTOBOOST_PAIR_TRY_EMPLACE_CONSTRUCT_CODE(N)\
   template< class KeyType AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N > \
   pair( try_emplace_t, AUTOBOOST_FWD_REF(KeyType) k AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_UREF##N )\
      : first(autoboost::forward<KeyType>(k)), second(AUTOBOOST_MOVE_FWD##N)\
   {}\
   //
   AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_PAIR_TRY_EMPLACE_CONSTRUCT_CODE)
   #undef AUTOBOOST_PAIR_TRY_EMPLACE_CONSTRUCT_CODE

   #endif   //AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES

   //piecewise construction from autoboost::tuple
   #define AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_AUTOBOOST_TUPLE_CODE(N,M)\
   template< template<class, class, class, class, class, class, class, class, class, class> class BoostTuple \
            AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_OR(N,M)) AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_AND(N,M)) AUTOBOOST_MOVE_CLASSQ##M > \
   pair( piecewise_construct_t\
       , BoostTuple<AUTOBOOST_MOVE_TARG##N  AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(10,N),::autoboost::tuples::null_type)> p\
       , BoostTuple<AUTOBOOST_MOVE_TARGQ##M AUTOBOOST_MOVE_I##M AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(10,M),::autoboost::tuples::null_type)> q)\
      : first(AUTOBOOST_MOVE_TMPL_GET##N), second(AUTOBOOST_MOVE_TMPL_GETQ##M)\
   { (void)p; (void)q; }\
   //
   AUTOBOOST_MOVE_ITER2D_0TOMAX(9, AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_AUTOBOOST_TUPLE_CODE)
   #undef AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_AUTOBOOST_TUPLE_CODE

   //piecewise construction from variadic tuple (with delegating constructors)
   #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
   #  if !defined(AUTOBOOST_CONTAINER_NO_CXX11_DELEGATING_CONSTRUCTORS)
      private:
      template<template<class ...> class Tuple, class... Args1, class... Args2, size_t... Indexes1, size_t... Indexes2>
      pair(Tuple<Args1...>& t1, Tuple<Args2...>& t2, index_tuple<Indexes1...>, index_tuple<Indexes2...>)
         : first (::autoboost::forward<Args1>(get<Indexes1>(t1))...)
         , second(::autoboost::forward<Args2>(get<Indexes2>(t2))...)
      {  (void) t1; (void)t2; }

      public:
      template<template<class ...> class Tuple, class... Args1, class... Args2>
      pair(piecewise_construct_t, Tuple<Args1...> t1, Tuple<Args2...> t2)
         : pair(t1, t2, typename build_number_seq<sizeof...(Args1)>::type(), typename build_number_seq<sizeof...(Args2)>::type())
      {}
   #  else
      //piecewise construction from variadic tuple (suboptimal, without delegating constructors)
      private:
      template<typename T, template<class ...> class Tuple, typename... Args>
      static T build_from_args(Tuple<Args...>&& t)
      {  return do_build_from_args<T>(::autoboost::move(t), typename build_number_seq<sizeof...(Args)>::type());   }

      template<typename T, template<class ...> class Tuple, typename... Args, std::size_t... Indexes>
      static T do_build_from_args(Tuple<Args...> && t, const index_tuple<Indexes...>&)
      {  (void)t; return T(::autoboost::forward<Args>(get<Indexes>(t))...);  }

      public:
      template<template<class ...> class Tuple, class... Args1, class... Args2>
      pair(piecewise_construct_t, Tuple<Args1...> t1, Tuple<Args2...> t2)
         : first  (build_from_args<first_type> (::autoboost::move(t1)))
         , second (build_from_args<second_type>(::autoboost::move(t2)))
      {}
   #  endif   //AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
   #elif defined(AUTOBOOST_MSVC) && (_CPPLIB_VER == 520)
      //MSVC 2010 tuple implementation
      #define AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_MSVC2010_TUPLE_CODE(N,M)\
      template< template<class, class, class, class, class, class, class, class, class, class> class StdTuple \
               AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_OR(N,M)) AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_AND(N,M)) AUTOBOOST_MOVE_CLASSQ##M > \
      pair( piecewise_construct_t\
          , StdTuple<AUTOBOOST_MOVE_TARG##N  AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(10,N),::std::tr1::_Nil)> p\
          , StdTuple<AUTOBOOST_MOVE_TARGQ##M AUTOBOOST_MOVE_I##M AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(10,M),::std::tr1::_Nil)> q)\
         : first(AUTOBOOST_MOVE_GET_IDX##N), second(AUTOBOOST_MOVE_GET_IDXQ##M)\
      { (void)p; (void)q; }\
      //
      AUTOBOOST_MOVE_ITER2D_0TOMAX(9, AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_MSVC2010_TUPLE_CODE)
      #undef AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_MSVC2010_TUPLE_CODE
   #elif defined(AUTOBOOST_MSVC) && (_CPPLIB_VER == 540)
      #if _VARIADIC_MAX >= 9
      #define AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_MAX_IT 9
      #else
      #define AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_MAX_IT AUTOBOOST_MOVE_ADD(_VARIADIC_MAX, 1)
      #endif

      //MSVC 2012 tuple implementation
      #define AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_CODE(N,M)\
      template< template<AUTOBOOST_MOVE_REPEAT(_VARIADIC_MAX, class), class, class, class> class StdTuple \
               AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_OR(N,M)) AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_AND(N,M)) AUTOBOOST_MOVE_CLASSQ##M > \
      pair( piecewise_construct_t\
          , StdTuple<AUTOBOOST_MOVE_TARG##N  AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(AUTOBOOST_MOVE_ADD(_VARIADIC_MAX, 3),N),::std::_Nil) > p\
          , StdTuple<AUTOBOOST_MOVE_TARGQ##M AUTOBOOST_MOVE_I##M AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(AUTOBOOST_MOVE_ADD(_VARIADIC_MAX, 3),M),::std::_Nil) > q)\
         : first(AUTOBOOST_MOVE_GET_IDX##N), second(AUTOBOOST_MOVE_GET_IDXQ##M)\
      { (void)p; (void)q; }\
      //
      AUTOBOOST_MOVE_ITER2D_0TOMAX(AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_MAX_IT, AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_CODE)
      #undef AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_MSVC2010_TUPLE_CODE
      #undef AUTOBOOST_PAIR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_MAX_IT
   #endif

   //pair copy assignment
   pair& operator=(AUTOBOOST_COPY_ASSIGN_REF(pair) p)
   {
      first  = p.first;
      second = p.second;
      return *this;
   }

   //pair move assignment
   pair& operator=(AUTOBOOST_RV_REF(pair) p)
   {
      first  = ::autoboost::move(p.first);
      second = ::autoboost::move(p.second);
      return *this;
   }

   template <class D, class S>
   typename ::autoboost::container::container_detail::disable_if_or
      < pair &
      , ::autoboost::container::container_detail::is_same<T1, D>
      , ::autoboost::container::container_detail::is_same<T2, S>
      >::type
      operator=(const pair<D, S>&p)
   {
      first  = p.first;
      second = p.second;
      return *this;
   }

   template <class D, class S>
   typename ::autoboost::container::container_detail::disable_if_or
      < pair &
      , ::autoboost::container::container_detail::is_same<T1, D>
      , ::autoboost::container::container_detail::is_same<T2, S>
      >::type
      operator=(AUTOBOOST_RV_REF_BEG pair<D, S> AUTOBOOST_RV_REF_END p)
   {
      first  = ::autoboost::move(p.first);
      second = ::autoboost::move(p.second);
      return *this;
   }
//std::pair copy assignment
   pair& operator=(const std::pair<T1, T2> &p)
   {
      first  = p.first;
      second = p.second;
      return *this;
   }

   template <class D, class S>
   pair& operator=(const std::pair<D, S> &p)
   {
      first  = ::autoboost::move(p.first);
      second = ::autoboost::move(p.second);
      return *this;
   }

   //std::pair move assignment
   pair& operator=(AUTOBOOST_RV_REF_BEG std::pair<T1, T2> AUTOBOOST_RV_REF_END p)
   {
      first  = ::autoboost::move(p.first);
      second = ::autoboost::move(p.second);
      return *this;
   }

   template <class D, class S>
   pair& operator=(AUTOBOOST_RV_REF_BEG std::pair<D, S> AUTOBOOST_RV_REF_END p)
   {
      first  = ::autoboost::move(p.first);
      second = ::autoboost::move(p.second);
      return *this;
   }

   //swap
   void swap(pair& p)
   {
      ::autoboost::adl_move_swap(this->first, p.first);
      ::autoboost::adl_move_swap(this->second, p.second);
   }
};

template <class T1, class T2>
inline bool operator==(const pair<T1,T2>& x, const pair<T1,T2>& y)
{  return static_cast<bool>(x.first == y.first && x.second == y.second);  }

template <class T1, class T2>
inline bool operator< (const pair<T1,T2>& x, const pair<T1,T2>& y)
{  return static_cast<bool>(x.first < y.first ||
                         (!(y.first < x.first) && x.second < y.second)); }

template <class T1, class T2>
inline bool operator!=(const pair<T1,T2>& x, const pair<T1,T2>& y)
{  return static_cast<bool>(!(x == y));  }

template <class T1, class T2>
inline bool operator> (const pair<T1,T2>& x, const pair<T1,T2>& y)
{  return y < x;  }

template <class T1, class T2>
inline bool operator>=(const pair<T1,T2>& x, const pair<T1,T2>& y)
{  return static_cast<bool>(!(x < y)); }

template <class T1, class T2>
inline bool operator<=(const pair<T1,T2>& x, const pair<T1,T2>& y)
{  return static_cast<bool>(!(y < x)); }

template <class T1, class T2>
inline pair<T1, T2> make_pair(T1 x, T2 y)
{  return pair<T1, T2>(x, y); }

template <class T1, class T2>
inline void swap(pair<T1, T2>& x, pair<T1, T2>& y)
{  x.swap(y);  }

}  //namespace container_detail {
}  //namespace container {

#ifdef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

template<class T1, class T2>
struct has_move_emulation_enabled< ::autoboost::container::container_detail::pair<T1, T2> >
{
   static const bool value = true;
};

#endif

namespace move_detail{

template<class T>
struct is_class_or_union;

template <class T1, class T2>
struct is_class_or_union< ::autoboost::container::container_detail::pair<T1, T2> >
//This specialization is needed to avoid instantiation of pair in
//is_class, and allow recursive maps.
{
   static const bool value = true;
};

template <class T1, class T2>
struct is_class_or_union< std::pair<T1, T2> >
//This specialization is needed to avoid instantiation of pair in
//is_class, and allow recursive maps.
{
   static const bool value = true;
};

template<class T>
struct is_union;

template <class T1, class T2>
struct is_union< ::autoboost::container::container_detail::pair<T1, T2> >
//This specialization is needed to avoid instantiation of pair in
//is_class, and allow recursive maps.
{
   static const bool value = false;
};

template <class T1, class T2>
struct is_union< std::pair<T1, T2> >
//This specialization is needed to avoid instantiation of pair in
//is_class, and allow recursive maps.
{
   static const bool value = false;
};

template<class T>
struct is_class;

template <class T1, class T2>
struct is_class< ::autoboost::container::container_detail::pair<T1, T2> >
//This specialization is needed to avoid instantiation of pair in
//is_class, and allow recursive maps.
{
   static const bool value = true;
};

template <class T1, class T2>
struct is_class< std::pair<T1, T2> >
//This specialization is needed to avoid instantiation of pair in
//is_class, and allow recursive maps.
{
   static const bool value = true;
};

}  //namespace move_detail{

}  //namespace autoboost {

#include <autoboost/container/detail/config_end.hpp>

#endif   //#ifndef AUTOBOOST_CONTAINER_DETAIL_PAIR_HPP

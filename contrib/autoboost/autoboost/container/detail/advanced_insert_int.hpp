//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_ADVANCED_INSERT_INT_HPP
#define AUTOBOOST_CONTAINER_ADVANCED_INSERT_INT_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>

#include <autoboost/container/allocator_traits.hpp>
#include <autoboost/container/detail/destroyers.hpp>
#include <autoboost/aligned_storage.hpp>
#include <autoboost/move/utility_core.hpp>
#include <autoboost/container/detail/mpl.hpp>
#include <autoboost/container/detail/utilities.hpp>
#include <autoboost/container/detail/type_traits.hpp>
#include <autoboost/container/detail/iterators.hpp>
#include <iterator>  //std::iterator_traits
#include <autoboost/assert.hpp>
#include <autoboost/core/no_exceptions_support.hpp>

namespace autoboost { namespace container { namespace container_detail {

template<class A, class FwdIt, class Iterator>
struct move_insert_range_proxy
{
   typedef typename allocator_traits<A>::size_type size_type;
   typedef typename allocator_traits<A>::value_type value_type;

   explicit move_insert_range_proxy(FwdIt first)
      :  first_(first)
   {}

   void uninitialized_copy_n_and_update(A &a, Iterator p, size_type n)
   {
      this->first_ = ::autoboost::container::uninitialized_move_alloc_n_source
         (a, this->first_, n, p);
   }

   void copy_n_and_update(A &, Iterator p, size_type n)
   {
      this->first_ = ::autoboost::container::move_n_source(this->first_, n, p);
   }

   FwdIt first_;
};


template<class A, class FwdIt, class Iterator>
struct insert_range_proxy
{
   typedef typename allocator_traits<A>::size_type size_type;
   typedef typename allocator_traits<A>::value_type value_type;

   explicit insert_range_proxy(FwdIt first)
      :  first_(first)
   {}

   void uninitialized_copy_n_and_update(A &a, Iterator p, size_type n)
   {
      this->first_ = ::autoboost::container::uninitialized_copy_alloc_n_source(a, this->first_, n, p);
   }

   void copy_n_and_update(A &, Iterator p, size_type n)
   {
      this->first_ = ::autoboost::container::copy_n_source(this->first_, n, p);
   }

   FwdIt first_;
};


template<class A, class Iterator>
struct insert_n_copies_proxy
{
   typedef typename allocator_traits<A>::size_type size_type;
   typedef typename allocator_traits<A>::value_type value_type;

   explicit insert_n_copies_proxy(const value_type &v)
      :  v_(v)
   {}

   void uninitialized_copy_n_and_update(A &a, Iterator p, size_type n) const
   {  autoboost::container::uninitialized_fill_alloc_n(a, v_, n, p);  }

   void copy_n_and_update(A &, Iterator p, size_type n) const
   {
	   for (; 0 < n; --n, ++p){
		   *p = v_;
      }
   }

   const value_type &v_;
};

template<class A, class Iterator>
struct insert_value_initialized_n_proxy
{
   typedef ::autoboost::container::allocator_traits<A> alloc_traits;
   typedef typename allocator_traits<A>::size_type size_type;
   typedef typename allocator_traits<A>::value_type value_type;

   void uninitialized_copy_n_and_update(A &a, Iterator p, size_type n) const
   {  autoboost::container::uninitialized_value_init_alloc_n(a, n, p);  }

   void copy_n_and_update(A &, Iterator, size_type) const
   {  AUTOBOOST_ASSERT(false); }
};

template<class A, class Iterator>
struct insert_default_initialized_n_proxy
{
   typedef ::autoboost::container::allocator_traits<A> alloc_traits;
   typedef typename allocator_traits<A>::size_type size_type;
   typedef typename allocator_traits<A>::value_type value_type;

   void uninitialized_copy_n_and_update(A &a, Iterator p, size_type n) const
   {  autoboost::container::uninitialized_default_init_alloc_n(a, n, p);  }

   void copy_n_and_update(A &, Iterator, size_type) const
   {  AUTOBOOST_ASSERT(false); }
};

template<class A, class Iterator>
struct insert_copy_proxy
{
   typedef autoboost::container::allocator_traits<A> alloc_traits;
   typedef typename alloc_traits::size_type size_type;
   typedef typename alloc_traits::value_type value_type;

   explicit insert_copy_proxy(const value_type &v)
      :  v_(v)
   {}

   void uninitialized_copy_n_and_update(A &a, Iterator p, size_type n) const
   {
      AUTOBOOST_ASSERT(n == 1);  (void)n;
      alloc_traits::construct( a, iterator_to_raw_pointer(p), v_);
   }

   void copy_n_and_update(A &, Iterator p, size_type n) const
   {
      AUTOBOOST_ASSERT(n == 1);  (void)n;
      *p =v_;
   }

   const value_type &v_;
};


template<class A, class Iterator>
struct insert_move_proxy
{
   typedef autoboost::container::allocator_traits<A> alloc_traits;
   typedef typename alloc_traits::size_type size_type;
   typedef typename alloc_traits::value_type value_type;

   explicit insert_move_proxy(value_type &v)
      :  v_(v)
   {}

   void uninitialized_copy_n_and_update(A &a, Iterator p, size_type n) const
   {
      AUTOBOOST_ASSERT(n == 1);  (void)n;
      alloc_traits::construct( a, iterator_to_raw_pointer(p), ::autoboost::move(v_) );
   }

   void copy_n_and_update(A &, Iterator p, size_type n) const
   {
      AUTOBOOST_ASSERT(n == 1);  (void)n;
      *p = ::autoboost::move(v_);
   }

   value_type &v_;
};

template<class It, class A>
insert_move_proxy<A, It> get_insert_value_proxy(AUTOBOOST_RV_REF(typename std::iterator_traits<It>::value_type) v)
{
   return insert_move_proxy<A, It>(v);
}

template<class It, class A>
insert_copy_proxy<A, It> get_insert_value_proxy(const typename std::iterator_traits<It>::value_type &v)
{
   return insert_copy_proxy<A, It>(v);
}

}}}   //namespace autoboost { namespace container { namespace container_detail {

#ifdef AUTOBOOST_CONTAINER_PERFECT_FORWARDING

#include <autoboost/container/detail/variadic_templates_tools.hpp>
#include <autoboost/move/utility_core.hpp>
#include <typeinfo>
//#include <iostream> //For debugging purposes

namespace autoboost {
namespace container {
namespace container_detail {

template<class A, class Iterator, class ...Args>
struct insert_non_movable_emplace_proxy
{
   typedef autoboost::container::allocator_traits<A>   alloc_traits;
   typedef typename alloc_traits::size_type        size_type;
   typedef typename alloc_traits::value_type       value_type;

   typedef typename build_number_seq<sizeof...(Args)>::type index_tuple_t;

   explicit insert_non_movable_emplace_proxy(Args&&... args)
      : args_(args...)
   {}

   void uninitialized_copy_n_and_update(A &a, Iterator p, size_type n)
   {  this->priv_uninitialized_copy_some_and_update(a, index_tuple_t(), p, n);  }

   private:
   template<int ...IdxPack>
   void priv_uninitialized_copy_some_and_update(A &a, const index_tuple<IdxPack...>&, Iterator p, size_type n)
   {
      AUTOBOOST_ASSERT(n == 1); (void)n;
      alloc_traits::construct( a, iterator_to_raw_pointer(p), ::autoboost::forward<Args>(get<IdxPack>(this->args_))... );
   }

   protected:
   tuple<Args&...> args_;
};

template<class A, class Iterator, class ...Args>
struct insert_emplace_proxy
   :  public insert_non_movable_emplace_proxy<A, Iterator, Args...>
{
   typedef insert_non_movable_emplace_proxy<A, Iterator, Args...> base_t;
   typedef autoboost::container::allocator_traits<A>   alloc_traits;
   typedef typename base_t::value_type             value_type;
   typedef typename base_t::size_type              size_type;
   typedef typename base_t::index_tuple_t          index_tuple_t;

   explicit insert_emplace_proxy(Args&&... args)
      : base_t(::autoboost::forward<Args>(args)...)
   {}

   void copy_n_and_update(A &a, Iterator p, size_type n)
   {  this->priv_copy_some_and_update(a, index_tuple_t(), p, n);  }

   private:

   template<int ...IdxPack>
   void priv_copy_some_and_update(A &a, const index_tuple<IdxPack...>&, Iterator p, size_type n)
   {
      AUTOBOOST_ASSERT(n ==1); (void)n;
      aligned_storage<sizeof(value_type), alignment_of<value_type>::value> v;
      value_type *vp = static_cast<value_type *>(static_cast<void *>(&v));
      alloc_traits::construct(a, vp,
         ::autoboost::forward<Args>(get<IdxPack>(this->args_))...);
      AUTOBOOST_TRY{
         *p = ::autoboost::move(*vp);
      }
      AUTOBOOST_CATCH(...){
         alloc_traits::destroy(a, vp);
         AUTOBOOST_RETHROW
      }
      AUTOBOOST_CATCH_END
      alloc_traits::destroy(a, vp);
   }
};

//Specializations to avoid an unneeded temporary when emplacing from a single argument o type value_type
template<class A, class Iterator>
struct insert_emplace_proxy<A, Iterator, typename autoboost::container::allocator_traits<A>::value_type>
   : public insert_move_proxy<A, Iterator>
{
   explicit insert_emplace_proxy(typename autoboost::container::allocator_traits<A>::value_type &&v)
   : insert_move_proxy<A, Iterator>(v)
   {}
};

//We use "add_const" here as adding "const" only confuses MSVC12(and maybe later) provoking
//compiler error C2752 (�more than one partial specialization matches�).
//Any problem is solvable with an extra layer of indirection? ;-)
template<class A, class Iterator>
struct insert_emplace_proxy<A, Iterator
   , typename autoboost::container::container_detail::add_const<typename autoboost::container::allocator_traits<A>::value_type>::type
   >
   : public insert_copy_proxy<A, Iterator>
{
   explicit insert_emplace_proxy(const typename autoboost::container::allocator_traits<A>::value_type &v)
   : insert_copy_proxy<A, Iterator>(v)
   {}
};

template<class A, class Iterator>
struct insert_emplace_proxy<A, Iterator, typename autoboost::container::allocator_traits<A>::value_type &>
   : public insert_copy_proxy<A, Iterator>
{
   explicit insert_emplace_proxy(const typename autoboost::container::allocator_traits<A>::value_type &v)
   : insert_copy_proxy<A, Iterator>(v)
   {}
};

template<class A, class Iterator>
struct insert_emplace_proxy<A, Iterator
   , typename autoboost::container::container_detail::add_const<typename autoboost::container::allocator_traits<A>::value_type>::type &
   >
   : public insert_copy_proxy<A, Iterator>
{
   explicit insert_emplace_proxy(const typename autoboost::container::allocator_traits<A>::value_type &v)
   : insert_copy_proxy<A, Iterator>(v)
   {}
};

}}}   //namespace autoboost { namespace container { namespace container_detail {

#else //#ifdef AUTOBOOST_CONTAINER_PERFECT_FORWARDING

#include <autoboost/container/detail/preprocessor.hpp>
#include <autoboost/container/detail/value_init.hpp>

namespace autoboost {
namespace container {
namespace container_detail {

#define AUTOBOOST_PP_LOCAL_MACRO(N)                                                     \
template<class A, class Iterator AUTOBOOST_PP_ENUM_TRAILING_PARAMS(N, class P) >        \
struct AUTOBOOST_PP_CAT(insert_non_movable_emplace_proxy_arg, N)                        \
{                                                                                   \
   typedef autoboost::container::allocator_traits<A> alloc_traits;                      \
   typedef typename alloc_traits::size_type size_type;                              \
   typedef typename alloc_traits::value_type value_type;                            \
                                                                                    \
   explicit AUTOBOOST_PP_CAT(insert_non_movable_emplace_proxy_arg, N)                   \
      ( AUTOBOOST_PP_ENUM(N, AUTOBOOST_CONTAINER_PP_PARAM_LIST, _) )                        \
      AUTOBOOST_PP_EXPR_IF(N, :) AUTOBOOST_PP_ENUM(N, AUTOBOOST_CONTAINER_PP_PARAM_INIT, _)     \
   {}                                                                               \
                                                                                    \
   void uninitialized_copy_n_and_update(A &a, Iterator p, size_type n)              \
   {                                                                                \
      AUTOBOOST_ASSERT(n == 1); (void)n;                                                \
      alloc_traits::construct                                                       \
         ( a, iterator_to_raw_pointer(p)                                            \
         AUTOBOOST_PP_ENUM_TRAILING(N, AUTOBOOST_CONTAINER_PP_MEMBER_FORWARD, _)            \
         );                                                                         \
   }                                                                                \
                                                                                    \
   void copy_n_and_update(A &, Iterator, size_type)                                 \
   {  AUTOBOOST_ASSERT(false);   }                                                      \
                                                                                    \
   protected:                                                                       \
   AUTOBOOST_PP_REPEAT(N, AUTOBOOST_CONTAINER_PP_PARAM_DEFINE, _)                           \
};                                                                                  \
                                                                                    \
template<class A, class Iterator AUTOBOOST_PP_ENUM_TRAILING_PARAMS(N, class P) >        \
struct AUTOBOOST_PP_CAT(insert_emplace_proxy_arg, N)                                    \
   : AUTOBOOST_PP_CAT(insert_non_movable_emplace_proxy_arg, N)                          \
         < A, Iterator AUTOBOOST_PP_ENUM_TRAILING_PARAMS(N, P) >                        \
{                                                                                   \
   typedef AUTOBOOST_PP_CAT(insert_non_movable_emplace_proxy_arg, N)                    \
         <A, Iterator AUTOBOOST_PP_ENUM_TRAILING_PARAMS(N, P) > base_t;                 \
   typedef typename base_t::value_type       value_type;                            \
   typedef typename base_t::size_type  size_type;                                   \
   typedef autoboost::container::allocator_traits<A> alloc_traits;                      \
                                                                                    \
   explicit AUTOBOOST_PP_CAT(insert_emplace_proxy_arg, N)                               \
      ( AUTOBOOST_PP_ENUM(N, AUTOBOOST_CONTAINER_PP_PARAM_LIST, _) )                        \
      : base_t(AUTOBOOST_PP_ENUM(N, AUTOBOOST_CONTAINER_PP_PARAM_FORWARD, _) )              \
   {}                                                                               \
                                                                                    \
   void copy_n_and_update(A &a, Iterator p, size_type n)                            \
   {                                                                                \
      AUTOBOOST_ASSERT(n == 1); (void)n;                                                \
      aligned_storage<sizeof(value_type), alignment_of<value_type>::value> v;       \
      value_type *vp = static_cast<value_type *>(static_cast<void *>(&v));          \
      alloc_traits::construct(a, vp                                                 \
         AUTOBOOST_PP_ENUM_TRAILING(N, AUTOBOOST_CONTAINER_PP_MEMBER_FORWARD, _));          \
      AUTOBOOST_TRY{                                                                    \
         *p = ::autoboost::move(*vp);                                                   \
      }                                                                             \
      AUTOBOOST_CATCH(...){                                                             \
         alloc_traits::destroy(a, vp);                                              \
         AUTOBOOST_RETHROW                                                              \
      }                                                                             \
      AUTOBOOST_CATCH_END                                                               \
      alloc_traits::destroy(a, vp);                                                 \
   }                                                                                \
};                                                                                  \
//!
#define AUTOBOOST_PP_LOCAL_LIMITS (0, AUTOBOOST_CONTAINER_MAX_CONSTRUCTOR_PARAMETERS)
#include AUTOBOOST_PP_LOCAL_ITERATE()

#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)

//Specializations to avoid an unneeded temporary when emplacing from a single argument o type value_type
template<class A, class Iterator>
struct insert_emplace_proxy_arg1<A, Iterator, ::autoboost::rv<typename autoboost::container::allocator_traits<A>::value_type> >
   : public insert_move_proxy<A, Iterator>
{
   explicit insert_emplace_proxy_arg1(typename autoboost::container::allocator_traits<A>::value_type &v)
   : insert_move_proxy<A, Iterator>(v)
   {}
};

template<class A, class Iterator>
struct insert_emplace_proxy_arg1<A, Iterator, typename autoboost::container::allocator_traits<A>::value_type>
   : public insert_copy_proxy<A, Iterator>
{
   explicit insert_emplace_proxy_arg1(const typename autoboost::container::allocator_traits<A>::value_type &v)
   : insert_copy_proxy<A, Iterator>(v)
   {}
};

#else //e.g. MSVC10 & MSVC11

//Specializations to avoid an unneeded temporary when emplacing from a single argument o type value_type
template<class A, class Iterator>
struct insert_emplace_proxy_arg1<A, Iterator, typename autoboost::container::allocator_traits<A>::value_type>
   : public insert_move_proxy<A, Iterator>
{
   explicit insert_emplace_proxy_arg1(typename autoboost::container::allocator_traits<A>::value_type &&v)
   : insert_move_proxy<A, Iterator>(v)
   {}
};

//We use "add_const" here as adding "const" only confuses MSVC10&11 provoking
//compiler error C2752 (�more than one partial specialization matches�).
//Any problem is solvable with an extra layer of indirection? ;-)
template<class A, class Iterator>
struct insert_emplace_proxy_arg1<A, Iterator
   , typename autoboost::container::container_detail::add_const<typename autoboost::container::allocator_traits<A>::value_type>::type
   >
   : public insert_copy_proxy<A, Iterator>
{
   explicit insert_emplace_proxy_arg1(const typename autoboost::container::allocator_traits<A>::value_type &v)
   : insert_copy_proxy<A, Iterator>(v)
   {}
};

template<class A, class Iterator>
struct insert_emplace_proxy_arg1<A, Iterator, typename autoboost::container::allocator_traits<A>::value_type &>
   : public insert_copy_proxy<A, Iterator>
{
   explicit insert_emplace_proxy_arg1(const typename autoboost::container::allocator_traits<A>::value_type &v)
   : insert_copy_proxy<A, Iterator>(v)
   {}
};

template<class A, class Iterator>
struct insert_emplace_proxy_arg1<A, Iterator
   , typename autoboost::container::container_detail::add_const<typename autoboost::container::allocator_traits<A>::value_type>::type &
   >
   : public insert_copy_proxy<A, Iterator>
{
   explicit insert_emplace_proxy_arg1(const typename autoboost::container::allocator_traits<A>::value_type &v)
   : insert_copy_proxy<A, Iterator>(v)
   {}
};

#endif

}}}   //namespace autoboost { namespace container { namespace container_detail {

#endif   //#ifdef AUTOBOOST_CONTAINER_PERFECT_FORWARDING

#include <autoboost/container/detail/config_end.hpp>

#endif //#ifndef AUTOBOOST_CONTAINER_ADVANCED_INSERT_INT_HPP

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
// container/detail
#include <autoboost/container/detail/copy_move_algo.hpp>
#include <autoboost/container/detail/destroyers.hpp>
#include <autoboost/container/detail/mpl.hpp>
#include <autoboost/container/detail/type_traits.hpp>
#include <autoboost/container/detail/iterator.hpp>
#include <autoboost/container/detail/iterators.hpp>
#include <autoboost/container/detail/iterator_to_raw_pointer.hpp>
#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <autoboost/move/detail/fwd_macros.hpp>
#endif
// move
#include <autoboost/move/utility_core.hpp>
// other
#include <autoboost/assert.hpp>
#include <autoboost/core/no_exceptions_support.hpp>

namespace autoboost { namespace container { namespace container_detail {

template<class Allocator, class FwdIt, class Iterator>
struct move_insert_range_proxy
{
   typedef typename allocator_traits<Allocator>::size_type size_type;
   typedef typename allocator_traits<Allocator>::value_type value_type;

   explicit move_insert_range_proxy(FwdIt first)
      :  first_(first)
   {}

   void uninitialized_copy_n_and_update(Allocator &a, Iterator p, size_type n)
   {
      this->first_ = ::autoboost::container::uninitialized_move_alloc_n_source
         (a, this->first_, n, p);
   }

   void copy_n_and_update(Allocator &, Iterator p, size_type n)
   {
      this->first_ = ::autoboost::container::move_n_source(this->first_, n, p);
   }

   FwdIt first_;
};


template<class Allocator, class FwdIt, class Iterator>
struct insert_range_proxy
{
   typedef typename allocator_traits<Allocator>::size_type size_type;
   typedef typename allocator_traits<Allocator>::value_type value_type;

   explicit insert_range_proxy(FwdIt first)
      :  first_(first)
   {}

   void uninitialized_copy_n_and_update(Allocator &a, Iterator p, size_type n)
   {
      this->first_ = ::autoboost::container::uninitialized_copy_alloc_n_source(a, this->first_, n, p);
   }

   void copy_n_and_update(Allocator &, Iterator p, size_type n)
   {
      this->first_ = ::autoboost::container::copy_n_source(this->first_, n, p);
   }

   FwdIt first_;
};


template<class Allocator, class Iterator>
struct insert_n_copies_proxy
{
   typedef typename allocator_traits<Allocator>::size_type size_type;
   typedef typename allocator_traits<Allocator>::value_type value_type;

   explicit insert_n_copies_proxy(const value_type &v)
      :  v_(v)
   {}

   void uninitialized_copy_n_and_update(Allocator &a, Iterator p, size_type n) const
   {  autoboost::container::uninitialized_fill_alloc_n(a, v_, n, p);  }

   void copy_n_and_update(Allocator &, Iterator p, size_type n) const
   {
      for (; 0 < n; --n, ++p){
         *p = v_;
      }
   }

   const value_type &v_;
};

template<class Allocator, class Iterator>
struct insert_value_initialized_n_proxy
{
   typedef ::autoboost::container::allocator_traits<Allocator> alloc_traits;
   typedef typename allocator_traits<Allocator>::size_type size_type;
   typedef typename allocator_traits<Allocator>::value_type value_type;

   void uninitialized_copy_n_and_update(Allocator &a, Iterator p, size_type n) const
   {  autoboost::container::uninitialized_value_init_alloc_n(a, n, p);  }

   void copy_n_and_update(Allocator &, Iterator, size_type) const
   {  AUTOBOOST_ASSERT(false); }
};

template<class Allocator, class Iterator>
struct insert_default_initialized_n_proxy
{
   typedef ::autoboost::container::allocator_traits<Allocator> alloc_traits;
   typedef typename allocator_traits<Allocator>::size_type size_type;
   typedef typename allocator_traits<Allocator>::value_type value_type;

   void uninitialized_copy_n_and_update(Allocator &a, Iterator p, size_type n) const
   {  autoboost::container::uninitialized_default_init_alloc_n(a, n, p);  }

   void copy_n_and_update(Allocator &, Iterator, size_type) const
   {  AUTOBOOST_ASSERT(false); }
};

template<class Allocator, class Iterator>
struct insert_copy_proxy
{
   typedef autoboost::container::allocator_traits<Allocator> alloc_traits;
   typedef typename alloc_traits::size_type size_type;
   typedef typename alloc_traits::value_type value_type;

   explicit insert_copy_proxy(const value_type &v)
      :  v_(v)
   {}

   void uninitialized_copy_n_and_update(Allocator &a, Iterator p, size_type n) const
   {
      AUTOBOOST_ASSERT(n == 1);  (void)n;
      alloc_traits::construct( a, iterator_to_raw_pointer(p), v_);
   }

   void copy_n_and_update(Allocator &, Iterator p, size_type n) const
   {
      AUTOBOOST_ASSERT(n == 1);  (void)n;
      *p =v_;
   }

   const value_type &v_;
};


template<class Allocator, class Iterator>
struct insert_move_proxy
{
   typedef autoboost::container::allocator_traits<Allocator> alloc_traits;
   typedef typename alloc_traits::size_type size_type;
   typedef typename alloc_traits::value_type value_type;

   explicit insert_move_proxy(value_type &v)
      :  v_(v)
   {}

   void uninitialized_copy_n_and_update(Allocator &a, Iterator p, size_type n) const
   {
      AUTOBOOST_ASSERT(n == 1);  (void)n;
      alloc_traits::construct( a, iterator_to_raw_pointer(p), ::autoboost::move(v_) );
   }

   void copy_n_and_update(Allocator &, Iterator p, size_type n) const
   {
      AUTOBOOST_ASSERT(n == 1);  (void)n;
      *p = ::autoboost::move(v_);
   }

   value_type &v_;
};

template<class It, class Allocator>
insert_move_proxy<Allocator, It> get_insert_value_proxy(AUTOBOOST_RV_REF(typename autoboost::container::iterator_traits<It>::value_type) v)
{
   return insert_move_proxy<Allocator, It>(v);
}

template<class It, class Allocator>
insert_copy_proxy<Allocator, It> get_insert_value_proxy(const typename autoboost::container::iterator_traits<It>::value_type &v)
{
   return insert_copy_proxy<Allocator, It>(v);
}

}}}   //namespace autoboost { namespace container { namespace container_detail {

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

#include <autoboost/container/detail/variadic_templates_tools.hpp>
#include <autoboost/move/utility_core.hpp>

namespace autoboost {
namespace container {
namespace container_detail {

template<class Allocator, class Iterator, class ...Args>
struct insert_nonmovable_emplace_proxy
{
   typedef autoboost::container::allocator_traits<Allocator>   alloc_traits;
   typedef typename alloc_traits::size_type        size_type;
   typedef typename alloc_traits::value_type       value_type;

   typedef typename build_number_seq<sizeof...(Args)>::type index_tuple_t;

   explicit insert_nonmovable_emplace_proxy(AUTOBOOST_FWD_REF(Args)... args)
      : args_(args...)
   {}

   void uninitialized_copy_n_and_update(Allocator &a, Iterator p, size_type n)
   {  this->priv_uninitialized_copy_some_and_update(a, index_tuple_t(), p, n);  }

   private:
   template<std::size_t ...IdxPack>
   void priv_uninitialized_copy_some_and_update(Allocator &a, const index_tuple<IdxPack...>&, Iterator p, size_type n)
   {
      AUTOBOOST_ASSERT(n == 1); (void)n;
      alloc_traits::construct( a, iterator_to_raw_pointer(p), ::autoboost::forward<Args>(get<IdxPack>(this->args_))... );
   }

   protected:
   tuple<Args&...> args_;
};

template<class Allocator, class Iterator, class ...Args>
struct insert_emplace_proxy
   :  public insert_nonmovable_emplace_proxy<Allocator, Iterator, Args...>
{
   typedef insert_nonmovable_emplace_proxy<Allocator, Iterator, Args...> base_t;
   typedef autoboost::container::allocator_traits<Allocator>   alloc_traits;
   typedef typename base_t::value_type             value_type;
   typedef typename base_t::size_type              size_type;
   typedef typename base_t::index_tuple_t          index_tuple_t;

   explicit insert_emplace_proxy(AUTOBOOST_FWD_REF(Args)... args)
      : base_t(::autoboost::forward<Args>(args)...)
   {}

   void copy_n_and_update(Allocator &a, Iterator p, size_type n)
   {  this->priv_copy_some_and_update(a, index_tuple_t(), p, n);  }

   private:

   template<std::size_t ...IdxPack>
   void priv_copy_some_and_update(Allocator &a, const index_tuple<IdxPack...>&, Iterator p, size_type n)
   {
      AUTOBOOST_ASSERT(n ==1); (void)n;
      typename aligned_storage<sizeof(value_type), alignment_of<value_type>::value>::type v;
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
template<class Allocator, class Iterator>
struct insert_emplace_proxy<Allocator, Iterator, typename autoboost::container::allocator_traits<Allocator>::value_type>
   : public insert_move_proxy<Allocator, Iterator>
{
   explicit insert_emplace_proxy(typename autoboost::container::allocator_traits<Allocator>::value_type &&v)
   : insert_move_proxy<Allocator, Iterator>(v)
   {}
};

//We use "add_const" here as adding "const" only confuses MSVC12(and maybe later) provoking
//compiler error C2752 ("more than one partial specialization matches").
//Any problem is solvable with an extra layer of indirection? ;-)
template<class Allocator, class Iterator>
struct insert_emplace_proxy<Allocator, Iterator
   , typename autoboost::container::container_detail::add_const<typename autoboost::container::allocator_traits<Allocator>::value_type>::type
   >
   : public insert_copy_proxy<Allocator, Iterator>
{
   explicit insert_emplace_proxy(const typename autoboost::container::allocator_traits<Allocator>::value_type &v)
   : insert_copy_proxy<Allocator, Iterator>(v)
   {}
};

template<class Allocator, class Iterator>
struct insert_emplace_proxy<Allocator, Iterator, typename autoboost::container::allocator_traits<Allocator>::value_type &>
   : public insert_copy_proxy<Allocator, Iterator>
{
   explicit insert_emplace_proxy(const typename autoboost::container::allocator_traits<Allocator>::value_type &v)
   : insert_copy_proxy<Allocator, Iterator>(v)
   {}
};

template<class Allocator, class Iterator>
struct insert_emplace_proxy<Allocator, Iterator
   , typename autoboost::container::container_detail::add_const<typename autoboost::container::allocator_traits<Allocator>::value_type>::type &
   >
   : public insert_copy_proxy<Allocator, Iterator>
{
   explicit insert_emplace_proxy(const typename autoboost::container::allocator_traits<Allocator>::value_type &v)
   : insert_copy_proxy<Allocator, Iterator>(v)
   {}
};

}}}   //namespace autoboost { namespace container { namespace container_detail {

#else // !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

#include <autoboost/container/detail/value_init.hpp>

namespace autoboost {
namespace container {
namespace container_detail {

#define AUTOBOOST_CONTAINER_ADVANCED_INSERT_INT_CODE(N) \
template< class Allocator, class Iterator AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N >\
struct insert_nonmovable_emplace_proxy##N\
{\
   typedef autoboost::container::allocator_traits<Allocator> alloc_traits;\
   typedef typename alloc_traits::size_type size_type;\
   typedef typename alloc_traits::value_type value_type;\
   \
   explicit insert_nonmovable_emplace_proxy##N(AUTOBOOST_MOVE_UREF##N)\
      AUTOBOOST_MOVE_COLON##N AUTOBOOST_MOVE_FWD_INIT##N {}\
   \
   void uninitialized_copy_n_and_update(Allocator &a, Iterator p, size_type n)\
   {\
      AUTOBOOST_ASSERT(n == 1); (void)n;\
      alloc_traits::construct(a, iterator_to_raw_pointer(p) AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_MFWD##N);\
   }\
   \
   void copy_n_and_update(Allocator &, Iterator, size_type)\
   {  AUTOBOOST_ASSERT(false);   }\
   \
   protected:\
   AUTOBOOST_MOVE_MREF##N\
};\
\
template< class Allocator, class Iterator AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N >\
struct insert_emplace_proxy_arg##N\
   : insert_nonmovable_emplace_proxy##N< Allocator, Iterator AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_TARG##N >\
{\
   typedef insert_nonmovable_emplace_proxy##N\
      < Allocator, Iterator AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_TARG##N > base_t;\
   typedef typename base_t::value_type value_type;\
   typedef typename base_t::size_type size_type;\
   typedef autoboost::container::allocator_traits<Allocator> alloc_traits;\
   \
   explicit insert_emplace_proxy_arg##N(AUTOBOOST_MOVE_UREF##N)\
      : base_t(AUTOBOOST_MOVE_FWD##N){}\
   \
   void copy_n_and_update(Allocator &a, Iterator p, size_type n)\
   {\
      AUTOBOOST_ASSERT(n == 1); (void)n;\
      typename aligned_storage<sizeof(value_type), alignment_of<value_type>::value>::type v;\
      AUTOBOOST_ASSERT((((size_type)(&v)) % alignment_of<value_type>::value) == 0);\
      value_type *vp = static_cast<value_type *>(static_cast<void *>(&v));\
      alloc_traits::construct(a, vp AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_MFWD##N);\
      AUTOBOOST_TRY{\
         *p = ::autoboost::move(*vp);\
      }\
      AUTOBOOST_CATCH(...){\
         alloc_traits::destroy(a, vp);\
         AUTOBOOST_RETHROW\
      }\
      AUTOBOOST_CATCH_END\
      alloc_traits::destroy(a, vp);\
   }\
};\
//
AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_CONTAINER_ADVANCED_INSERT_INT_CODE)
#undef AUTOBOOST_CONTAINER_ADVANCED_INSERT_INT_CODE

#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)

//Specializations to avoid an unneeded temporary when emplacing from a single argument o type value_type
template<class Allocator, class Iterator>
struct insert_emplace_proxy_arg1<Allocator, Iterator, ::autoboost::rv<typename autoboost::container::allocator_traits<Allocator>::value_type> >
   : public insert_move_proxy<Allocator, Iterator>
{
   explicit insert_emplace_proxy_arg1(typename autoboost::container::allocator_traits<Allocator>::value_type &v)
   : insert_move_proxy<Allocator, Iterator>(v)
   {}
};

template<class Allocator, class Iterator>
struct insert_emplace_proxy_arg1<Allocator, Iterator, typename autoboost::container::allocator_traits<Allocator>::value_type>
   : public insert_copy_proxy<Allocator, Iterator>
{
   explicit insert_emplace_proxy_arg1(const typename autoboost::container::allocator_traits<Allocator>::value_type &v)
   : insert_copy_proxy<Allocator, Iterator>(v)
   {}
};

#else //e.g. MSVC10 & MSVC11

//Specializations to avoid an unneeded temporary when emplacing from a single argument o type value_type
template<class Allocator, class Iterator>
struct insert_emplace_proxy_arg1<Allocator, Iterator, typename autoboost::container::allocator_traits<Allocator>::value_type>
   : public insert_move_proxy<Allocator, Iterator>
{
   explicit insert_emplace_proxy_arg1(typename autoboost::container::allocator_traits<Allocator>::value_type &&v)
   : insert_move_proxy<Allocator, Iterator>(v)
   {}
};

//We use "add_const" here as adding "const" only confuses MSVC10&11 provoking
//compiler error C2752 ("more than one partial specialization matches").
//Any problem is solvable with an extra layer of indirection? ;-)
template<class Allocator, class Iterator>
struct insert_emplace_proxy_arg1<Allocator, Iterator
   , typename autoboost::container::container_detail::add_const<typename autoboost::container::allocator_traits<Allocator>::value_type>::type
   >
   : public insert_copy_proxy<Allocator, Iterator>
{
   explicit insert_emplace_proxy_arg1(const typename autoboost::container::allocator_traits<Allocator>::value_type &v)
   : insert_copy_proxy<Allocator, Iterator>(v)
   {}
};

template<class Allocator, class Iterator>
struct insert_emplace_proxy_arg1<Allocator, Iterator, typename autoboost::container::allocator_traits<Allocator>::value_type &>
   : public insert_copy_proxy<Allocator, Iterator>
{
   explicit insert_emplace_proxy_arg1(const typename autoboost::container::allocator_traits<Allocator>::value_type &v)
   : insert_copy_proxy<Allocator, Iterator>(v)
   {}
};

template<class Allocator, class Iterator>
struct insert_emplace_proxy_arg1<Allocator, Iterator
   , typename autoboost::container::container_detail::add_const<typename autoboost::container::allocator_traits<Allocator>::value_type>::type &
   >
   : public insert_copy_proxy<Allocator, Iterator>
{
   explicit insert_emplace_proxy_arg1(const typename autoboost::container::allocator_traits<Allocator>::value_type &v)
   : insert_copy_proxy<Allocator, Iterator>(v)
   {}
};

#endif

}}}   //namespace autoboost { namespace container { namespace container_detail {

#endif   // !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

#include <autoboost/container/detail/config_end.hpp>

#endif //#ifndef AUTOBOOST_CONTAINER_ADVANCED_INSERT_INT_HPP

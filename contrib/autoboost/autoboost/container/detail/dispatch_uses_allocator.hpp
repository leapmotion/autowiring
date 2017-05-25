//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2015-2015. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_DISPATCH_USES_ALLOCATOR_HPP
#define AUTOBOOST_CONTAINER_DISPATCH_USES_ALLOCATOR_HPP

#if defined (_MSC_VER)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>

#include <autoboost/container/allocator_traits.hpp>
#include <autoboost/container/uses_allocator.hpp>

#include <autoboost/container/detail/addressof.hpp>
#include <autoboost/container/detail/mpl.hpp>
#include <autoboost/container/detail/pair.hpp>
#include <autoboost/container/detail/type_traits.hpp>

#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <autoboost/move/detail/fwd_macros.hpp>
#endif
#include <autoboost/move/utility_core.hpp>

#include <autoboost/core/no_exceptions_support.hpp>

namespace autoboost { namespace container {

namespace container_detail {


// Check if we can detect is_convertible using advanced SFINAE expressions
#if !defined(AUTOBOOST_NO_CXX11_DECLTYPE) && !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   //! Code inspired by Mathias Gaunard's is_convertible.cpp found in the Boost mailing list
   //! http://autoboost.2283326.n4.nabble.com/type-traits-is-constructible-when-decltype-is-supported-td3575452.html
   //! Thanks Mathias!

   //With variadic templates, we need a single class to implement the trait
   template<class T, class ...Args>
   struct is_constructible
   {
      typedef char yes_type;
      struct no_type
      { char padding[2]; };

      template<std::size_t N>
      struct dummy;

      template<class X>
      static decltype(X(autoboost::move_detail::declval<Args>()...), true_type()) test(int);

      template<class X>
      static no_type test(...);

      static const bool value = sizeof(test<T>(0)) == sizeof(yes_type);
   };

   template <class T, class InnerAlloc, class ...Args>
   struct is_constructible_with_allocator_prefix
      : is_constructible<T, allocator_arg_t, InnerAlloc, Args...>
   {};

#else    // #if !defined(AUTOBOOST_NO_SFINAE_EXPR) && !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   //Without advanced SFINAE expressions, we can't use is_constructible
   //so backup to constructible_with_allocator_xxx

   #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   template <class T, class InnerAlloc, class ...Args>
   struct is_constructible_with_allocator_prefix
      : constructible_with_allocator_prefix<T>
   {};

   template <class T, class InnerAlloc, class ...Args>
   struct is_constructible_with_allocator_suffix
      : constructible_with_allocator_suffix<T>
   {};

   #else    // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   template <class T, class InnerAlloc, AUTOBOOST_MOVE_CLASSDFLT9>
   struct is_constructible_with_allocator_prefix
      : constructible_with_allocator_prefix<T>
   {};

   template <class T, class InnerAlloc, AUTOBOOST_MOVE_CLASSDFLT9>
   struct is_constructible_with_allocator_suffix
      : constructible_with_allocator_suffix<T>
   {};

   #endif   // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

#endif   // #if !defined(AUTOBOOST_NO_SFINAE_EXPR)

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

template < typename ConstructAlloc
         , typename ArgAlloc
         , typename T
         , class ...Args
         >
inline typename container_detail::enable_if_and
   < void
   , container_detail::is_not_pair<T>
   , container_detail::not_< uses_allocator<T, ArgAlloc> >
   >::type dispatch_uses_allocator
   ( ConstructAlloc & construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, T* p, AUTOBOOST_FWD_REF(Args)...args)
{
   (void)arg_alloc;
   allocator_traits<ConstructAlloc>::construct(construct_alloc, p, ::autoboost::forward<Args>(args)...);
}

// allocator_arg_t
template < typename ConstructAlloc
         , typename ArgAlloc
         , typename T
         , class ...Args
         >
inline typename container_detail::enable_if_and
   < void
   , container_detail::is_not_pair<T>
   , uses_allocator<T, ArgAlloc>
   , is_constructible_with_allocator_prefix<T, ArgAlloc, Args...>
   >::type dispatch_uses_allocator
   ( ConstructAlloc& construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, T* p, AUTOBOOST_FWD_REF(Args) ...args)
{
   allocator_traits<ConstructAlloc>::construct
      ( construct_alloc, p, allocator_arg
      , ::autoboost::forward<ArgAlloc>(arg_alloc), ::autoboost::forward<Args>(args)...);
}

// allocator suffix
template < typename ConstructAlloc
         , typename ArgAlloc
         , typename T
         , class ...Args
         >
inline typename container_detail::enable_if_and
   < void
   , container_detail::is_not_pair<T>
   , uses_allocator<T, ArgAlloc>
   , container_detail::not_<is_constructible_with_allocator_prefix<T, ArgAlloc, Args...> >
   >::type dispatch_uses_allocator
   ( ConstructAlloc& construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, T* p, AUTOBOOST_FWD_REF(Args)...args)
{
   allocator_traits<ConstructAlloc>::construct
      (construct_alloc, p, ::autoboost::forward<Args>(args)..., ::autoboost::forward<ArgAlloc>(arg_alloc));
}

#else    //#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

#define AUTOBOOST_CONTAINER_SCOPED_ALLOCATOR_DISPATCH_USES_ALLOCATOR_CODE(N) \
   template <typename ConstructAlloc, typename ArgAlloc, typename T AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N >\
   inline typename container_detail::enable_if_and\
      < void\
      , container_detail::is_not_pair<T>\
      , container_detail::not_<uses_allocator<T, ArgAlloc> >\
      >::type\
      dispatch_uses_allocator\
      (ConstructAlloc &construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, T* p AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_UREF##N)\
   {\
      (void)arg_alloc;\
      allocator_traits<ConstructAlloc>::construct(construct_alloc, p AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_FWD##N);\
   }\
//
AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_CONTAINER_SCOPED_ALLOCATOR_DISPATCH_USES_ALLOCATOR_CODE)
#undef AUTOBOOST_CONTAINER_SCOPED_ALLOCATOR_DISPATCH_USES_ALLOCATOR_CODE

#define AUTOBOOST_CONTAINER_SCOPED_ALLOCATOR_DISPATCH_USES_ALLOCATOR_CODE(N) \
   template < typename ConstructAlloc, typename ArgAlloc, typename T AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N >\
   inline typename container_detail::enable_if_and\
      < void\
      , container_detail::is_not_pair<T>\
      , uses_allocator<T, ArgAlloc>\
      , is_constructible_with_allocator_prefix<T, ArgAlloc AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_TARG##N>\
      >::type\
      dispatch_uses_allocator\
      (ConstructAlloc& construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, T* p AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_UREF##N)\
   {\
      allocator_traits<ConstructAlloc>::construct\
         (construct_alloc, p, allocator_arg, ::autoboost::forward<ArgAlloc>(arg_alloc) AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_FWD##N);\
   }\
//
AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_CONTAINER_SCOPED_ALLOCATOR_DISPATCH_USES_ALLOCATOR_CODE)
#undef AUTOBOOST_CONTAINER_SCOPED_ALLOCATOR_DISPATCH_USES_ALLOCATOR_CODE

#define AUTOBOOST_CONTAINER_SCOPED_ALLOCATOR_DISPATCH_USES_ALLOCATOR_CODE(N) \
   template < typename ConstructAlloc, typename ArgAlloc, typename T AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N >\
   inline typename container_detail::enable_if_and\
      < void\
      , container_detail::is_not_pair<T>\
      , uses_allocator<T, ArgAlloc>\
      , container_detail::not_<is_constructible_with_allocator_prefix<T, ArgAlloc AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_TARG##N> >\
      >::type\
      dispatch_uses_allocator\
      (ConstructAlloc& construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, T* p AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_UREF##N)\
   {\
      allocator_traits<ConstructAlloc>::construct\
         (construct_alloc, p AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_FWD##N, ::autoboost::forward<ArgAlloc>(arg_alloc));\
   }\
//
AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_CONTAINER_SCOPED_ALLOCATOR_DISPATCH_USES_ALLOCATOR_CODE)
#undef AUTOBOOST_CONTAINER_SCOPED_ALLOCATOR_DISPATCH_USES_ALLOCATOR_CODE

#endif   //#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

template < typename ConstructAlloc
         , typename ArgAlloc
         , typename Pair
         > inline
AUTOBOOST_CONTAINER_DOC1ST(void, typename container_detail::enable_if<container_detail::is_pair<Pair> >::type)
   dispatch_uses_allocator
   ( ConstructAlloc & construct_alloc
   , AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc
   , Pair* p)
{
   (dispatch_uses_allocator)(construct_alloc, arg_alloc, container_detail::addressof(p->first));
   AUTOBOOST_TRY{
      (dispatch_uses_allocator)(construct_alloc, arg_alloc, container_detail::addressof(p->second));
   }
   AUTOBOOST_CATCH(...) {
      allocator_traits<ConstructAlloc>::destroy(construct_alloc, container_detail::addressof(p->first));
      AUTOBOOST_RETHROW
   }
   AUTOBOOST_CATCH_END
}


template < typename ConstructAlloc
         , typename ArgAlloc
         , class Pair, class U, class V>
AUTOBOOST_CONTAINER_DOC1ST(void, typename container_detail::enable_if<container_detail::is_pair<Pair> >::type)
   dispatch_uses_allocator
   ( ConstructAlloc & construct_alloc
   , AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc
   , Pair* p, AUTOBOOST_FWD_REF(U) x, AUTOBOOST_FWD_REF(V) y)
{
   (dispatch_uses_allocator)(construct_alloc, arg_alloc, container_detail::addressof(p->first), ::autoboost::forward<U>(x));
   AUTOBOOST_TRY{
      (dispatch_uses_allocator)(construct_alloc, arg_alloc, container_detail::addressof(p->second), ::autoboost::forward<V>(y));
   }
   AUTOBOOST_CATCH(...){
      allocator_traits<ConstructAlloc>::destroy(construct_alloc, container_detail::addressof(p->first));
      AUTOBOOST_RETHROW
   }
   AUTOBOOST_CATCH_END
}

template < typename ConstructAlloc
         , typename ArgAlloc
         , class Pair, class Pair2>
AUTOBOOST_CONTAINER_DOC1ST(void, typename container_detail::enable_if< container_detail::is_pair<Pair> >::type)
   dispatch_uses_allocator
   (ConstructAlloc & construct_alloc
   , AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc
   , Pair* p, Pair2& x)
{  (dispatch_uses_allocator)(construct_alloc, arg_alloc, p, x.first, x.second);  }

template < typename ConstructAlloc
         , typename ArgAlloc
         , class Pair, class Pair2>
typename container_detail::enable_if_and
   < void
   , container_detail::is_pair<Pair>
   , container_detail::not_<autoboost::move_detail::is_reference<Pair2> > >::type //This is needed for MSVC10 and ambiguous overloads
   dispatch_uses_allocator
   (ConstructAlloc & construct_alloc
      , AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc
      , Pair* p, AUTOBOOST_RV_REF_BEG Pair2 AUTOBOOST_RV_REF_END x)
{  (dispatch_uses_allocator)(construct_alloc, arg_alloc, p, ::autoboost::move(x.first), ::autoboost::move(x.second));  }


//piecewise construction from autoboost::tuple
#define AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_AUTOBOOST_TUPLE_CODE(N,M)\
template< typename ConstructAlloc, typename ArgAlloc, class Pair \
        , template<class, class, class, class, class, class, class, class, class, class> class BoostTuple \
         AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_OR(N,M)) AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_AND(N,M)) AUTOBOOST_MOVE_CLASSQ##M > \
typename container_detail::enable_if< container_detail::is_pair<Pair> >::type\
   dispatch_uses_allocator( ConstructAlloc & construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, Pair* pair, piecewise_construct_t\
      , BoostTuple<AUTOBOOST_MOVE_TARG##N  AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(10,N),::autoboost::tuples::null_type)> p\
      , BoostTuple<AUTOBOOST_MOVE_TARGQ##M AUTOBOOST_MOVE_I##M AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(10,M),::autoboost::tuples::null_type)> q)\
{\
   (void)p; (void)q;\
   (dispatch_uses_allocator)\
      (construct_alloc, arg_alloc, container_detail::addressof(pair->first) AUTOBOOST_MOVE_I_IF(N) AUTOBOOST_MOVE_TMPL_GET##N);\
   AUTOBOOST_TRY{\
      (dispatch_uses_allocator)\
         (construct_alloc, arg_alloc, container_detail::addressof(pair->second) AUTOBOOST_MOVE_I_IF(M) AUTOBOOST_MOVE_TMPL_GETQ##M);\
   }\
   AUTOBOOST_CATCH(...) {\
      allocator_traits<ConstructAlloc>::destroy(construct_alloc, container_detail::addressof(pair->first));\
      AUTOBOOST_RETHROW\
   }\
   AUTOBOOST_CATCH_END\
}\
//
AUTOBOOST_MOVE_ITER2D_0TOMAX(9, AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_AUTOBOOST_TUPLE_CODE)
#undef AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_AUTOBOOST_TUPLE_CODE

//piecewise construction from Std Tuple
#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   template< typename ConstructAlloc, typename ArgAlloc, class Pair
           , template<class ...> class Tuple, class... Args1, class... Args2, size_t... Indexes1, size_t... Indexes2>
   void dispatch_uses_allocator_index( ConstructAlloc & construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, Pair* pair
                                    , Tuple<Args1...>& t1, Tuple<Args2...>& t2, index_tuple<Indexes1...>, index_tuple<Indexes2...>)
   {
      (void)t1; (void)t2;
      (dispatch_uses_allocator)(construct_alloc, arg_alloc, container_detail::addressof(pair->first), ::autoboost::forward<Args1>(get<Indexes1>(t1))...);
      AUTOBOOST_TRY{
         (dispatch_uses_allocator)(construct_alloc, arg_alloc, container_detail::addressof(pair->second), ::autoboost::forward<Args2>(get<Indexes2>(t2))...);
      }
      AUTOBOOST_CATCH(...){
         allocator_traits<ConstructAlloc>::destroy(construct_alloc, container_detail::addressof(pair->first));
         AUTOBOOST_RETHROW
      }
      AUTOBOOST_CATCH_END
   }

   template< typename ConstructAlloc, typename ArgAlloc, class Pair
           , template<class ...> class Tuple, class... Args1, class... Args2>
   typename container_detail::enable_if< container_detail::is_pair<Pair> >::type
      dispatch_uses_allocator( ConstructAlloc & construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, Pair* pair, piecewise_construct_t
                             , Tuple<Args1...> t1, Tuple<Args2...> t2)
   {
      (dispatch_uses_allocator_index)( construct_alloc, arg_alloc, pair, t1, t2
                                     , typename build_number_seq<sizeof...(Args1)>::type()
                                     , typename build_number_seq<sizeof...(Args2)>::type());
   }

#elif defined(AUTOBOOST_MSVC) && (_CPPLIB_VER == 520)

   //MSVC 2010 tuple implementation
   #define AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_MSVC2010_TUPLE_CODE(N,M)\
   template< typename ConstructAlloc, typename ArgAlloc, class Pair\
           , template<class, class, class, class, class, class, class, class, class, class> class StdTuple\
            AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_OR(N,M)) AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_AND(N,M)) AUTOBOOST_MOVE_CLASSQ##M > \
   typename container_detail::enable_if< container_detail::is_pair<Pair> >::type\
      dispatch_uses_allocator(ConstructAlloc & construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, Pair* pair, piecewise_construct_t\
                           , StdTuple<AUTOBOOST_MOVE_TARG##N  AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(10,N),::std::tr1::_Nil)> p\
                           , StdTuple<AUTOBOOST_MOVE_TARGQ##M AUTOBOOST_MOVE_I##M AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(10,M),::std::tr1::_Nil)> q)\
   {\
      (void)p; (void)q;\
      (dispatch_uses_allocator)\
         (construct_alloc, arg_alloc, container_detail::addressof(pair->first) AUTOBOOST_MOVE_I_IF(N) AUTOBOOST_MOVE_GET_IDX##N);\
      AUTOBOOST_TRY{\
         (dispatch_uses_allocator)\
            (construct_alloc, arg_alloc, container_detail::addressof(pair->second) AUTOBOOST_MOVE_I_IF(M) AUTOBOOST_MOVE_GET_IDXQ##M);\
      }\
      AUTOBOOST_CATCH(...) {\
         allocator_traits<ConstructAlloc>::destroy(construct_alloc, container_detail::addressof(pair->first));\
         AUTOBOOST_RETHROW\
      }\
      AUTOBOOST_CATCH_END\
   }\
   //
   AUTOBOOST_MOVE_ITER2D_0TOMAX(9, AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_MSVC2010_TUPLE_CODE)
   #undef AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_MSVC2010_TUPLE_CODE

#elif defined(AUTOBOOST_MSVC) && (_CPPLIB_VER == 540)
   #if _VARIADIC_MAX >= 9
   #define AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_MAX_IT 9
   #else
   #define AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_MAX_IT AUTOBOOST_MOVE_ADD(_VARIADIC_MAX, 1)
   #endif

   //MSVC 2012 tuple implementation
   #define AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_CODE(N,M)\
   template< typename ConstructAlloc, typename ArgAlloc, class Pair\
            , template<AUTOBOOST_MOVE_REPEAT(_VARIADIC_MAX, class), class, class, class> class StdTuple \
            AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_OR(N,M)) AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_I_IF(AUTOBOOST_MOVE_AND(N,M)) AUTOBOOST_MOVE_CLASSQ##M > \
   typename container_detail::enable_if< container_detail::is_pair<Pair> >::type\
      dispatch_uses_allocator\
         ( ConstructAlloc & construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, Pair* pair, piecewise_construct_t\
         , StdTuple<AUTOBOOST_MOVE_TARG##N  AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(AUTOBOOST_MOVE_ADD(_VARIADIC_MAX, 3),N),::std::_Nil) > p\
         , StdTuple<AUTOBOOST_MOVE_TARGQ##M AUTOBOOST_MOVE_I##M AUTOBOOST_MOVE_REPEAT(AUTOBOOST_MOVE_SUB(AUTOBOOST_MOVE_ADD(_VARIADIC_MAX, 3),M),::std::_Nil) > q)\
   {\
      (void)p; (void)q;\
      (dispatch_uses_allocator)\
         (construct_alloc, arg_alloc, container_detail::addressof(pair->first) AUTOBOOST_MOVE_I_IF(N) AUTOBOOST_MOVE_GET_IDX##N);\
      AUTOBOOST_TRY{\
         (dispatch_uses_allocator)\
            (construct_alloc, arg_alloc, container_detail::addressof(pair->second) AUTOBOOST_MOVE_I_IF(M) AUTOBOOST_MOVE_GET_IDXQ##M);\
      }\
      AUTOBOOST_CATCH(...) {\
         allocator_traits<ConstructAlloc>::destroy(construct_alloc, container_detail::addressof(pair->first));\
         AUTOBOOST_RETHROW\
      }\
      AUTOBOOST_CATCH_END\
   }\
   //
   AUTOBOOST_MOVE_ITER2D_0TOMAX(AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_MAX_IT, AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_CODE)
   #undef AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_MSVC2010_TUPLE_CODE
   #undef AUTOBOOST_DISPATCH_USES_ALLOCATOR_PIECEWISE_CONSTRUCT_MSVC2012_TUPLE_MAX_IT

#endif   //!defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

template < typename ConstructAlloc
         , typename ArgAlloc
         , class Pair, class KeyType, class ... Args>
typename container_detail::enable_if< container_detail::is_pair<Pair>, void >::type
   dispatch_uses_allocator
   (ConstructAlloc & construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, Pair* p, try_emplace_t, AUTOBOOST_FWD_REF(KeyType) k, AUTOBOOST_FWD_REF(Args) ...args)
{
   (dispatch_uses_allocator)(construct_alloc, arg_alloc, container_detail::addressof(p->first), ::autoboost::forward<KeyType>(k));
   AUTOBOOST_TRY{
      (dispatch_uses_allocator)(construct_alloc, arg_alloc, container_detail::addressof(p->second), ::autoboost::forward<Args>(args)...);
   }
   AUTOBOOST_CATCH(...) {
      allocator_traits<ConstructAlloc>::destroy(construct_alloc, container_detail::addressof(p->first));
      AUTOBOOST_RETHROW
   }
   AUTOBOOST_CATCH_END
}

#else

#define AUTOBOOST_CONTAINER_DISPATCH_USES_ALLOCATOR_PAIR_TRY_EMPLACE_CODE(N) \
   template <typename ConstructAlloc, typename ArgAlloc, class Pair, class KeyType AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N >\
   inline typename container_detail::enable_if\
      < container_detail::is_pair<Pair>, void >::type\
      dispatch_uses_allocator\
      (ConstructAlloc &construct_alloc, AUTOBOOST_FWD_REF(ArgAlloc) arg_alloc, Pair* p, try_emplace_t, \
       AUTOBOOST_FWD_REF(KeyType) k AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_UREF##N)\
   {\
      (dispatch_uses_allocator)(construct_alloc, arg_alloc, container_detail::addressof(p->first), ::autoboost::forward<KeyType>(k));\
      AUTOBOOST_TRY{\
         (dispatch_uses_allocator)(construct_alloc, arg_alloc, container_detail::addressof(p->second) AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_FWD##N);\
      }\
      AUTOBOOST_CATCH(...) {\
         allocator_traits<ConstructAlloc>::destroy(construct_alloc, container_detail::addressof(p->first));\
         AUTOBOOST_RETHROW\
      }\
      AUTOBOOST_CATCH_END\
   }\
//
AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_CONTAINER_DISPATCH_USES_ALLOCATOR_PAIR_TRY_EMPLACE_CODE)
#undef AUTOBOOST_CONTAINER_DISPATCH_USES_ALLOCATOR_PAIR_TRY_EMPLACE_CODE

#endif   //!defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

}  //namespace container_detail

}} // namespace autoboost { namespace container {

#include <autoboost/container/detail/config_end.hpp>

#endif //  AUTOBOOST_CONTAINER_DISPATCH_USES_ALLOCATOR_HPP

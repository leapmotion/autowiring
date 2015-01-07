//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_DETAIL_PREPROCESSOR_HPP
#define AUTOBOOST_CONTAINER_DETAIL_PREPROCESSOR_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>
#include <autoboost/move/utility_core.hpp>

#ifdef AUTOBOOST_CONTAINER_PERFECT_FORWARDING
//#error "This file is not needed when perfect forwarding is available"
#endif   //AUTOBOOST_CONTAINER_PERFECT_FORWARDING

#include <autoboost/preprocessor/iteration/local.hpp>
#include <autoboost/preprocessor/punctuation/paren_if.hpp>
#include <autoboost/preprocessor/punctuation/comma_if.hpp>
#include <autoboost/preprocessor/control/expr_if.hpp>
#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/repetition/enum.hpp>
#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing.hpp>
#include <autoboost/preprocessor/repetition/repeat.hpp>
#include <autoboost/preprocessor/arithmetic/sub.hpp>
#include <autoboost/preprocessor/arithmetic/add.hpp>
#include <autoboost/preprocessor/iteration/iterate.hpp>

#define AUTOBOOST_CONTAINER_MAX_CONSTRUCTOR_PARAMETERS 10

//Note:
//We define template parameters as const references to
//be able to bind temporaries. After that we will un-const them.
//This cast is ugly but it is necessary until "perfect forwarding"
//is achieved in C++0x. Meanwhile, if we want to be able to
//bind rvalues with non-const references, we have to be ugly
#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
   #define AUTOBOOST_CONTAINER_PP_PARAM_LIST(z, n, data) \
   AUTOBOOST_PP_CAT(P, n) && AUTOBOOST_PP_CAT(p, n) \
   //!
#else
   #define AUTOBOOST_CONTAINER_PP_PARAM_LIST(z, n, data) \
   const AUTOBOOST_PP_CAT(P, n) & AUTOBOOST_PP_CAT(p, n) \
   //!
#endif   //#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#define AUTOBOOST_CONTAINER_PP_CONST_REF_PARAM_LIST_Q(z, n, Data) \
const AUTOBOOST_PP_CAT(Q, n) & AUTOBOOST_PP_CAT(q, n) \
//!

#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
   #define AUTOBOOST_CONTAINER_PP_PARAM(U, u) \
   U && u \
   //!
#else
   #define AUTOBOOST_CONTAINER_PP_PARAM(U, u) \
   const U & u \
   //!
#endif   //#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

   #define AUTOBOOST_CONTAINER_PP_PARAM_INIT(z, n, data) \
   AUTOBOOST_PP_CAT(m_p, n) (::autoboost::forward< AUTOBOOST_PP_CAT(P, n) >( AUTOBOOST_PP_CAT(p, n) ))           \
   //!

#else //AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

   #define AUTOBOOST_CONTAINER_PP_PARAM_INIT(z, n, data) \
   AUTOBOOST_PP_CAT(m_p, n) (const_cast<AUTOBOOST_PP_CAT(P, n) &>(AUTOBOOST_PP_CAT(p, n))) \
   //!
#endif   //#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

   #if defined(AUTOBOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG)

      namespace autoboost {
      namespace container {
      namespace container_detail {
         template<class T>
         struct ref_holder;

         template<class T>
         struct ref_holder<T &>
         {
            explicit ref_holder(T &t)
               : t_(t)
            {}
            T &t_;
            T & get() {  return t_;   }
         };

         template<class T>
         struct ref_holder<const T>
         {
            explicit ref_holder(const T &t)
               : t_(t)
            {}
            const T &t_;
            const T & get() {  return t_;   }
         };

         template<class T>
         struct ref_holder<const T &&>
         {
            explicit ref_holder(const T &t)
               : t_(t)
            {}
            const T &t_;
            const T & get() {  return t_;   }
         };

         template<class T>
         struct ref_holder
         {
            explicit ref_holder(T &&t)
               : t_(t)
            {}
            T &t_;
            T && get() {  return ::autoboost::move(t_);   }
         };

         template<class T>
         struct ref_holder<T &&>
         {
            explicit ref_holder(T &&t)
               : t_(t)
            {}
            T &t_;
            T && get()  { return ::autoboost::move(t_); }
         };

      }  //namespace container_detail {
      }  //namespace container {
      }  //namespace autoboost {

      #define AUTOBOOST_CONTAINER_PP_PARAM_DEFINE(z, n, data)  \
         ::autoboost::container::container_detail::ref_holder<AUTOBOOST_PP_CAT(P, n)> AUTOBOOST_PP_CAT(m_p, n);  \
      //!

   #else //AUTOBOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG

      #define AUTOBOOST_CONTAINER_PP_PARAM_DEFINE(z, n, data)  \
      AUTOBOOST_PP_CAT(P, n) && AUTOBOOST_PP_CAT(m_p, n);            \
      //!

   #endif //defined(AUTOBOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG)

#else //AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

   #define AUTOBOOST_CONTAINER_PP_PARAM_DEFINE(z, n, data)  \
   AUTOBOOST_PP_CAT(P, n) & AUTOBOOST_PP_CAT(m_p, n);             \
   //!
#endif   //#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && defined(AUTOBOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG)

   #define AUTOBOOST_CONTAINER_PP_MEMBER_FORWARD(z, n, data) AUTOBOOST_PP_CAT(this->m_p, n).get() \
   //!

#else //!defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && defined(AUTOBOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG)

   #define AUTOBOOST_CONTAINER_PP_MEMBER_FORWARD(z, n, data) \
   ::autoboost::forward< AUTOBOOST_PP_CAT(P, n) >( AUTOBOOST_PP_CAT(this->m_p, n) ) \
   //!

#endif   //!defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && defined(AUTOBOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG)

#define AUTOBOOST_CONTAINER_PP_PARAM_INC(z, n, data)   \
  AUTOBOOST_PP_CAT(++this->m_p, n)                     \
//!

#define AUTOBOOST_CONTAINER_PP_IDENTITY(z, n, data) data


#define AUTOBOOST_CONTAINER_PP_PARAM_FORWARD(z, n, data) \
::autoboost::forward< AUTOBOOST_PP_CAT(P, n) >( AUTOBOOST_PP_CAT(p, n) ) \
//!

#define AUTOBOOST_CONTAINER_PP_DECLVAL(z, n, data) \
::autoboost::move_detail::declval< AUTOBOOST_PP_CAT(P, n) >() \
//!

#define AUTOBOOST_CONTAINER_PP_MEMBER_IT_FORWARD(z, n, data) \
AUTOBOOST_PP_CAT(*this->m_p, n) \
//!

#define AUTOBOOST_CONTAINER_PP_TEMPLATE_PARAM_VOID_DEFAULT(z, n, data)   \
  AUTOBOOST_PP_CAT(class P, n) = void                                    \
//!

#define AUTOBOOST_CONTAINER_PP_TEMPLATE_PARAM_WITH_DEFAULT(z, n, default_type) \
  AUTOBOOST_PP_CAT(class P, n) = default_type                                  \
//!

#define AUTOBOOST_CONTAINER_PP_STATIC_PARAM_REF_DECLARE(z, n, data) \
   static AUTOBOOST_PP_CAT(P, n) & AUTOBOOST_PP_CAT(p, n); \
//!

#define AUTOBOOST_CONTAINER_PP_PARAM_PASS(z, n, data) \
   AUTOBOOST_PP_CAT(p, n) \
//!

#define AUTOBOOST_CONTAINER_PP_FWD_TYPE(z, n, data) \
   typename ::autoboost::move_detail::forward_type< AUTOBOOST_PP_CAT(P, n) >::type \
//!

#include <autoboost/container/detail/config_end.hpp>

//#else

//#ifdef AUTOBOOST_CONTAINER_PERFECT_FORWARDING
//#error "This file is not needed when perfect forwarding is available"
//#endif   //AUTOBOOST_CONTAINER_PERFECT_FORWARDING

#endif //#ifndef AUTOBOOST_CONTAINER_DETAIL_PREPROCESSOR_HPP

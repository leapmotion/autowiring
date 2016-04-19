//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2012-2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////

//! \file
//! This header implements macros to define movable classes and
//! move-aware functions

#ifndef AUTOBOOST_MOVE_CORE_HPP
#define AUTOBOOST_MOVE_CORE_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/move/detail/config_begin.hpp>
#include <autoboost/move/detail/workaround.hpp>

// @cond

//autoboost_move_no_copy_constructor_or_assign typedef
//used to detect noncopyable types for other Boost libraries.
#if defined(AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS) || defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
   #define AUTOBOOST_MOVE_IMPL_NO_COPY_CTOR_OR_ASSIGN(TYPE) \
      private:\
      TYPE(TYPE &);\
      TYPE& operator=(TYPE &);\
      public:\
      typedef int autoboost_move_no_copy_constructor_or_assign; \
      private:\
   //
#else
   #define AUTOBOOST_MOVE_IMPL_NO_COPY_CTOR_OR_ASSIGN(TYPE) \
      public:\
      TYPE(TYPE const &) = delete;\
      TYPE& operator=(TYPE const &) = delete;\
      public:\
      typedef int autoboost_move_no_copy_constructor_or_assign; \
      private:\
   //
#endif   //AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS

// @endcond

#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

   #include <autoboost/move/detail/type_traits.hpp>

   #if defined(AUTOBOOST_MOVE_ADDRESS_SANITIZER_ON)
      #define AUTOBOOST_MOVE_TO_RV_CAST(RV_TYPE, ARG) reinterpret_cast<RV_TYPE>(ARG)
   #else
      #define AUTOBOOST_MOVE_TO_RV_CAST(RV_TYPE, ARG) static_cast<RV_TYPE>(ARG)
   #endif

   //Move emulation rv breaks standard aliasing rules so add workarounds for some compilers
   #if defined(__GNUC__) && (__GNUC__ >= 4) && \
      (\
         defined(AUTOBOOST_GCC) ||   \
         (defined(AUTOBOOST_INTEL) && (AUTOBOOST_INTEL_CXX_VERSION >= 1300)) \
      )
      #define AUTOBOOST_MOVE_ATTRIBUTE_MAY_ALIAS __attribute__((__may_alias__))
   #else
      #define AUTOBOOST_MOVE_ATTRIBUTE_MAY_ALIAS
   #endif

   namespace autoboost {

   //////////////////////////////////////////////////////////////////////////////
   //
   //                            struct rv
   //
   //////////////////////////////////////////////////////////////////////////////
   template <class T>
   class rv
      : public ::autoboost::move_detail::if_c
         < ::autoboost::move_detail::is_class<T>::value
         , T
         , ::autoboost::move_detail::nat
         >::type
   {
      rv();
      ~rv() throw();
      rv(rv const&);
      void operator=(rv const&);
   } AUTOBOOST_MOVE_ATTRIBUTE_MAY_ALIAS;


   //////////////////////////////////////////////////////////////////////////////
   //
   //                            is_rv
   //
   //////////////////////////////////////////////////////////////////////////////

   namespace move_detail {

   template <class T>
   struct is_rv
        //Derive from integral constant because some Boost code assummes it has
        //a "type" internal typedef
      : integral_constant<bool, ::autoboost::move_detail::is_rv_impl<T>::value >
   {};

   template <class T>
   struct is_not_rv
   {
      static const bool value = !is_rv<T>::value;
   };

   }  //namespace move_detail {

   //////////////////////////////////////////////////////////////////////////////
   //
   //                               has_move_emulation_enabled
   //
   //////////////////////////////////////////////////////////////////////////////
   template<class T>
   struct has_move_emulation_enabled
      : ::autoboost::move_detail::has_move_emulation_enabled_impl<T>
   {};

   template<class T>
   struct has_move_emulation_disabled
   {
      static const bool value = !::autoboost::move_detail::has_move_emulation_enabled_impl<T>::value;
   };

   }  //namespace autoboost {

   #define AUTOBOOST_RV_REF(TYPE)\
      ::autoboost::rv< TYPE >& \
   //

   #define AUTOBOOST_RV_REF_2_TEMPL_ARGS(TYPE, ARG1, ARG2)\
      ::autoboost::rv< TYPE<ARG1, ARG2> >& \
   //

   #define AUTOBOOST_RV_REF_3_TEMPL_ARGS(TYPE, ARG1, ARG2, ARG3)\
      ::autoboost::rv< TYPE<ARG1, ARG2, ARG3> >& \
   //

   #define AUTOBOOST_RV_REF_BEG\
      ::autoboost::rv<   \
   //

   #define AUTOBOOST_RV_REF_END\
      >& \
   //

   #define AUTOBOOST_RV_REF_BEG_IF_CXX11 \
      \
   //

   #define AUTOBOOST_RV_REF_END_IF_CXX11 \
      \
   //

   #define AUTOBOOST_FWD_REF(TYPE)\
      const TYPE & \
   //

   #define AUTOBOOST_COPY_ASSIGN_REF(TYPE)\
      const ::autoboost::rv< TYPE >& \
   //

   #define AUTOBOOST_COPY_ASSIGN_REF_BEG \
      const ::autoboost::rv<  \
   //

   #define AUTOBOOST_COPY_ASSIGN_REF_END \
      >& \
   //

   #define AUTOBOOST_COPY_ASSIGN_REF_2_TEMPL_ARGS(TYPE, ARG1, ARG2)\
      const ::autoboost::rv< TYPE<ARG1, ARG2> >& \
   //

   #define AUTOBOOST_COPY_ASSIGN_REF_3_TEMPL_ARGS(TYPE, ARG1, ARG2, ARG3)\
      const ::autoboost::rv< TYPE<ARG1, ARG2, ARG3> >& \
   //

   #define AUTOBOOST_CATCH_CONST_RLVALUE(TYPE)\
      const ::autoboost::rv< TYPE >& \
   //

   namespace autoboost {
   namespace move_detail {

   template <class Ret, class T>
   inline typename ::autoboost::move_detail::enable_if_c
      <  ::autoboost::move_detail::is_lvalue_reference<Ret>::value ||
        !::autoboost::has_move_emulation_enabled<T>::value
      , T&>::type
         move_return(T& x) AUTOBOOST_NOEXCEPT
   {
      return x;
   }

   template <class Ret, class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < !::autoboost::move_detail::is_lvalue_reference<Ret>::value &&
         ::autoboost::has_move_emulation_enabled<T>::value
      , ::autoboost::rv<T>&>::type
         move_return(T& x) AUTOBOOST_NOEXCEPT
   {
      return *AUTOBOOST_MOVE_TO_RV_CAST(::autoboost::rv<T>*, ::autoboost::move_detail::addressof(x));
   }

   template <class Ret, class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < !::autoboost::move_detail::is_lvalue_reference<Ret>::value &&
         ::autoboost::has_move_emulation_enabled<T>::value
      , ::autoboost::rv<T>&>::type
         move_return(::autoboost::rv<T>& x) AUTOBOOST_NOEXCEPT
   {
      return x;
   }

   }  //namespace move_detail {
   }  //namespace autoboost {

   #define AUTOBOOST_MOVE_RET(RET_TYPE, REF)\
      autoboost::move_detail::move_return< RET_TYPE >(REF)
   //

   #define AUTOBOOST_MOVE_BASE(BASE_TYPE, ARG) \
      ::autoboost::move((BASE_TYPE&)(ARG))
   //

   //////////////////////////////////////////////////////////////////////////////
   //
   //                         AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE
   //
   //////////////////////////////////////////////////////////////////////////////
   #define AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE(TYPE)\
      AUTOBOOST_MOVE_IMPL_NO_COPY_CTOR_OR_ASSIGN(TYPE)\
      public:\
      operator ::autoboost::rv<TYPE>&() \
      {  return *AUTOBOOST_MOVE_TO_RV_CAST(::autoboost::rv<TYPE>*, this);  }\
      operator const ::autoboost::rv<TYPE>&() const \
      {  return *AUTOBOOST_MOVE_TO_RV_CAST(const ::autoboost::rv<TYPE>*, this);  }\
      private:\
   //

   //////////////////////////////////////////////////////////////////////////////
   //
   //                         AUTOBOOST_COPYABLE_AND_MOVABLE
   //
   //////////////////////////////////////////////////////////////////////////////

   #define AUTOBOOST_COPYABLE_AND_MOVABLE(TYPE)\
      public:\
      TYPE& operator=(TYPE &t)\
      {  this->operator=(const_cast<const TYPE &>(t)); return *this;}\
      public:\
      operator ::autoboost::rv<TYPE>&() \
      {  return *AUTOBOOST_MOVE_TO_RV_CAST(::autoboost::rv<TYPE>*, this);  }\
      operator const ::autoboost::rv<TYPE>&() const \
      {  return *AUTOBOOST_MOVE_TO_RV_CAST(const ::autoboost::rv<TYPE>*, this);  }\
      private:\
   //

   #define AUTOBOOST_COPYABLE_AND_MOVABLE_ALT(TYPE)\
      public:\
      operator ::autoboost::rv<TYPE>&() \
      {  return *AUTOBOOST_MOVE_TO_RV_CAST(::autoboost::rv<TYPE>*, this);  }\
      operator const ::autoboost::rv<TYPE>&() const \
      {  return *AUTOBOOST_MOVE_TO_RV_CAST(const ::autoboost::rv<TYPE>*, this);  }\
      private:\
   //

   namespace autoboost{
   namespace move_detail{

   template< class T>
   struct forward_type
   { typedef const T &type; };

   template< class T>
   struct forward_type< autoboost::rv<T> >
   { typedef T type; };

   }}

#else    //AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

   //! This macro marks a type as movable but not copyable, disabling copy construction
   //! and assignment. The user will need to write a move constructor/assignment as explained
   //! in the documentation to fully write a movable but not copyable class.
   #define AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE(TYPE)\
      AUTOBOOST_MOVE_IMPL_NO_COPY_CTOR_OR_ASSIGN(TYPE)\
      public:\
      typedef int autoboost_move_emulation_t;\
   //

   //! This macro marks a type as copyable and movable.
   //! The user will need to write a move constructor/assignment and a copy assignment
   //! as explained in the documentation to fully write a copyable and movable class.
   #define AUTOBOOST_COPYABLE_AND_MOVABLE(TYPE)\
   //

   #if !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)
   #define AUTOBOOST_COPYABLE_AND_MOVABLE_ALT(TYPE)\
   //
   #endif   //#if !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

   namespace autoboost {

   //!This trait yields to a compile-time true boolean if T was marked as
   //!AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE or AUTOBOOST_COPYABLE_AND_MOVABLE and
   //!rvalue references are not available on the platform. False otherwise.
   template<class T>
   struct has_move_emulation_enabled
   {
      static const bool value = false;
   };

   template<class T>
   struct has_move_emulation_disabled
   {
      static const bool value = true;
   };

   }  //namespace autoboost{

   //!This macro is used to achieve portable syntax in move
   //!constructors and assignments for classes marked as
   //!AUTOBOOST_COPYABLE_AND_MOVABLE or AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE
   #define AUTOBOOST_RV_REF(TYPE)\
      TYPE && \
   //

   //!This macro is used to achieve portable syntax in move
   //!constructors and assignments for template classes marked as
   //!AUTOBOOST_COPYABLE_AND_MOVABLE or AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE.
   //!As macros have problems with comma-separated template arguments,
   //!the template argument must be preceded with AUTOBOOST_RV_REF_BEG
   //!and ended with AUTOBOOST_RV_REF_END
   #define AUTOBOOST_RV_REF_BEG\
         \
   //

   //!This macro is used to achieve portable syntax in move
   //!constructors and assignments for template classes marked as
   //!AUTOBOOST_COPYABLE_AND_MOVABLE or AUTOBOOST_MOVABLE_BUT_NOT_COPYABLE.
   //!As macros have problems with comma-separated template arguments,
   //!the template argument must be preceded with AUTOBOOST_RV_REF_BEG
   //!and ended with AUTOBOOST_RV_REF_END
   #define AUTOBOOST_RV_REF_END\
      && \
   //

   //!This macro expands to AUTOBOOST_RV_REF_BEG if AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
   //!is not defined, empty otherwise
   #define AUTOBOOST_RV_REF_BEG_IF_CXX11 \
      AUTOBOOST_RV_REF_BEG \
   //

   //!This macro expands to AUTOBOOST_RV_REF_END if AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
   //!is not defined, empty otherwise
   #define AUTOBOOST_RV_REF_END_IF_CXX11 \
      AUTOBOOST_RV_REF_END \
   //

   //!This macro is used to achieve portable syntax in copy
   //!assignment for classes marked as AUTOBOOST_COPYABLE_AND_MOVABLE.
   #define AUTOBOOST_COPY_ASSIGN_REF(TYPE)\
      const TYPE & \
   //

   //! This macro is used to implement portable perfect forwarding
   //! as explained in the documentation.
   #define AUTOBOOST_FWD_REF(TYPE)\
      TYPE && \
   //

   #if !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

   #define AUTOBOOST_RV_REF_2_TEMPL_ARGS(TYPE, ARG1, ARG2)\
      TYPE<ARG1, ARG2> && \
   //

   #define AUTOBOOST_RV_REF_3_TEMPL_ARGS(TYPE, ARG1, ARG2, ARG3)\
      TYPE<ARG1, ARG2, ARG3> && \
   //

   #define AUTOBOOST_COPY_ASSIGN_REF_BEG \
      const \
   //

   #define AUTOBOOST_COPY_ASSIGN_REF_END \
      & \
   //

   #define AUTOBOOST_COPY_ASSIGN_REF_2_TEMPL_ARGS(TYPE, ARG1, ARG2)\
      const TYPE<ARG1, ARG2> & \
   //

   #define AUTOBOOST_COPY_ASSIGN_REF_3_TEMPL_ARGS(TYPE, ARG1, ARG2, ARG3)\
      const TYPE<ARG1, ARG2, ARG3>& \
   //

   #define AUTOBOOST_CATCH_CONST_RLVALUE(TYPE)\
      const TYPE & \
   //

   #endif   //#if !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

   #if !defined(AUTOBOOST_MOVE_MSVC_AUTO_MOVE_RETURN_BUG) || defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

      //!This macro is used to achieve portable move return semantics.
      //!The C++11 Standard allows implicit move returns when the object to be returned
      //!is designated by a lvalue and:
      //!   - The criteria for elision of a copy operation are met OR
      //!   - The criteria would be met save for the fact that the source object is a function parameter
      //!
      //!For C++11 conforming compilers this macros only yields to REF:
      //! <code>return AUTOBOOST_MOVE_RET(RET_TYPE, REF);</code> -> <code>return REF;</code>
      //!
      //!For compilers without rvalue references
      //!this macro does an explicit move if the move emulation is activated
      //!and the return type (RET_TYPE) is not a reference.
      //!
      //!For non-conforming compilers with rvalue references like Visual 2010 & 2012,
      //!an explicit move is performed if RET_TYPE is not a reference.
      //!
      //! <b>Caution</b>: When using this macro in non-conforming or C++03
      //!compilers, a move will be performed even if the C++11 standard does not allow it
      //!(e.g. returning a static variable). The user is responsible for using this macro
      //!only to return local objects that met C++11 criteria.
      #define AUTOBOOST_MOVE_RET(RET_TYPE, REF)\
         REF
      //

   #else //!defined(AUTOBOOST_MOVE_MSVC_AUTO_MOVE_RETURN_BUG) || defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

      #include <autoboost/move/detail/meta_utils.hpp>

      namespace autoboost {
      namespace move_detail {

      template <class Ret, class T>
      inline typename ::autoboost::move_detail::enable_if_c
         <  ::autoboost::move_detail::is_lvalue_reference<Ret>::value
         , T&>::type
            move_return(T& x) AUTOBOOST_NOEXCEPT
      {
         return x;
      }

      template <class Ret, class T>
      inline typename ::autoboost::move_detail::enable_if_c
         < !::autoboost::move_detail::is_lvalue_reference<Ret>::value
         , Ret && >::type
            move_return(T&& t) AUTOBOOST_NOEXCEPT
      {
         return static_cast< Ret&& >(t);
      }

      }  //namespace move_detail {
      }  //namespace autoboost {

      #define AUTOBOOST_MOVE_RET(RET_TYPE, REF)\
         autoboost::move_detail::move_return< RET_TYPE >(REF)
      //

   #endif   //!defined(AUTOBOOST_MOVE_MSVC_AUTO_MOVE_RETURN_BUG) || defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

   //!This macro is used to achieve portable optimal move constructors.
   //!
   //!When implementing the move constructor, in C++03 compilers the moved-from argument must be
   //!cast to the base type before calling `::autoboost::move()` due to rvalue reference limitations.
   //!
   //!In C++11 compilers the cast from a rvalue reference of a derived type to a rvalue reference of
   //!a base type is implicit.
   #define AUTOBOOST_MOVE_BASE(BASE_TYPE, ARG) \
      ::autoboost::move((BASE_TYPE&)(ARG))
   //

   namespace autoboost {
   namespace move_detail {

   template< class T> struct forward_type { typedef T type; };

   }}

#endif   //AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#include <autoboost/move/detail/config_end.hpp>

#endif //#ifndef AUTOBOOST_MOVE_CORE_HPP

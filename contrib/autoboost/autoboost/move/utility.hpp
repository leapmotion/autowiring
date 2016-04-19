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
//! This header includes core utilities from <tt><autoboost/move/utility_core.hpp></tt> and defines
//! some more advanced utilities such as:

#ifndef AUTOBOOST_MOVE_MOVE_UTILITY_HPP
#define AUTOBOOST_MOVE_MOVE_UTILITY_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/move/detail/config_begin.hpp>
#include <autoboost/move/utility_core.hpp>
#include <autoboost/move/traits.hpp>

#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

   namespace autoboost {

   //////////////////////////////////////////////////////////////////////////////
   //
   //                            move_if_noexcept()
   //
   //////////////////////////////////////////////////////////////////////////////

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && !has_move_emulation_enabled<T>::value
      , typename ::autoboost::move_detail::add_const<T>::type &
      >::type
         move_if_noexcept(T& x) AUTOBOOST_NOEXCEPT
   {
      return x;
   }

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && has_move_emulation_enabled<T>::value
            && ::autoboost::move_detail::is_nothrow_move_constructible_or_uncopyable<T>::value, rv<T>&>::type
         move_if_noexcept(T& x) AUTOBOOST_NOEXCEPT
   {
      return *static_cast<rv<T>* >(::autoboost::move_detail::addressof(x));
   }

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && has_move_emulation_enabled<T>::value
            && ::autoboost::move_detail::is_nothrow_move_constructible_or_uncopyable<T>::value
      , rv<T>&
      >::type
         move_if_noexcept(rv<T>& x) AUTOBOOST_NOEXCEPT
   {
      return x;
   }

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && has_move_emulation_enabled<T>::value
            && !::autoboost::move_detail::is_nothrow_move_constructible_or_uncopyable<T>::value
      , typename ::autoboost::move_detail::add_const<T>::type &
      >::type
         move_if_noexcept(T& x) AUTOBOOST_NOEXCEPT
   {
      return x;
   }

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && has_move_emulation_enabled<T>::value
            && !::autoboost::move_detail::is_nothrow_move_constructible_or_uncopyable<T>::value
      , typename ::autoboost::move_detail::add_const<T>::type &
      >::type
         move_if_noexcept(rv<T>& x) AUTOBOOST_NOEXCEPT
   {
      return x;
   }

   }  //namespace autoboost

#else    //#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

   #if defined(AUTOBOOST_MOVE_USE_STANDARD_LIBRARY_MOVE)
      #include <utility>

      namespace autoboost{

      using ::std::move_if_noexcept;

      }  //namespace autoboost

   #else //!AUTOBOOST_MOVE_USE_STANDARD_LIBRARY_MOVE

      namespace autoboost {

      //////////////////////////////////////////////////////////////////////////////
      //
      //                            move_if_noexcept()
      //
      //////////////////////////////////////////////////////////////////////////////
      #if defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)
         //! This function provides a way to convert a reference into a rvalue reference
         //! in compilers with rvalue references. For other compilers converts T & into
         //! <i>::autoboost::rv<T> &</i> so that move emulation is activated. Reference
         //! would be converted to rvalue reference only if input type is nothrow move
         //! constructible or if it has no copy constructor. In all other cases const
         //! reference would be returned
         template <class T>
         rvalue_reference_or_const_lvalue_reference move_if_noexcept(input_reference) noexcept;

      #else //AUTOBOOST_MOVE_DOXYGEN_INVOKED

         template <class T>
         typename ::autoboost::move_detail::enable_if_c
            < ::autoboost::move_detail::is_nothrow_move_constructible_or_uncopyable<T>::value, T&&>::type
               move_if_noexcept(T& x) AUTOBOOST_NOEXCEPT
         {  return ::autoboost::move(x);   }

         template <class T>
         typename ::autoboost::move_detail::enable_if_c
            < !::autoboost::move_detail::is_nothrow_move_constructible_or_uncopyable<T>::value, const T&>::type
               move_if_noexcept(T& x) AUTOBOOST_NOEXCEPT
         {  return x;  }

      #endif //AUTOBOOST_MOVE_DOXYGEN_INVOKED

      }  //namespace autoboost {

   #endif   //#if defined(AUTOBOOST_MOVE_USE_STANDARD_LIBRARY_MOVE)

#endif   //AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#include <autoboost/move/detail/config_end.hpp>

#endif //#ifndef AUTOBOOST_MOVE_MOVE_UTILITY_HPP

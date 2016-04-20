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
//! This header defines core utilities to ease the development
//! of move-aware functions. This header minimizes dependencies
//! from other libraries.

#ifndef AUTOBOOST_MOVE_MOVE_UTILITY_CORE_HPP
#define AUTOBOOST_MOVE_MOVE_UTILITY_CORE_HPP

#include <autoboost/move/detail/config_begin.hpp>
#include <autoboost/move/core.hpp>
#include <autoboost/move/detail/meta_utils.hpp>
#include <autoboost/static_assert.hpp>

#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

   namespace autoboost {

   template<class T>
   struct enable_move_utility_emulation
   {
      static const bool value = true;
   };

   //////////////////////////////////////////////////////////////////////////////
   //
   //                            move()
   //
   //////////////////////////////////////////////////////////////////////////////

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && !has_move_emulation_enabled<T>::value, T&>::type
         move(T& x) AUTOBOOST_NOEXCEPT
   {
      return x;
   }

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && has_move_emulation_enabled<T>::value, rv<T>&>::type
         move(T& x) AUTOBOOST_NOEXCEPT
   {
      return *static_cast<rv<T>* >(::autoboost::move_detail::addressof(x));
   }

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && has_move_emulation_enabled<T>::value, rv<T>&>::type
         move(rv<T>& x) AUTOBOOST_NOEXCEPT
   {
      return x;
   }

   //////////////////////////////////////////////////////////////////////////////
   //
   //                            forward()
   //
   //////////////////////////////////////////////////////////////////////////////

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && ::autoboost::move_detail::is_rv<T>::value, T &>::type
         forward(const typename ::autoboost::move_detail::identity<T>::type &x) AUTOBOOST_NOEXCEPT
   {
      return const_cast<T&>(x);
   }

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && !::autoboost::move_detail::is_rv<T>::value, const T &>::type
      forward(const typename ::autoboost::move_detail::identity<T>::type &x) AUTOBOOST_NOEXCEPT
   {
      return x;
   }

   //////////////////////////////////////////////////////////////////////////////
   //
   //                        move_if_not_lvalue_reference()
   //
   //////////////////////////////////////////////////////////////////////////////

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value &&
        ::autoboost::move_detail::is_rv<T>::value
      , T &>::type
         move_if_not_lvalue_reference(const typename ::autoboost::move_detail::identity<T>::type &x) AUTOBOOST_NOEXCEPT
   {
      return const_cast<T&>(x);
   }

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value &&
        !::autoboost::move_detail::is_rv<T>::value  &&
        (::autoboost::move_detail::is_lvalue_reference<T>::value ||
         !has_move_emulation_enabled<T>::value)
      , typename ::autoboost::move_detail::add_lvalue_reference<T>::type
      >::type
         move_if_not_lvalue_reference(typename ::autoboost::move_detail::remove_reference<T>::type &x) AUTOBOOST_NOEXCEPT
   {
      return x;
   }

   template <class T>
   inline typename ::autoboost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value &&
        !::autoboost::move_detail::is_rv<T>::value  &&
        (!::autoboost::move_detail::is_lvalue_reference<T>::value &&
         has_move_emulation_enabled<T>::value)
      , rv<T>&
      >::type
         move_if_not_lvalue_reference(typename ::autoboost::move_detail::remove_reference<T>::type &x) AUTOBOOST_NOEXCEPT
   {
      return move(x);
   }

   }  //namespace autoboost

#else    //#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

   #if defined(AUTOBOOST_MOVE_USE_STANDARD_LIBRARY_MOVE)
      #include <utility>

      namespace autoboost{

      using ::std::move;
      using ::std::forward;

      }  //namespace autoboost

   #else //!AUTOBOOST_MOVE_USE_STANDARD_LIBRARY_MOVE

      namespace autoboost {

      //! This trait's internal boolean `value` is false in compilers with rvalue references
      //! and true in compilers without rvalue references.
      //!
      //! A user can specialize this trait for a type T to false to SFINAE out `move` and `forward`
      //! so that the user can define a different move emulation for that type in namespace autoboost
      //! (e.g. another Boost library for its types) and avoid any overload ambiguity.
      template<class T>
      struct enable_move_utility_emulation
      {
         static const bool value = false;
      };

      //////////////////////////////////////////////////////////////////////////////
      //
      //                                  move
      //
      //////////////////////////////////////////////////////////////////////////////

      #if defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)
         //! This function provides a way to convert a reference into a rvalue reference
         //! in compilers with rvalue references. For other compilers if `T` is Boost.Move
         //! enabled type then it converts `T&` into <tt>::autoboost::rv<T> &</tt> so that
         //! move emulation is activated, else it returns `T &`.
         template <class T>
         rvalue_reference move(input_reference) noexcept;

      #elif defined(AUTOBOOST_MOVE_OLD_RVALUE_REF_BINDING_RULES)

         //Old move approach, lvalues could bind to rvalue references
         template <class T>
         inline typename ::autoboost::move_detail::remove_reference<T>::type && move(T&& t) AUTOBOOST_NOEXCEPT
         {  return t;   }

      #else //AUTOBOOST_MOVE_OLD_RVALUE_REF_BINDING_RULES

         template <class T>
         inline typename ::autoboost::move_detail::remove_reference<T>::type && move(T&& t) AUTOBOOST_NOEXCEPT
         { return static_cast<typename ::autoboost::move_detail::remove_reference<T>::type &&>(t); }

      #endif   //AUTOBOOST_MOVE_OLD_RVALUE_REF_BINDING_RULES

      //////////////////////////////////////////////////////////////////////////////
      //
      //                                  forward
      //
      //////////////////////////////////////////////////////////////////////////////


      #if defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)
         //! This function provides limited form of forwarding that is usually enough for
         //! in-place construction and avoids the exponential overloading for
         //! achieve the limited forwarding in C++03.
         //!
         //! For compilers with rvalue references this function provides perfect forwarding.
         //!
         //! Otherwise:
         //! * If input_reference binds to const ::autoboost::rv<T> & then it output_reference is
         //!   ::autoboost::rv<T> &
         //!
         //! * Else, output_reference is equal to input_reference.
         template <class T> output_reference forward(input_reference) noexcept;
      #elif defined(AUTOBOOST_MOVE_OLD_RVALUE_REF_BINDING_RULES)

         //Old move approach, lvalues could bind to rvalue references

         template <class T>
         inline T&& forward(typename ::autoboost::move_detail::identity<T>::type&& t) AUTOBOOST_NOEXCEPT
         {  return t;   }

      #else //Old move

         template <class T>
         inline T&& forward(typename ::autoboost::move_detail::remove_reference<T>::type& t) AUTOBOOST_NOEXCEPT
         {  return static_cast<T&&>(t);   }

         template <class T>
         inline T&& forward(typename ::autoboost::move_detail::remove_reference<T>::type&& t) AUTOBOOST_NOEXCEPT
         {
            //"autoboost::forward<T> error: 'T' is a lvalue reference, can't forward as rvalue.";
            AUTOBOOST_STATIC_ASSERT(!autoboost::move_detail::is_lvalue_reference<T>::value);
            return static_cast<T&&>(t);
         }

      #endif   //AUTOBOOST_MOVE_DOXYGEN_INVOKED

      //////////////////////////////////////////////////////////////////////////////
      //
      //                         move_if_not_lvalue_reference
      //
      //////////////////////////////////////////////////////////////////////////////


      #if defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)
         template <class T> output_reference move_if_not_lvalue_reference(input_reference) noexcept;
      #elif defined(AUTOBOOST_MOVE_OLD_RVALUE_REF_BINDING_RULES)

         //Old move approach, lvalues could bind to rvalue references

         template <class T>
         inline T&& move_if_not_lvalue_reference(typename ::autoboost::move_detail::identity<T>::type&& t) AUTOBOOST_NOEXCEPT
         {  return t;   }

      #else //Old move

         template <class T>
         inline T&& move_if_not_lvalue_reference(typename ::autoboost::move_detail::remove_reference<T>::type& t) AUTOBOOST_NOEXCEPT
         {  return static_cast<T&&>(t);   }

         template <class T>
         inline T&& move_if_not_lvalue_reference(typename ::autoboost::move_detail::remove_reference<T>::type&& t) AUTOBOOST_NOEXCEPT
         {
            //"autoboost::forward<T> error: 'T' is a lvalue reference, can't forward as rvalue.";
            AUTOBOOST_STATIC_ASSERT(!autoboost::move_detail::is_lvalue_reference<T>::value);
            return static_cast<T&&>(t);
         }

      #endif   //AUTOBOOST_MOVE_DOXYGEN_INVOKED

      }  //namespace autoboost {

   #endif   //#if defined(AUTOBOOST_MOVE_USE_STANDARD_LIBRARY_MOVE)

#endif   //AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#if !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

namespace autoboost{
namespace move_detail{

template<class T>
void swap(T &a, T &b)
{
   T c((::autoboost::move(a)));
   a = ::autoboost::move(b);
   b = ::autoboost::move(c);
}

template <typename T>
typename autoboost::move_detail::add_rvalue_reference<T>::type declval();

}  //namespace move_detail{
}  //namespace autoboost{

#endif   //#if !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)


#include <autoboost/move/detail/config_end.hpp>

#endif //#ifndef AUTOBOOST_MOVE_MOVE_UTILITY_CORE_HPP

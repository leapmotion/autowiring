// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2007 Anthony Williams
// (C) Copyright 2011-2012 Vicente J. Botet Escriba

#ifndef AUTOBOOST_THREAD_LOCKABLE_TRAITS_HPP
#define AUTOBOOST_THREAD_LOCKABLE_TRAITS_HPP

#include <autoboost/thread/detail/config.hpp>

#include <autoboost/assert.hpp>
#include <autoboost/detail/workaround.hpp>
#include <autoboost/type_traits/is_class.hpp>

#include <autoboost/config/abi_prefix.hpp>

// todo make use of integral_constant, true_type and false_type

namespace autoboost
{
  namespace sync
  {

#if defined(AUTOBOOST_NO_SFINAE) ||                           \
    AUTOBOOST_WORKAROUND(__IBMCPP__, AUTOBOOST_TESTED_AT(600)) || \
    AUTOBOOST_WORKAROUND(__SUNPRO_CC, AUTOBOOST_TESTED_AT(0x590))
#if ! defined AUTOBOOST_THREAD_NO_AUTO_DETECT_MUTEX_TYPES
#define AUTOBOOST_THREAD_NO_AUTO_DETECT_MUTEX_TYPES
#endif
#endif

#ifndef AUTOBOOST_THREAD_NO_AUTO_DETECT_MUTEX_TYPES
    namespace detail
    {
#define AUTOBOOST_THREAD_DEFINE_HAS_MEMBER_CALLED(member_name)                     \
        template<typename T, bool=autoboost::is_class<T>::value>            \
        struct has_member_called_##member_name                          \
        {                                                               \
            AUTOBOOST_STATIC_CONSTANT(bool, value=false);                   \
        };                                                              \
                                                                        \
        template<typename T>                                            \
        struct has_member_called_##member_name<T,true>                  \
        {                                                               \
            typedef char true_type;                                     \
            struct false_type                                           \
            {                                                           \
                true_type dummy[2];                                     \
            };                                                          \
                                                                        \
            struct fallback { int member_name; };                       \
            struct derived:                                             \
                T, fallback                                             \
            {                                                           \
                derived();                                              \
            };                                                          \
                                                                        \
            template<int fallback::*> struct tester;                    \
                                                                        \
            template<typename U>                                        \
                static false_type has_member(tester<&U::member_name>*); \
            template<typename U>                                        \
                static true_type has_member(...);                       \
                                                                        \
            AUTOBOOST_STATIC_CONSTANT(                                      \
                bool, value=sizeof(has_member<derived>(0))==sizeof(true_type)); \
        }

      AUTOBOOST_THREAD_DEFINE_HAS_MEMBER_CALLED(lock)
;      AUTOBOOST_THREAD_DEFINE_HAS_MEMBER_CALLED(unlock);
      AUTOBOOST_THREAD_DEFINE_HAS_MEMBER_CALLED(try_lock);

      template<typename T,bool=has_member_called_lock<T>::value >
      struct has_member_lock
      {
        AUTOBOOST_STATIC_CONSTANT(bool, value=false);
      };

      template<typename T>
      struct has_member_lock<T,true>
      {
        typedef char true_type;
        struct false_type
        {
          true_type dummy[2];
        };

        template<typename U,typename V>
        static true_type has_member(V (U::*)());
        template<typename U>
        static false_type has_member(U);

        AUTOBOOST_STATIC_CONSTANT(
            bool,value=sizeof(has_member_lock<T>::has_member(&T::lock))==sizeof(true_type));
      };

      template<typename T,bool=has_member_called_unlock<T>::value >
      struct has_member_unlock
      {
        AUTOBOOST_STATIC_CONSTANT(bool, value=false);
      };

      template<typename T>
      struct has_member_unlock<T,true>
      {
        typedef char true_type;
        struct false_type
        {
          true_type dummy[2];
        };

        template<typename U,typename V>
        static true_type has_member(V (U::*)());
        template<typename U>
        static false_type has_member(U);

        AUTOBOOST_STATIC_CONSTANT(
            bool,value=sizeof(has_member_unlock<T>::has_member(&T::unlock))==sizeof(true_type));
      };

      template<typename T,bool=has_member_called_try_lock<T>::value >
      struct has_member_try_lock
      {
        AUTOBOOST_STATIC_CONSTANT(bool, value=false);
      };

      template<typename T>
      struct has_member_try_lock<T,true>
      {
        typedef char true_type;
        struct false_type
        {
          true_type dummy[2];
        };

        template<typename U>
        static true_type has_member(bool (U::*)());
        template<typename U>
        static false_type has_member(U);

        AUTOBOOST_STATIC_CONSTANT(
            bool,value=sizeof(has_member_try_lock<T>::has_member(&T::try_lock))==sizeof(true_type));
      };

    }

    template<typename T>
    struct is_basic_lockable
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = detail::has_member_lock<T>::value &&
          detail::has_member_unlock<T>::value);
    };
    template<typename T>
    struct is_lockable
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value =
          is_basic_lockable<T>::value &&
          detail::has_member_try_lock<T>::value);
    };

#else
    template<typename T>
    struct is_basic_lockable
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = false);
    };
    template<typename T>
    struct is_lockable
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = false);
    };
#endif

    template<typename T>
    struct is_recursive_mutex_sur_parole
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = false);
    };
    template<typename T>
    struct is_recursive_mutex_sur_parolle : is_recursive_mutex_sur_parole<T>
    {
    };

    template<typename T>
    struct is_recursive_basic_lockable
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = is_basic_lockable<T>::value &&
          is_recursive_mutex_sur_parolle<T>::value);
    };
    template<typename T>
    struct is_recursive_lockable
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = is_lockable<T>::value &&
          is_recursive_mutex_sur_parolle<T>::value);
    };
  }
  template<typename T>
  struct is_mutex_type
  {
    AUTOBOOST_STATIC_CONSTANT(bool, value = sync::is_lockable<T>::value);
  };

}
#include <autoboost/config/abi_suffix.hpp>

#endif

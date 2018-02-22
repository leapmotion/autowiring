// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2007-8 Anthony Williams
// (C) Copyright 2011-2012 Vicente J. Botet Escriba

#ifndef AUTOBOOST_THREAD_MOVE_HPP
#define AUTOBOOST_THREAD_MOVE_HPP

#include <autoboost/thread/detail/config.hpp>
#ifndef AUTOBOOST_NO_SFINAE
#include <autoboost/core/enable_if.hpp>
#include <autoboost/type_traits/is_convertible.hpp>
#include <autoboost/type_traits/remove_reference.hpp>
#include <autoboost/type_traits/remove_cv.hpp>
#include <autoboost/type_traits/decay.hpp>
#include <autoboost/type_traits/conditional.hpp>
#include <autoboost/type_traits/remove_extent.hpp>
#include <autoboost/type_traits/is_array.hpp>
#include <autoboost/type_traits/is_function.hpp>
#include <autoboost/type_traits/remove_cv.hpp>
#include <autoboost/type_traits/add_pointer.hpp>
#include <autoboost/type_traits/decay.hpp>
#endif

#include <autoboost/thread/detail/delete.hpp>
#include <autoboost/move/utility.hpp>
#include <autoboost/move/traits.hpp>
#include <autoboost/config/abi_prefix.hpp>
#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#include <type_traits>
#endif
namespace autoboost
{

    namespace detail
    {
      template <typename T>
      struct enable_move_utility_emulation_dummy_specialization;
        template<typename T>
        struct thread_move_t
        {
            T& t;
            explicit thread_move_t(T& t_):
                t(t_)
            {}

            T& operator*() const
            {
                return t;
            }

            T* operator->() const
            {
                return &t;
            }
        private:
            void operator=(thread_move_t&);
        };
    }

#if !defined AUTOBOOST_THREAD_USES_MOVE

#ifndef AUTOBOOST_NO_SFINAE
    template<typename T>
    typename enable_if<autoboost::is_convertible<T&,autoboost::detail::thread_move_t<T> >, autoboost::detail::thread_move_t<T> >::type move(T& t)
    {
        return autoboost::detail::thread_move_t<T>(t);
    }
#endif

    template<typename T>
    autoboost::detail::thread_move_t<T> move(autoboost::detail::thread_move_t<T> t)
    {
        return t;
    }

#endif   //#if !defined AUTOBOOST_THREAD_USES_MOVE
}

#if ! defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#define AUTOBOOST_THREAD_COPY_ASSIGN_REF(TYPE) AUTOBOOST_COPY_ASSIGN_REF(TYPE)
#define AUTOBOOST_THREAD_RV_REF(TYPE) AUTOBOOST_RV_REF(TYPE)
#define AUTOBOOST_THREAD_RV_REF_2_TEMPL_ARGS(TYPE) AUTOBOOST_RV_REF_2_TEMPL_ARGS(TYPE)
#define AUTOBOOST_THREAD_RV_REF_BEG AUTOBOOST_RV_REF_BEG
#define AUTOBOOST_THREAD_RV_REF_END AUTOBOOST_RV_REF_END
#define AUTOBOOST_THREAD_RV(V) V
#define AUTOBOOST_THREAD_MAKE_RV_REF(RVALUE) RVALUE
#define AUTOBOOST_THREAD_FWD_REF(TYPE) AUTOBOOST_FWD_REF(TYPE)
#define AUTOBOOST_THREAD_DCL_MOVABLE(TYPE)
#define AUTOBOOST_THREAD_DCL_MOVABLE_BEG(T) \
  namespace detail { \
    template <typename T> \
    struct enable_move_utility_emulation_dummy_specialization<

#define AUTOBOOST_THREAD_DCL_MOVABLE_BEG2(T1, T2) \
  namespace detail { \
    template <typename T1, typename T2> \
    struct enable_move_utility_emulation_dummy_specialization<

#define AUTOBOOST_THREAD_DCL_MOVABLE_END > \
      : integral_constant<bool, false> \
      {}; \
    }

#elif ! defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES && defined  AUTOBOOST_MSVC

#define AUTOBOOST_THREAD_COPY_ASSIGN_REF(TYPE) AUTOBOOST_COPY_ASSIGN_REF(TYPE)
#define AUTOBOOST_THREAD_RV_REF(TYPE) AUTOBOOST_RV_REF(TYPE)
#define AUTOBOOST_THREAD_RV_REF_2_TEMPL_ARGS(TYPE) AUTOBOOST_RV_REF_2_TEMPL_ARGS(TYPE)
#define AUTOBOOST_THREAD_RV_REF_BEG AUTOBOOST_RV_REF_BEG
#define AUTOBOOST_THREAD_RV_REF_END AUTOBOOST_RV_REF_END
#define AUTOBOOST_THREAD_RV(V) V
#define AUTOBOOST_THREAD_MAKE_RV_REF(RVALUE) RVALUE
#define AUTOBOOST_THREAD_FWD_REF(TYPE) AUTOBOOST_FWD_REF(TYPE)
#define AUTOBOOST_THREAD_DCL_MOVABLE(TYPE)
#define AUTOBOOST_THREAD_DCL_MOVABLE_BEG(T) \
  namespace detail { \
    template <typename T> \
    struct enable_move_utility_emulation_dummy_specialization<

#define AUTOBOOST_THREAD_DCL_MOVABLE_BEG2(T1, T2) \
  namespace detail { \
    template <typename T1, typename T2> \
    struct enable_move_utility_emulation_dummy_specialization<

#define AUTOBOOST_THREAD_DCL_MOVABLE_END > \
      : integral_constant<bool, false> \
      {}; \
    }

#else

#if defined AUTOBOOST_THREAD_USES_MOVE
#define AUTOBOOST_THREAD_COPY_ASSIGN_REF(TYPE) AUTOBOOST_COPY_ASSIGN_REF(TYPE)
#define AUTOBOOST_THREAD_RV_REF(TYPE) AUTOBOOST_RV_REF(TYPE)
#define AUTOBOOST_THREAD_RV_REF_2_TEMPL_ARGS(TYPE) AUTOBOOST_RV_REF_2_TEMPL_ARGS(TYPE)
#define AUTOBOOST_THREAD_RV_REF_BEG AUTOBOOST_RV_REF_BEG
#define AUTOBOOST_THREAD_RV_REF_END AUTOBOOST_RV_REF_END
#define AUTOBOOST_THREAD_RV(V) V
#define AUTOBOOST_THREAD_FWD_REF(TYPE) AUTOBOOST_FWD_REF(TYPE)
#define AUTOBOOST_THREAD_DCL_MOVABLE(TYPE)
#define AUTOBOOST_THREAD_DCL_MOVABLE_BEG(T) \
  namespace detail { \
    template <typename T> \
    struct enable_move_utility_emulation_dummy_specialization<

#define AUTOBOOST_THREAD_DCL_MOVABLE_BEG2(T1, T2) \
  namespace detail { \
    template <typename T1, typename T2> \
    struct enable_move_utility_emulation_dummy_specialization<

#define AUTOBOOST_THREAD_DCL_MOVABLE_END > \
      : integral_constant<bool, false> \
      {}; \
    }

#else

#define AUTOBOOST_THREAD_COPY_ASSIGN_REF(TYPE) const TYPE&
#define AUTOBOOST_THREAD_RV_REF(TYPE) autoboost::detail::thread_move_t< TYPE >
#define AUTOBOOST_THREAD_RV_REF_BEG autoboost::detail::thread_move_t<
#define AUTOBOOST_THREAD_RV_REF_END >
#define AUTOBOOST_THREAD_RV(V) (*V)
#define AUTOBOOST_THREAD_FWD_REF(TYPE) AUTOBOOST_FWD_REF(TYPE)

#define AUTOBOOST_THREAD_DCL_MOVABLE(TYPE) \
template <> \
struct enable_move_utility_emulation< TYPE > \
{ \
   static const bool value = false; \
};

#define AUTOBOOST_THREAD_DCL_MOVABLE_BEG(T) \
template <typename T> \
struct enable_move_utility_emulation<

#define AUTOBOOST_THREAD_DCL_MOVABLE_BEG2(T1, T2) \
template <typename T1, typename T2> \
struct enable_move_utility_emulation<

#define AUTOBOOST_THREAD_DCL_MOVABLE_END > \
{ \
   static const bool value = false; \
};

#endif

namespace autoboost
{
namespace detail
{
  template <typename T>
  AUTOBOOST_THREAD_RV_REF(typename ::autoboost::remove_cv<typename ::autoboost::remove_reference<T>::type>::type)
  make_rv_ref(T v)  AUTOBOOST_NOEXCEPT
  {
    return (AUTOBOOST_THREAD_RV_REF(typename ::autoboost::remove_cv<typename ::autoboost::remove_reference<T>::type>::type))(v);
  }
//  template <typename T>
//  AUTOBOOST_THREAD_RV_REF(typename ::autoboost::remove_cv<typename ::autoboost::remove_reference<T>::type>::type)
//  make_rv_ref(T &v)  AUTOBOOST_NOEXCEPT
//  {
//    return (AUTOBOOST_THREAD_RV_REF(typename ::autoboost::remove_cv<typename ::autoboost::remove_reference<T>::type>::type))(v);
//  }
//  template <typename T>
//  const AUTOBOOST_THREAD_RV_REF(typename ::autoboost::remove_cv<typename ::autoboost::remove_reference<T>::type>::type)
//  make_rv_ref(T const&v)  AUTOBOOST_NOEXCEPT
//  {
//    return (const AUTOBOOST_THREAD_RV_REF(typename ::autoboost::remove_cv<typename ::autoboost::remove_reference<T>::type>::type))(v);
//  }
}
}

#define AUTOBOOST_THREAD_MAKE_RV_REF(RVALUE) RVALUE.move()
//#define AUTOBOOST_THREAD_MAKE_RV_REF(RVALUE) autoboost::detail::make_rv_ref(RVALUE)
#endif


#if ! defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#define AUTOBOOST_THREAD_MOVABLE(TYPE)

#define AUTOBOOST_THREAD_COPYABLE(TYPE)

#else

#if defined AUTOBOOST_THREAD_USES_MOVE

#define AUTOBOOST_THREAD_MOVABLE(TYPE) \
    ::autoboost::rv<TYPE>& move()  AUTOBOOST_NOEXCEPT \
    { \
      return *static_cast< ::autoboost::rv<TYPE>* >(this); \
    } \
    const ::autoboost::rv<TYPE>& move() const AUTOBOOST_NOEXCEPT \
    { \
      return *static_cast<const ::autoboost::rv<TYPE>* >(this); \
    } \
    operator ::autoboost::rv<TYPE>&() \
    { \
      return *static_cast< ::autoboost::rv<TYPE>* >(this); \
    } \
    operator const ::autoboost::rv<TYPE>&() const \
    { \
      return *static_cast<const ::autoboost::rv<TYPE>* >(this); \
    }\

#define AUTOBOOST_THREAD_COPYABLE(TYPE) \
  TYPE& operator=(TYPE &t)\
  {  this->operator=(static_cast<const ::autoboost::rv<TYPE> &>(const_cast<const TYPE &>(t))); return *this;}


#else

#define AUTOBOOST_THREAD_MOVABLE(TYPE) \
    operator ::autoboost::detail::thread_move_t<TYPE>() AUTOBOOST_NOEXCEPT \
    { \
        return move(); \
    } \
    ::autoboost::detail::thread_move_t<TYPE> move() AUTOBOOST_NOEXCEPT \
    { \
      ::autoboost::detail::thread_move_t<TYPE> x(*this); \
        return x; \
    } \

#define AUTOBOOST_THREAD_COPYABLE(TYPE)

#endif
#endif

#define AUTOBOOST_THREAD_MOVABLE_ONLY(TYPE) \
  AUTOBOOST_THREAD_NO_COPYABLE(TYPE) \
  AUTOBOOST_THREAD_MOVABLE(TYPE) \
  typedef int autoboost_move_no_copy_constructor_or_assign; \


#define AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(TYPE) \
    AUTOBOOST_THREAD_COPYABLE(TYPE) \
    AUTOBOOST_THREAD_MOVABLE(TYPE) \



namespace autoboost
{
  namespace thread_detail
  {

#if ! defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#elif defined AUTOBOOST_THREAD_USES_MOVE
    template <class T>
    struct is_rv
       : ::autoboost::move_detail::is_rv<T>
    {};

#else
    template <class T>
    struct is_rv
       : ::autoboost::integral_constant<bool, false>
    {};

    template <class T>
    struct is_rv< ::autoboost::detail::thread_move_t<T> >
       : ::autoboost::integral_constant<bool, true>
    {};

    template <class T>
    struct is_rv< const ::autoboost::detail::thread_move_t<T> >
       : ::autoboost::integral_constant<bool, true>
    {};
#endif

#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
    template <class Tp>
    struct remove_reference : autoboost::remove_reference<Tp> {};
    template <class Tp>
    struct  decay : autoboost::decay<Tp> {};
#else
  template <class Tp>
  struct remove_reference
  {
    typedef Tp type;
  };
  template <class Tp>
  struct remove_reference<Tp&>
  {
    typedef Tp type;
  };
  template <class Tp>
  struct remove_reference< rv<Tp> > {
    typedef Tp type;
  };

  template <class Tp>
  struct  decay
  {
  private:
    typedef typename autoboost::move_detail::remove_rvalue_reference<Tp>::type Up0;
    typedef typename autoboost::remove_reference<Up0>::type Up;
  public:
      typedef typename conditional
                       <
                           is_array<Up>::value,
                           typename remove_extent<Up>::type*,
                           typename conditional
                           <
                                is_function<Up>::value,
                                typename add_pointer<Up>::type,
                                typename remove_cv<Up>::type
                           >::type
                       >::type type;
  };
#endif

#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
      template <class T>
      typename decay<T>::type
      decay_copy(T&& t)
      {
          return autoboost::forward<T>(t);
      }
#else
  template <class T>
  typename decay<T>::type
  decay_copy(AUTOBOOST_THREAD_FWD_REF(T) t)
  {
      return autoboost::forward<T>(t);
  }
#endif
  }
}

#include <autoboost/config/abi_suffix.hpp>

#endif

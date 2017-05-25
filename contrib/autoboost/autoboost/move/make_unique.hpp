//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2006-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_MOVE_MAKE_UNIQUE_HPP_INCLUDED
#define AUTOBOOST_MOVE_MAKE_UNIQUE_HPP_INCLUDED

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/move/detail/config_begin.hpp>
#include <autoboost/move/detail/workaround.hpp>
#include <autoboost/move/utility_core.hpp>
#include <autoboost/move/unique_ptr.hpp>
#include <cstddef>   //for std::size_t
#include <autoboost/move/detail/unique_ptr_meta_utils.hpp>
#ifdef AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
#  include <autoboost/move/detail/fwd_macros.hpp>
#endif

//!\file
//! Defines "make_unique" functions, which are factories to create instances
//! of unique_ptr depending on the passed arguments.
//!
//! This header can be a bit heavyweight in C++03 compilers due to the use of the
//! preprocessor library, that's why it's a a separate header from <tt>unique_ptr.hpp</tt>

#if !defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

namespace std {   //no namespace versioning in clang+libc++

struct nothrow_t;

}  //namespace std {

namespace autoboost{
namespace move_upmu {

//Compile time switch between
//single element, unknown bound array
//and known bound array
template<class T>
struct unique_ptr_if
{
   typedef ::autoboost::movelib::unique_ptr<T> t_is_not_array;
};

template<class T>
struct unique_ptr_if<T[]>
{
   typedef ::autoboost::movelib::unique_ptr<T[]> t_is_array_of_unknown_bound;
};

template<class T, std::size_t N>
struct unique_ptr_if<T[N]>
{
   typedef void t_is_array_of_known_bound;
};

template <int Dummy = 0>
struct nothrow_holder
{
   static std::nothrow_t *pnothrow;
};

template <int Dummy>
std::nothrow_t *nothrow_holder<Dummy>::pnothrow =
   reinterpret_cast<std::nothrow_t *>(0x1234);  //Avoid reference to null errors in sanitizers

}  //namespace move_upmu {
}  //namespace autoboost{

#endif   //!defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED)

namespace autoboost{
namespace movelib {

#if defined(AUTOBOOST_MOVE_DOXYGEN_INVOKED) || !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is not an array.
//!
//! <b>Returns</b>: <tt>unique_ptr<T>(new T(std::forward<Args>(args)...))</tt>.
template<class T, class... Args>
inline AUTOBOOST_MOVE_DOC1ST(unique_ptr<T>,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_not_array)
      make_unique(AUTOBOOST_FWD_REF(Args)... args)
{  return unique_ptr<T>(new T(::autoboost::forward<Args>(args)...));  }

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is not an array.
//!
//! <b>Returns</b>: <tt>unique_ptr<T>(new T(std::nothrow)(std::forward<Args>(args)...))</tt>.
template<class T, class... Args>
inline AUTOBOOST_MOVE_DOC1ST(unique_ptr<T>,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_not_array)
      make_unique_nothrow(AUTOBOOST_FWD_REF(Args)... args)
{  return unique_ptr<T>(new (*autoboost::move_upmu::nothrow_holder<>::pnothrow)T(::autoboost::forward<Args>(args)...));  }

#else
   #define AUTOBOOST_MOVE_MAKE_UNIQUE_CODE(N)\
      template<class T AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N>\
      typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_not_array\
         make_unique( AUTOBOOST_MOVE_UREF##N)\
      {  return unique_ptr<T>( new T( AUTOBOOST_MOVE_FWD##N ) );  }\
      \
      template<class T AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N>\
      typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_not_array\
         make_unique_nothrow( AUTOBOOST_MOVE_UREF##N)\
      {  return unique_ptr<T>( new (*autoboost::move_upmu::nothrow_holder<>::pnothrow)T ( AUTOBOOST_MOVE_FWD##N ) );  }\
      //
   AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_MOVE_MAKE_UNIQUE_CODE)
   #undef AUTOBOOST_MOVE_MAKE_UNIQUE_CODE

#endif

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is not an array.
//!
//! <b>Returns</b>: <tt>unique_ptr<T>(new T)</tt> (default initialization)
template<class T>
inline AUTOBOOST_MOVE_DOC1ST(unique_ptr<T>,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_not_array)
      make_unique_definit()
{
    return unique_ptr<T>(new T);
}

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is not an array.
//!
//! <b>Returns</b>: <tt>unique_ptr<T>(new T(std::nothrow)</tt> (default initialization)
template<class T>
inline AUTOBOOST_MOVE_DOC1ST(unique_ptr<T>,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_not_array)
      make_unique_nothrow_definit()
{
    return unique_ptr<T>(new (*autoboost::move_upmu::nothrow_holder<>::pnothrow)T);
}

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is an array of
//!   unknown bound.
//!
//! <b>Returns</b>: <tt>unique_ptr<T>(new remove_extent_t<T>[n]())</tt> (value initialization)
template<class T>
inline AUTOBOOST_MOVE_DOC1ST(unique_ptr<T>,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_array_of_unknown_bound)
      make_unique(std::size_t n)
{
    typedef typename ::autoboost::move_upmu::remove_extent<T>::type U;
    return unique_ptr<T>(new U[n]());
}

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is an array of
//!   unknown bound.
//!
//! <b>Returns</b>: <tt>unique_ptr<T>(new (std::nothrow)remove_extent_t<T>[n]())</tt> (value initialization)
template<class T>
inline AUTOBOOST_MOVE_DOC1ST(unique_ptr<T>,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_array_of_unknown_bound)
      make_unique_nothrow(std::size_t n)
{
    typedef typename ::autoboost::move_upmu::remove_extent<T>::type U;
    return unique_ptr<T>(new (*autoboost::move_upmu::nothrow_holder<>::pnothrow)U[n]());
}

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is an array of
//!   unknown bound.
//!
//! <b>Returns</b>: <tt>unique_ptr<T>(new remove_extent_t<T>[n])</tt> (default initialization)
template<class T>
inline AUTOBOOST_MOVE_DOC1ST(unique_ptr<T>,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_array_of_unknown_bound)
      make_unique_definit(std::size_t n)
{
    typedef typename ::autoboost::move_upmu::remove_extent<T>::type U;
    return unique_ptr<T>(new U[n]);
}

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is an array of
//!   unknown bound.
//!
//! <b>Returns</b>: <tt>unique_ptr<T>(new (std::nothrow)remove_extent_t<T>[n])</tt> (default initialization)
template<class T>
inline AUTOBOOST_MOVE_DOC1ST(unique_ptr<T>,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_array_of_unknown_bound)
      make_unique_nothrow_definit(std::size_t n)
{
    typedef typename ::autoboost::move_upmu::remove_extent<T>::type U;
    return unique_ptr<T>(new (*autoboost::move_upmu::nothrow_holder<>::pnothrow) U[n]);
}

#if !defined(AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS)

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is
//!   an array of known bound.
template<class T, class... Args>
inline AUTOBOOST_MOVE_DOC1ST(unspecified,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_array_of_known_bound)
      make_unique(AUTOBOOST_FWD_REF(Args) ...) = delete;

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is
//!   an array of known bound.
template<class T, class... Args>
inline AUTOBOOST_MOVE_DOC1ST(unspecified,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_array_of_known_bound)
      make_unique_definit(AUTOBOOST_FWD_REF(Args) ...) = delete;

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is
//!   an array of known bound.
template<class T, class... Args>
inline AUTOBOOST_MOVE_DOC1ST(unspecified,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_array_of_known_bound)
      make_unique_nothrow(AUTOBOOST_FWD_REF(Args) ...) = delete;

//! <b>Remarks</b>: This function shall not participate in overload resolution unless T is
//!   an array of known bound.
template<class T, class... Args>
inline AUTOBOOST_MOVE_DOC1ST(unspecified,
   typename ::autoboost::move_upmu::unique_ptr_if<T>::t_is_array_of_known_bound)
      make_unique_nothrow_definit(AUTOBOOST_FWD_REF(Args) ...) = delete;

#endif

}  //namespace movelib {

}  //namespace autoboost{

#include <autoboost/move/detail/config_end.hpp>

#endif   //#ifndef AUTOBOOST_MOVE_MAKE_UNIQUE_HPP_INCLUDED

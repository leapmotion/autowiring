// (C) Copyright 2002-2008, Fernando Luis Cacciola Carballal.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// 21 Ago 2002 (Created) Fernando Cacciola
// 24 Dec 2007 (Refactored and worked around various compiler bugs) Fernando Cacciola, Niels Dekker
// 23 May 2008 (Fixed operator= const issue, added initialized_value) Niels Dekker, Fernando Cacciola
// 21 Ago 2008 (Added swap) Niels Dekker, Fernando Cacciola
// 20 Feb 2009 (Fixed logical const-ness issues) Niels Dekker, Fernando Cacciola
// 03 Apr 2010 (Added initialized<T>, suggested by Jeffrey Hellrung, fixing #3472) Niels Dekker
// 30 May 2010 (Made memset call conditional, fixing #3869) Niels Dekker
//
#ifndef AUTOBOOST_UTILITY_VALUE_INIT_21AGO2002_HPP
#define AUTOBOOST_UTILITY_VALUE_INIT_21AGO2002_HPP

// Note: The implementation of autoboost::value_initialized had to deal with the
// fact that various compilers haven't fully implemented value-initialization.
// The constructor of autoboost::value_initialized<T> works around these compiler
// issues, by clearing the bytes of T, before constructing the T object it
// contains. More details on these issues are at libs/utility/value_init.htm

#include <autoboost/aligned_storage.hpp>
#include <autoboost/config.hpp> // For AUTOBOOST_NO_COMPLETE_VALUE_INITIALIZATION.
#include <autoboost/detail/workaround.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/type_traits/cv_traits.hpp>
#include <autoboost/type_traits/alignment_of.hpp>
#include <autoboost/swap.hpp>
#include <cstring>
#include <new>

#ifdef AUTOBOOST_MSVC
#pragma warning(push)
// It is safe to ignore the following warning from MSVC 7.1 or higher:
// "warning C4351: new behavior: elements of array will be default initialized"
#pragma warning(disable: 4351)
// It is safe to ignore the following MSVC warning, which may pop up when T is
// a const type: "warning C4512: assignment operator could not be generated".
#pragma warning(disable: 4512)
#endif

#ifdef AUTOBOOST_NO_COMPLETE_VALUE_INITIALIZATION
  // Implementation detail: The macro AUTOBOOST_DETAIL_VALUE_INIT_WORKAROUND_SUGGESTED
  // suggests that a workaround should be applied, because of compiler issues
  // regarding value-initialization.
  #define AUTOBOOST_DETAIL_VALUE_INIT_WORKAROUND_SUGGESTED
#endif

// Implementation detail: The macro AUTOBOOST_DETAIL_VALUE_INIT_WORKAROUND
// switches the value-initialization workaround either on or off.
#ifndef AUTOBOOST_DETAIL_VALUE_INIT_WORKAROUND
  #ifdef AUTOBOOST_DETAIL_VALUE_INIT_WORKAROUND_SUGGESTED
  #define AUTOBOOST_DETAIL_VALUE_INIT_WORKAROUND 1
  #else
  #define AUTOBOOST_DETAIL_VALUE_INIT_WORKAROUND 0
  #endif
#endif

namespace autoboost {

template<class T>
class initialized
{
  private :
    struct wrapper
    {
#if !AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x592))
      typename
#endif
      remove_const<T>::type data;

      AUTOBOOST_GPU_ENABLED
      wrapper()
      :
      data()
      {
      }

      AUTOBOOST_GPU_ENABLED
      wrapper(T const & arg)
      :
      data(arg)
      {
      }
    };

    mutable
#if !AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x592))
      typename
#endif
      aligned_storage<sizeof(wrapper), alignment_of<wrapper>::value>::type x;

    AUTOBOOST_GPU_ENABLED
    wrapper * wrapper_address() const
    {
      return static_cast<wrapper *>( static_cast<void*>(&x));
    }

  public :

    AUTOBOOST_GPU_ENABLED
    initialized()
    {
#if AUTOBOOST_DETAIL_VALUE_INIT_WORKAROUND
      std::memset(&x, 0, sizeof(x));
#endif
      new (wrapper_address()) wrapper();
    }

    AUTOBOOST_GPU_ENABLED
    initialized(initialized const & arg)
    {
      new (wrapper_address()) wrapper( static_cast<wrapper const &>(*(arg.wrapper_address())));
    }

    AUTOBOOST_GPU_ENABLED
    explicit initialized(T const & arg)
    {
      new (wrapper_address()) wrapper(arg);
    }

    AUTOBOOST_GPU_ENABLED
    initialized & operator=(initialized const & arg)
    {
      // Assignment is only allowed when T is non-const.
      AUTOBOOST_STATIC_ASSERT( ! is_const<T>::value );
      *wrapper_address() = static_cast<wrapper const &>(*(arg.wrapper_address()));
      return *this;
    }

    AUTOBOOST_GPU_ENABLED
    ~initialized()
    {
      wrapper_address()->wrapper::~wrapper();
    }

    AUTOBOOST_GPU_ENABLED
    T const & data() const
    {
      return wrapper_address()->data;
    }

    AUTOBOOST_GPU_ENABLED
    T& data()
    {
      return wrapper_address()->data;
    }

    AUTOBOOST_GPU_ENABLED
    void swap(initialized & arg)
    {
      ::autoboost::swap( this->data(), arg.data() );
    }

    AUTOBOOST_GPU_ENABLED
    operator T const &() const
    {
      return wrapper_address()->data;
    }

    AUTOBOOST_GPU_ENABLED
    operator T&()
    {
      return wrapper_address()->data;
    }

} ;

template<class T>
AUTOBOOST_GPU_ENABLED
T const& get ( initialized<T> const& x )
{
  return x.data() ;
}

template<class T>
AUTOBOOST_GPU_ENABLED
T& get ( initialized<T>& x )
{
  return x.data() ;
}

template<class T>
AUTOBOOST_GPU_ENABLED
void swap ( initialized<T> & lhs, initialized<T> & rhs )
{
  lhs.swap(rhs) ;
}

template<class T>
class value_initialized
{
  private :

    // initialized<T> does value-initialization by default.
    initialized<T> m_data;

  public :

    AUTOBOOST_GPU_ENABLED
    value_initialized()
    :
    m_data()
    { }

    AUTOBOOST_GPU_ENABLED
    T const & data() const
    {
      return m_data.data();
    }

    AUTOBOOST_GPU_ENABLED
    T& data()
    {
      return m_data.data();
    }

    AUTOBOOST_GPU_ENABLED
    void swap(value_initialized & arg)
    {
      m_data.swap(arg.m_data);
    }

    AUTOBOOST_GPU_ENABLED
    operator T const &() const
    {
      return m_data;
    }

    AUTOBOOST_GPU_ENABLED
    operator T&()
    {
      return m_data;
    }
} ;


template<class T>
AUTOBOOST_GPU_ENABLED
T const& get ( value_initialized<T> const& x )
{
  return x.data() ;
}

template<class T>
AUTOBOOST_GPU_ENABLED
T& get ( value_initialized<T>& x )
{
  return x.data() ;
}

template<class T>
AUTOBOOST_GPU_ENABLED
void swap ( value_initialized<T> & lhs, value_initialized<T> & rhs )
{
  lhs.swap(rhs) ;
}


class initialized_value_t
{
  public :

    template <class T> AUTOBOOST_GPU_ENABLED operator T() const
    {
      return initialized<T>().data();
    }
};

initialized_value_t const initialized_value = {} ;


} // namespace autoboost

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

#endif

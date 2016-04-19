// Copyright Peter Dimov and David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef GET_POINTER_DWA20021219_HPP
#define GET_POINTER_DWA20021219_HPP

#include <autoboost/config.hpp>

// In order to avoid circular dependencies with Boost.TR1
// we make sure that our include of <memory> doesn't try to
// pull in the TR1 headers: that's why we use this header
// rather than including <memory> directly:
#include <autoboost/config/no_tr1/memory.hpp>  // std::auto_ptr

namespace autoboost {

// get_pointer(p) extracts a ->* capable pointer from p

template<class T> T * get_pointer(T * p)
{
    return p;
}

// get_pointer(shared_ptr<T> const & p) has been moved to shared_ptr.hpp

#if !defined( AUTOBOOST_NO_AUTO_PTR )

#if defined( __GNUC__ ) && (defined( __GXX_EXPERIMENTAL_CXX0X__ ) || (__cplusplus >= 201103L))
#if defined( AUTOBOOST_GCC )
#if AUTOBOOST_GCC >= 40600
#define AUTOBOOST_CORE_DETAIL_DISABLE_LIBSTDCXX_DEPRECATED_WARNINGS
#endif // AUTOBOOST_GCC >= 40600
#elif defined( __clang__ ) && defined( __has_warning )
#if __has_warning("-Wdeprecated-declarations")
#define AUTOBOOST_CORE_DETAIL_DISABLE_LIBSTDCXX_DEPRECATED_WARNINGS
#endif // __has_warning("-Wdeprecated-declarations")
#endif
#endif // defined( __GNUC__ ) && (defined( __GXX_EXPERIMENTAL_CXX0X__ ) || (__cplusplus >= 201103L))

#if defined( AUTOBOOST_CORE_DETAIL_DISABLE_LIBSTDCXX_DEPRECATED_WARNINGS )
// Disable libstdc++ warnings about std::auto_ptr being deprecated in C++11 mode
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define AUTOBOOST_CORE_DETAIL_DISABLED_DEPRECATED_WARNINGS
#endif

template<class T> T * get_pointer(std::auto_ptr<T> const& p)
{
    return p.get();
}

#if defined( AUTOBOOST_CORE_DETAIL_DISABLE_LIBSTDCXX_DEPRECATED_WARNINGS )
#pragma GCC diagnostic pop
#undef AUTOBOOST_CORE_DETAIL_DISABLE_LIBSTDCXX_DEPRECATED_WARNINGS
#endif

#endif // !defined( AUTOBOOST_NO_AUTO_PTR )

#if !defined( AUTOBOOST_NO_CXX11_SMART_PTR )

template<class T> T * get_pointer( std::unique_ptr<T> const& p )
{
    return p.get();
}

template<class T> T * get_pointer( std::shared_ptr<T> const& p )
{
    return p.get();
}

#endif

} // namespace autoboost

#endif // GET_POINTER_DWA20021219_HPP

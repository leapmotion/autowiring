
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "autoboost/coroutine/stack_traits.hpp"

extern "C" {
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
}

//#if _POSIX_C_SOURCE >= 200112L

#include <algorithm>
#include <cmath>

#include <autoboost/assert.hpp>
#include <autoboost/thread.hpp>

#if !defined (SIGSTKSZ)
# define SIGSTKSZ (8 * 1024)
# define UDEF_SIGSTKSZ
#endif

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_PREFIX
#endif

namespace autoboost {
namespace coroutines {

void pagesize_( std::size_t * size)
{
    // conform to POSIX.1-2001
    * size = ::sysconf( _SC_PAGESIZE);
}

void stacksize_limit_( rlimit * limit)
{
    // conforming to POSIX.1-2001
#if defined(AUTOBOOST_DISABLE_ASSERTS)
    ::getrlimit( RLIMIT_STACK, limit);
#else
    const int result = ::getrlimit( RLIMIT_STACK, limit);
    AUTOBOOST_ASSERT( 0 == result);
#endif
}

std::size_t pagesize()
{
    static std::size_t size = 0;
    static autoboost::once_flag flag;
    autoboost::call_once( flag, pagesize_, & size);
    return size;
}

rlimit stacksize_limit()
{
    static rlimit limit;
    static autoboost::once_flag flag;
    autoboost::call_once( flag, stacksize_limit_, & limit);
    return limit;
}

bool
stack_traits::is_unbounded() AUTOBOOST_NOEXCEPT
{ return RLIM_INFINITY == stacksize_limit().rlim_max; }

std::size_t
stack_traits::page_size() AUTOBOOST_NOEXCEPT
{ return pagesize(); }

std::size_t
stack_traits::default_size() AUTOBOOST_NOEXCEPT
{
    std::size_t size = 8 * minimum_size();
    if ( is_unbounded() ) return size;

    AUTOBOOST_ASSERT( maximum_size() >= minimum_size() );
    return maximum_size() == size
        ? size
        : (std::min)( size, maximum_size() );
}

std::size_t
stack_traits::minimum_size() AUTOBOOST_NOEXCEPT
{ return SIGSTKSZ; }

std::size_t
stack_traits::maximum_size() AUTOBOOST_NOEXCEPT
{
    AUTOBOOST_ASSERT( ! is_unbounded() );
    return static_cast< std::size_t >( stacksize_limit().rlim_max);
}

}}

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_SUFFIX
#endif

#ifdef UDEF_SIGSTKSZ
# undef SIGSTKSZ
#endif

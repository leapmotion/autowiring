//
//  autoboost/assert.hpp - AUTOBOOST_ASSERT(expr)
//                     AUTOBOOST_ASSERT_MSG(expr, msg)
//                     AUTOBOOST_VERIFY(expr)
//                     AUTOBOOST_VERIFY_MSG(expr, msg)
//
//  Copyright (c) 2001, 2002 Peter Dimov and Multi Media Ltd.
//  Copyright (c) 2007, 2014 Peter Dimov
//  Copyright (c) Beman Dawes 2011
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//  Note: There are no include guards. This is intentional.
//
//  See http://www.boost.org/libs/assert/assert.html for documentation.
//

//
// Stop inspect complaining about use of 'assert':
//
// autoboostinspect:naassert_macro
//

//
// AUTOBOOST_ASSERT, AUTOBOOST_ASSERT_MSG
//

#undef AUTOBOOST_ASSERT
#undef AUTOBOOST_ASSERT_MSG

#if defined(AUTOBOOST_DISABLE_ASSERTS) || ( defined(AUTOBOOST_ENABLE_ASSERT_DEBUG_HANDLER) && defined(NDEBUG) )

# define AUTOBOOST_ASSERT(expr) ((void)0)
# define AUTOBOOST_ASSERT_MSG(expr, msg) ((void)0)

#elif defined(AUTOBOOST_ENABLE_ASSERT_HANDLER) || ( defined(AUTOBOOST_ENABLE_ASSERT_DEBUG_HANDLER) && !defined(NDEBUG) )

#include <autoboost/config.hpp> // for AUTOBOOST_LIKELY
#include <autoboost/current_function.hpp>

namespace autoboost
{
    void assertion_failed(char const * expr, char const * function, char const * file, long line); // user defined
    void assertion_failed_msg(char const * expr, char const * msg, char const * function, char const * file, long line); // user defined
} // namespace autoboost

#define AUTOBOOST_ASSERT(expr) (AUTOBOOST_LIKELY(!!(expr))? ((void)0): ::autoboost::assertion_failed(#expr, AUTOBOOST_CURRENT_FUNCTION, __FILE__, __LINE__))
#define AUTOBOOST_ASSERT_MSG(expr, msg) (AUTOBOOST_LIKELY(!!(expr))? ((void)0): ::autoboost::assertion_failed_msg(#expr, msg, AUTOBOOST_CURRENT_FUNCTION, __FILE__, __LINE__))

#else

# include <assert.h> // .h to support old libraries w/o <cassert> - effect is the same

# define AUTOBOOST_ASSERT(expr) assert(expr)
# define AUTOBOOST_ASSERT_MSG(expr, msg) assert((expr)&&(msg))

#endif

//
// AUTOBOOST_VERIFY, AUTOBOOST_VERIFY_MSG
//

#undef AUTOBOOST_VERIFY
#undef AUTOBOOST_VERIFY_MSG

#if defined(AUTOBOOST_DISABLE_ASSERTS) || ( !defined(AUTOBOOST_ENABLE_ASSERT_HANDLER) && defined(NDEBUG) )

# define AUTOBOOST_VERIFY(expr) ((void)(expr))
# define AUTOBOOST_VERIFY_MSG(expr, msg) ((void)(expr))

#else

# define AUTOBOOST_VERIFY(expr) AUTOBOOST_ASSERT(expr)
# define AUTOBOOST_VERIFY_MSG(expr, msg) AUTOBOOST_ASSERT_MSG(expr,msg)

#endif

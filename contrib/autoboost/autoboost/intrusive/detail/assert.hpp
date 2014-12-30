/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2006-2013
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTRUSIVE_DETAIL_ASSERT_HPP
#define AUTOBOOST_INTRUSIVE_DETAIL_ASSERT_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#if !defined(AUTOBOOST_INTRUSIVE_INVARIANT_ASSERT)
   #include <autoboost/assert.hpp>
   #define AUTOBOOST_INTRUSIVE_INVARIANT_ASSERT AUTOBOOST_ASSERT
#elif defined(AUTOBOOST_INTRUSIVE_INVARIANT_ASSERT_INCLUDE)
   #include AUTOBOOST_INTRUSIVE_INVARIANT_ASSERT_INCLUDE
#endif

#if !defined(AUTOBOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT)
   #include <autoboost/assert.hpp>
   #define AUTOBOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT AUTOBOOST_ASSERT
#elif defined(AUTOBOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT_INCLUDE)
   #include AUTOBOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT_INCLUDE
#endif

#if !defined(AUTOBOOST_INTRUSIVE_SAFE_HOOK_DESTRUCTOR_ASSERT)
   #include <autoboost/assert.hpp>
   #define AUTOBOOST_INTRUSIVE_SAFE_HOOK_DESTRUCTOR_ASSERT AUTOBOOST_ASSERT
#elif defined(AUTOBOOST_INTRUSIVE_SAFE_HOOK_DESTRUCTOR_ASSERT_INCLUDE)
   #include AUTOBOOST_INTRUSIVE_SAFE_HOOK_DESTRUCTOR_ASSERT_INCLUDE
#endif

#endif //AUTOBOOST_INTRUSIVE_DETAIL_ASSERT_HPP

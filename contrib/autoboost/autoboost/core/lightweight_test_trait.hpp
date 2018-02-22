#ifndef AUTOBOOST_CORE_LIGHTWEIGHT_TEST_TRAIT_HPP
#define AUTOBOOST_CORE_LIGHTWEIGHT_TEST_TRAIT_HPP

// MS compatible compilers support #pragma once

#if defined(_MSC_VER)
# pragma once
#endif

// autoboost/core/lightweight_test_trait.hpp
//
// AUTOBOOST_TEST_TRAIT_TRUE, AUTOBOOST_TEST_TRAIT_FALSE
//
// Copyright 2014 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt

#include <autoboost/core/lightweight_test.hpp>
#include <autoboost/core/typeinfo.hpp>

namespace autoboost
{

namespace detail
{

template< class T > inline void test_trait_impl( char const * trait, void (*)( T ),
  bool expected, char const * file, int line, char const * function )
{
    if( T::value == expected )
    {
        report_errors_remind();
    }
    else
    {
        AUTOBOOST_LIGHTWEIGHT_TEST_OSTREAM
            << file << "(" << line << "): predicate '" << trait << "' ["
            << autoboost::core::demangled_name( AUTOBOOST_CORE_TYPEID(T) ) << "]"
            << " test failed in function '" << function
            << "' (should have been " << ( expected? "true": "false" ) << ")"
            << std::endl;

        ++test_errors();
    }
}

} // namespace detail

} // namespace autoboost

#define AUTOBOOST_TEST_TRAIT_TRUE(type) ( ::autoboost::detail::test_trait_impl(#type, (void(*)type)0, true, __FILE__, __LINE__, AUTOBOOST_CURRENT_FUNCTION) )
#define AUTOBOOST_TEST_TRAIT_FALSE(type) ( ::autoboost::detail::test_trait_impl(#type, (void(*)type)0, false, __FILE__, __LINE__, AUTOBOOST_CURRENT_FUNCTION) )

#endif // #ifndef AUTOBOOST_CORE_LIGHTWEIGHT_TEST_TRAIT_HPP

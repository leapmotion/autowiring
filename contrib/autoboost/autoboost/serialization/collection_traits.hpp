#ifndef AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HPP
#define AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// collection_traits.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// This header assigns a level implemenation trait to a collection type
// for all primitives.  It is needed so that archives which are meant to be
// portable don't write class information in the archive.  Since, not all
// compiles recognize the same set of primitive types, the possibility
// exists for archives to be non-portable if class information for primitive
// types is included.  This is addressed by the following macros.
#include <autoboost/config.hpp>
#include <autoboost/mpl/integral_c.hpp>
#include <autoboost/mpl/integral_c_tag.hpp>

#include <autoboost/cstdint.hpp>
#include <autoboost/integer_traits.hpp>
#include <climits> // ULONG_MAX
#include <autoboost/serialization/level.hpp>

#define AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(T, C)          \
template<>                                                          \
struct implementation_level< C < T > > {                            \
    typedef mpl::integral_c_tag tag;                                \
    typedef mpl::int_<object_serializable> type;                    \
    AUTOBOOST_STATIC_CONSTANT(int, value = object_serializable);        \
};                                                                  \
/**/

#if defined(AUTOBOOST_NO_CWCHAR) || defined(AUTOBOOST_NO_INTRINSIC_WCHAR_T)
    #define AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER_WCHAR(C)
#else
    #define AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER_WCHAR(C)   \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(wchar_t, C)        \
    /**/
#endif

#if defined(AUTOBOOST_HAS_LONG_LONG)
    #define AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER_INT64(C)    \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(autoboost::long_long_type, C)  \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(autoboost::ulong_long_type, C) \
    /**/
#else
    #define AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER_INT64(C)
#endif

#define AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS(C)                     \
    namespace autoboost { namespace serialization {                      \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(bool, C)            \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(char, C)            \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(signed char, C)     \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(unsigned char, C)   \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(signed int, C)      \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(unsigned int, C)    \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(signed long, C)     \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(unsigned long, C)   \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(float, C)           \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(double, C)          \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(unsigned short, C)  \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER(signed short, C)    \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER_INT64(C)            \
    AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS_HELPER_WCHAR(C)            \
    } }                                                              \
    /**/

#endif // AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS

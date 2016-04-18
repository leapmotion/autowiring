// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_SIZE_TYPE_HPP
#define AUTOBOOST_RANGE_SIZE_TYPE_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/range/config.hpp>
#include <autoboost/range/difference_type.hpp>
#include <autoboost/range/concepts.hpp>

#include <autoboost/utility/enable_if.hpp>
#include <autoboost/type_traits/make_unsigned.hpp>
#include <autoboost/type_traits/remove_const.hpp>
#include <cstddef>
#include <utility>

namespace autoboost
{
    namespace detail
    {

        //////////////////////////////////////////////////////////////////////////
        // default
        //////////////////////////////////////////////////////////////////////////

        template<typename T>
        class has_size_type
        {
            typedef char no_type;
            struct yes_type { char dummy[2]; };

            template<typename C>
            static yes_type test(AUTOBOOST_DEDUCED_TYPENAME C::size_type x);

            template<typename C>
            static no_type test(...);

        public:
            static const bool value = sizeof(test<T>(0)) == sizeof(yes_type);
        };

        template<typename C, typename Enabler=void>
        struct range_size
        {
            typedef AUTOBOOST_DEDUCED_TYPENAME make_unsigned<
                AUTOBOOST_DEDUCED_TYPENAME range_difference<C>::type
            >::type type;
        };

        template<typename C>
        struct range_size<
            C,
            AUTOBOOST_DEDUCED_TYPENAME ::autoboost::enable_if<has_size_type<C>, void>::type
        >
        {
            typedef AUTOBOOST_DEDUCED_TYPENAME C::size_type type;
        };

    }

    template< class T >
    struct range_size :
        detail::range_size<T>
    {
// Very strange things happen on some compilers that have the range concept
// asserts disabled. This preprocessor condition is clearly redundant on a
// working compiler but is vital for at least some compilers such as clang 4.2
// but only on the Mac!
#if AUTOBOOST_RANGE_ENABLE_CONCEPT_ASSERT == 1
        AUTOBOOST_RANGE_CONCEPT_ASSERT((autoboost::SinglePassRangeConcept<T>));
#endif
    };

    template< class T >
    struct range_size<const T >
        : detail::range_size<T>
    {
#if AUTOBOOST_RANGE_ENABLE_CONCEPT_ASSERT == 1
        AUTOBOOST_RANGE_CONCEPT_ASSERT((autoboost::SinglePassRangeConcept<T>));
#endif
    };

} // namespace autoboost



#endif

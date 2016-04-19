//  autoboost polymorphic_pointer_cast.hpp header file  ----------------------------------------------//
//  (C) Copyright Boris Rasin and Antony Polukhin 2014-2015.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/conversion for Documentation.

#ifndef AUTOBOOST_CONVERSION_POLYMORPHIC_POINTER_CAST_HPP
#define AUTOBOOST_CONVERSION_POLYMORPHIC_POINTER_CAST_HPP

# include <autoboost/config.hpp>
# include <autoboost/assert.hpp>
# include <autoboost/pointer_cast.hpp>
# include <autoboost/throw_exception.hpp>
# include <autoboost/utility/declval.hpp>
# ifdef AUTOBOOST_NO_CXX11_DECLTYPE
#   include <autoboost/typeof/typeof.hpp>
# endif

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

namespace autoboost
{
//  See the documentation for descriptions of how to choose between
//  static_pointer_cast<>, dynamic_pointer_cast<>, polymorphic_pointer_cast<> and polymorphic_pointer_downcast<>

//  polymorphic_pointer_downcast  --------------------------------------------//

    //  AUTOBOOST_ASSERT() checked polymorphic downcast.  Crosscasts prohibited.
    //  Supports any type with static_pointer_cast/dynamic_pointer_cast functions:
    //  built-in pointers, std::shared_ptr, autoboost::shared_ptr, autoboost::intrusive_ptr, etc.

    //  WARNING: Because this cast uses AUTOBOOST_ASSERT(), it violates
    //  the One Definition Rule if used in multiple translation units
    //  where AUTOBOOST_DISABLE_ASSERTS, AUTOBOOST_ENABLE_ASSERT_HANDLER
    //  NDEBUG are defined inconsistently.

    //  Contributed by Boris Rasin

    namespace detail
    {
        template <typename Target, typename Source>
        struct dynamic_pointer_cast_result
        {
#ifdef AUTOBOOST_NO_CXX11_DECLTYPE
            AUTOBOOST_TYPEOF_NESTED_TYPEDEF_TPL(nested, dynamic_pointer_cast<Target>(autoboost::declval<Source>()))
            typedef typename nested::type type;
#else
            typedef decltype(dynamic_pointer_cast<Target>(autoboost::declval<Source>())) type;
#endif
        };
    }

    template <typename Target, typename Source>
    inline typename detail::dynamic_pointer_cast_result<Target, Source>::type
    polymorphic_pointer_downcast (const Source& x)
    {
        AUTOBOOST_ASSERT(dynamic_pointer_cast<Target> (x) == x);
        return static_pointer_cast<Target> (x);
    }

    template <typename Target, typename Source>
    inline typename detail::dynamic_pointer_cast_result<Target, Source>::type
    polymorphic_pointer_cast (const Source& x)
    {
        typename detail::dynamic_pointer_cast_result<Target, Source>::type tmp
            = dynamic_pointer_cast<Target> (x);
        if ( !tmp ) autoboost::throw_exception( std::bad_cast() );

        return tmp;
    }

} // namespace autoboost

#endif  // AUTOBOOST_CONVERSION_POLYMORPHIC_POINTER_CAST_HPP

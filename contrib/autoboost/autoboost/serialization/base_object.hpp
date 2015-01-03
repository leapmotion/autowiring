#ifndef AUTOBOOST_SERIALIZATION_BASE_OBJECT_HPP
#define AUTOBOOST_SERIALIZATION_BASE_OBJECT_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// base_object.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// if no archive headers have been included this is a no op
// this is to permit AUTOBOOST_EXPORT etc to be included in a 
// file declaration header

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

#include <autoboost/mpl/eval_if.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/identity.hpp>

#include <autoboost/type_traits/is_base_and_derived.hpp>
#include <autoboost/type_traits/is_pointer.hpp>
#include <autoboost/type_traits/is_const.hpp>
#include <autoboost/type_traits/is_polymorphic.hpp>

#include <autoboost/static_assert.hpp>
#include <autoboost/serialization/access.hpp>
#include <autoboost/serialization/force_include.hpp>
#include <autoboost/serialization/void_cast_fwd.hpp>

namespace autoboost {
namespace serialization {

namespace detail
{
    // get the base type for a given derived type
    // preserving the const-ness
    template<class B, class D>
    struct base_cast
    {
        typedef typename
        mpl::if_<
            is_const<D>,
            const B,
            B
        >::type type;
        AUTOBOOST_STATIC_ASSERT(is_const<type>::value == is_const<D>::value);
    };

    // only register void casts if the types are polymorphic
    template<class Base, class Derived>
    struct base_register
    {
        struct polymorphic {
            static void const * invoke(){
                Base const * const b = 0;
                Derived const * const d = 0;
                return & void_cast_register(d, b);
            }
        };
        struct non_polymorphic {
            static void const * invoke(){
                return 0;
            }
        };
        static void const * invoke(){
            typedef typename mpl::eval_if<
                is_polymorphic<Base>,
                mpl::identity<polymorphic>,
                mpl::identity<non_polymorphic>
            >::type type;
            return type::invoke();
        }
    };

} // namespace detail
#if defined(__BORLANDC__) && __BORLANDC__ < 0x610
template<class Base, class Derived>
const Base & 
base_object(const Derived & d)
{
    AUTOBOOST_STATIC_ASSERT(! is_pointer<Derived>::value);
    detail::base_register<Base, Derived>::invoke();
    return access::cast_reference<const Base, Derived>(d);
}
#else
template<class Base, class Derived>
typename detail::base_cast<Base, Derived>::type & 
base_object(Derived &d)
{
    AUTOBOOST_STATIC_ASSERT(( is_base_and_derived<Base,Derived>::value));
    AUTOBOOST_STATIC_ASSERT(! is_pointer<Derived>::value);
    typedef typename detail::base_cast<Base, Derived>::type type;
    detail::base_register<type, Derived>::invoke();
    return access::cast_reference<type, Derived>(d);
}
#endif

} // namespace serialization
} // namespace autoboost

#endif // AUTOBOOST_SERIALIZATION_BASE_OBJECT_HPP

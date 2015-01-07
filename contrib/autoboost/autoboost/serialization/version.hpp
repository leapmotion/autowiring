#ifndef AUTOBOOST_SERIALIZATION_VERSION_HPP
#define AUTOBOOST_SERIALIZATION_VERSION_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// version.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <autoboost/config.hpp>
#include <autoboost/mpl/assert.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/eval_if.hpp>
#include <autoboost/mpl/identity.hpp>
#include <autoboost/mpl/integral_c_tag.hpp>

#include <autoboost/type_traits/is_base_and_derived.hpp>

namespace autoboost { 
namespace serialization {

struct basic_traits;

// default version number is 0. Override with higher version
// when class definition changes.
template<class T>
struct version
{
    template<class U>
    struct traits_class_version {
        typedef typename U::version type;
    };

    typedef mpl::integral_c_tag tag;
    // note: at least one compiler complained w/o the full qualification
    // on basic traits below
    typedef
        typename mpl::eval_if<
            is_base_and_derived<autoboost::serialization::basic_traits,T>,
            traits_class_version< T >,
            mpl::int_<0>
        >::type type;
    AUTOBOOST_STATIC_CONSTANT(int, value = version::type::value);
};

#ifndef AUTOBOOST_NO_INCLASS_MEMBER_INITIALIZATION
template<class T>
const int version<T>::value;
#endif

} // namespace serialization
} // namespace autoboost

/* note: at first it seemed that this would be a good place to trap
 * as an error an attempt to set a version # for a class which doesn't
 * save its class information (including version #) in the archive.
 * However, this imposes a requirement that the version be set after
 * the implemention level which would be pretty confusing.  If this
 * is to be done, do this check in the input or output operators when
 * ALL the serialization traits are available.  Included the implementation
 * here with this comment as a reminder not to do this!
 */
//#include <autoboost/serialization/level.hpp>
//#include <autoboost/mpl/equal_to.hpp>

#include <autoboost/mpl/less.hpp>
#include <autoboost/mpl/comparison.hpp>

// specify the current version number for the class
// version numbers limited to 8 bits !!!
#define AUTOBOOST_CLASS_VERSION(T, N)                                      \
namespace autoboost {                                                      \
namespace serialization {                                              \
template<>                                                             \
struct version<T >                                                     \
{                                                                      \
    typedef mpl::int_<N> type;                                         \
    typedef mpl::integral_c_tag tag;                                   \
    AUTOBOOST_STATIC_CONSTANT(int, value = version::type::value);          \
    AUTOBOOST_MPL_ASSERT((                                                 \
        autoboost::mpl::less<                                              \
            autoboost::mpl::int_<N>,                                       \
            autoboost::mpl::int_<256>                                      \
        >                                                              \
    ));                                                                \
    /*                                                                 \
    AUTOBOOST_MPL_ASSERT((                                                 \
        mpl::equal_to<                                                 \
            :implementation_level<T >,                                 \
            mpl::int_<object_class_info>                               \
        >::value                                                       \
    ));                                                                \
    */                                                                 \
};                                                                     \
}                                                                      \
}

#endif // AUTOBOOST_SERIALIZATION_VERSION_HPP

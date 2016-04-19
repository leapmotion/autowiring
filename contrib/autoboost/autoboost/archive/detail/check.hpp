#ifndef AUTOBOOST_ARCHIVE_DETAIL_CHECK_HPP
#define AUTOBOOST_ARCHIVE_DETAIL_CHECK_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#pragma inline_depth(511)
#pragma inline_recursion(on)
#endif

#if defined(__MWERKS__)
#pragma inline_depth(511)
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// check.hpp: interface for serialization system.

// (C) Copyright 2009 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <autoboost/config.hpp>

#include <autoboost/static_assert.hpp>
#include <autoboost/type_traits/is_const.hpp>

#include <autoboost/mpl/eval_if.hpp>
#include <autoboost/mpl/or.hpp>
#include <autoboost/mpl/equal_to.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/not.hpp>
#include <autoboost/mpl/greater.hpp>
#include <autoboost/mpl/assert.hpp>

#include <autoboost/serialization/static_warning.hpp>
#include <autoboost/serialization/version.hpp>
#include <autoboost/serialization/level.hpp>
#include <autoboost/serialization/tracking.hpp>
#include <autoboost/serialization/wrapper.hpp>

namespace autoboost {
namespace archive {
namespace detail {

// checks for objects

template<class T>
inline void check_object_level(){
    typedef
        typename mpl::greater_equal<
            serialization::implementation_level< T >,
            mpl::int_<serialization::primitive_type>
        >::type typex;

    // trap attempts to serialize objects marked
    // not_serializable
    AUTOBOOST_STATIC_ASSERT(typex::value);
}

template<class T>
inline void check_object_versioning(){
    typedef
        typename mpl::or_<
            typename mpl::greater<
                serialization::implementation_level< T >,
                mpl::int_<serialization::object_serializable>
            >,
            typename mpl::equal_to<
                serialization::version< T >,
                mpl::int_<0>
            >
        > typex;
    // trap attempts to serialize with objects that don't
    // save class information in the archive with versioning.
    AUTOBOOST_STATIC_ASSERT(typex::value);
}

template<class T>
inline void check_object_tracking(){
    // presume it has already been determined that
    // T is not a const
    AUTOBOOST_STATIC_ASSERT(! autoboost::is_const< T >::value);
    typedef typename mpl::equal_to<
        serialization::tracking_level< T >,
        mpl::int_<serialization::track_never>
    >::type typex;
    // saving an non-const object of a type not marked "track_never)

    // may be an indicator of an error usage of the
    // serialization library and should be double checked.
    // See documentation on object tracking.  Also, see the
    // "rationale" section of the documenation
    // for motivation for this checking.

    AUTOBOOST_STATIC_WARNING(typex::value);
}

// checks for pointers

template<class T>
inline void check_pointer_level(){
    // we should only invoke this once we KNOW that T
    // has been used as a pointer!!
    typedef
        typename mpl::or_<
            typename mpl::greater<
                serialization::implementation_level< T >,
                mpl::int_<serialization::object_serializable>
            >,
            typename mpl::not_<
                typename mpl::equal_to<
                    serialization::tracking_level< T >,
                    mpl::int_<serialization::track_selectively>
                >
            >
        > typex;
    // Address the following when serializing to a pointer:

    // a) This type doesn't save class information in the
    // archive. That is, the serialization trait implementation
    // level <= object_serializable.
    // b) Tracking for this type is set to "track selectively"

    // in this case, indication that an object is tracked is
    // not stored in the archive itself - see level == object_serializable
    // but rather the existence of the operation ar >> T * is used to
    // infer that an object of this type should be tracked.  So, if
    // you save via a pointer but don't load via a pointer the operation
    // will fail on load without given any valid reason for the failure.

    // So if your program traps here, consider changing the
    // tracking or implementation level traits - or not
    // serializing via a pointer.
    AUTOBOOST_STATIC_WARNING(typex::value);
}

template<class T>
void inline check_pointer_tracking(){
    typedef typename mpl::greater<
        serialization::tracking_level< T >,
        mpl::int_<serialization::track_never>
    >::type typex;
    // serializing an object of a type marked "track_never" through a pointer
    // could result in creating more objects than were saved!
    AUTOBOOST_STATIC_WARNING(typex::value);
}

template<class T>
inline void check_const_loading(){
    typedef
        typename mpl::or_<
            typename autoboost::serialization::is_wrapper< T >,
            typename mpl::not_<
                typename autoboost::is_const< T >
            >
        >::type typex;
    // cannot load data into a "const" object unless it's a
    // wrapper around some other non-const object.
    AUTOBOOST_STATIC_ASSERT(typex::value);
}

} // detail
} // archive
} // autoboost

#endif // AUTOBOOST_ARCHIVE_DETAIL_CHECK_HPP

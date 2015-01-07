#ifndef AUTOBOOST_SERIALIZATION_LEVEL_HPP
#define AUTOBOOST_SERIALIZATION_LEVEL_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// level.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

#include <autoboost/type_traits/is_fundamental.hpp>
#include <autoboost/type_traits/is_enum.hpp>
#include <autoboost/type_traits/is_array.hpp>
#include <autoboost/type_traits/is_class.hpp>
#include <autoboost/type_traits/is_base_and_derived.hpp>

#include <autoboost/mpl/eval_if.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/integral_c.hpp>
#include <autoboost/mpl/integral_c_tag.hpp>

#include <autoboost/serialization/level_enum.hpp>

namespace autoboost {
namespace serialization {

struct basic_traits;

// default serialization implementation level
template<class T>
struct implementation_level_impl {
    template<class U>
    struct traits_class_level {
        typedef typename U::level type;
    };

    typedef mpl::integral_c_tag tag;
    // note: at least one compiler complained w/o the full qualification
    // on basic traits below
    typedef
        typename mpl::eval_if<
            is_base_and_derived<autoboost::serialization::basic_traits, T>,
            traits_class_level< T >,
        //else
        typename mpl::eval_if<
            is_fundamental< T >,
            mpl::int_<primitive_type>,
        //else
        typename mpl::eval_if<
            is_class< T >,
            mpl::int_<object_class_info>,
        //else
        typename mpl::eval_if<
            is_array< T >,
            #if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x560))
                mpl::int_<not_serializable>,
            #else
                mpl::int_<object_serializable>,
            #endif
        //else
        typename mpl::eval_if<
            is_enum< T >,
            //#if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x560))
            //    mpl::int_<not_serializable>,
            //#else
                mpl::int_<primitive_type>,
            //#endif
        //else
            mpl::int_<not_serializable>
        >
        >
        >
        >
        >::type type;
        // vc 7.1 doesn't like enums here
    AUTOBOOST_STATIC_CONSTANT(int, value = type::value);
};

template<class T>
struct implementation_level : 
    public implementation_level_impl<const T>
{
};

template<class T, int L>
inline bool operator>=(implementation_level< T > t, enum level_type l)
{
    return t.value >= (int)l;
}

} // namespace serialization
} // namespace autoboost

// specify the level of serialization implementation for the class
// require that class info saved when versioning is used
#define AUTOBOOST_CLASS_IMPLEMENTATION(T, E)                 \
    namespace autoboost {                                    \
    namespace serialization {                            \
    template <>                                          \
    struct implementation_level_impl< const T >                     \
    {                                                    \
        typedef mpl::integral_c_tag tag;                 \
        typedef mpl::int_< E > type;                     \
        AUTOBOOST_STATIC_CONSTANT(                           \
            int,                                         \
            value = implementation_level_impl::type::value    \
        );                                               \
    };                                                   \
    }                                                    \
    }
    /**/

#endif // AUTOBOOST_SERIALIZATION_LEVEL_HPP

// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_EXT_PUSH_FRONT_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_EXT_PUSH_FRONT_HPP_INCLUDED

#include <autoboost/range/config.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/difference_type.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/detail/implementation_help.hpp>
#include <autoboost/assert.hpp>

namespace autoboost
{
    namespace range
    {

template< class Container, class Range >
inline Container& push_front( Container& on, const Range& from )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<Container> ));
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const Range> ));
    AUTOBOOST_ASSERT_MSG(!range_detail::is_same_object(on, from),
        "cannot copy from a container to itself");
    on.insert( on.begin(), autoboost::begin(from), autoboost::end(from) );
    return on;
}

    } // namespace range
    using range::push_front;
} // namespace autoboost

#endif // include guard

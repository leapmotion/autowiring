// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_EXT_INSERT_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_EXT_INSERT_HPP_INCLUDED

#include <autoboost/range/config.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/difference_type.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/assert.hpp>

namespace autoboost
{
    namespace range
    {

template< class Container, class Range >
inline Container& insert( Container& on,
                          AUTOBOOST_DEDUCED_TYPENAME Container::iterator before,
                          const Range& from )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<Container> ));
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<Range> ));
    on.insert( before, autoboost::begin(from), autoboost::end(from) );
    return on;
}

template< class Container, class Range >
inline Container& insert( Container& on, const Range& from )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<Container> ));
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<Range> ));
    on.insert(autoboost::begin(from), autoboost::end(from));
}

    } // namespace range
    using range::insert;
} // namespace autoboost

#endif // include guard

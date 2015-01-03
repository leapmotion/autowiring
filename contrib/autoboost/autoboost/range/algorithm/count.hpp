//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_COUNT_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_COUNT_HPP_INCLUDED

#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/difference_type.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function count
///
/// range-based version of the count std algorithm
///
/// \pre SinglePassRange is a model of the SinglePassRangeConcept
template< class SinglePassRange, class Value >
inline AUTOBOOST_DEDUCED_TYPENAME range_difference<SinglePassRange>::type
count(SinglePassRange& rng, const Value& val)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<SinglePassRange> ));
    return std::count(autoboost::begin(rng), autoboost::end(rng), val);
}

/// \overload
template< class SinglePassRange, class Value >
inline AUTOBOOST_DEDUCED_TYPENAME range_difference<SinglePassRange const>::type
count(const SinglePassRange& rng, const Value& val)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange> ));
    return std::count(autoboost::begin(rng), autoboost::end(rng), val);
}

    } // namespace range
    using range::count;
} // namespace autoboost

#endif // include guard

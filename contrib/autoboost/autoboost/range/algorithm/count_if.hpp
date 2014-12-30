//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_COUNT_IF_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_COUNT_IF_HPP_INCLUDED

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

/// \brief template function count_if
///
/// range-based version of the count_if std algorithm
///
/// \pre SinglePassRange is a model of the SinglePassRangeConcept
/// \pre UnaryPredicate is a model of the UnaryPredicateConcept
template< class SinglePassRange, class UnaryPredicate >
inline AUTOBOOST_DEDUCED_TYPENAME autoboost::range_difference<SinglePassRange>::type
count_if(SinglePassRange& rng, UnaryPredicate pred)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<SinglePassRange> ));
    return std::count_if(autoboost::begin(rng), autoboost::end(rng), pred);
}

/// \overload
template< class SinglePassRange, class UnaryPredicate >
inline AUTOBOOST_DEDUCED_TYPENAME autoboost::range_difference<const SinglePassRange>::type
count_if(const SinglePassRange& rng, UnaryPredicate pred)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange> ));
    return std::count_if(autoboost::begin(rng), autoboost::end(rng), pred);
}

    } // namespace range
    using range::count_if;
} // namespace autoboost

#endif // include guard

//  Copyright Bryce Lelbach 2010
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_EXT_IS_SORTED_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_EXT_IS_SORTED_HPP_INCLUDED

#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/value_type.hpp>
#include <autoboost/detail/is_sorted.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function is_sorted
///
/// range-based version of the is_sorted std algorithm
///
/// \pre SinglePassRange is a model of the SinglePassRangeConcept
template<class SinglePassRange>
inline bool is_sorted(const SinglePassRange& rng)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<const SinglePassRange>));
    AUTOBOOST_RANGE_CONCEPT_ASSERT((LessThanComparableConcept<AUTOBOOST_DEDUCED_TYPENAME
      range_value<const SinglePassRange>::type>));
    return ::autoboost::detail::is_sorted(autoboost::begin(rng), autoboost::end(rng));
}

/// \overload
template<class SinglePassRange, class BinaryPredicate>
inline bool is_sorted(const SinglePassRange& rng, BinaryPredicate pred)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<const SinglePassRange>));
    AUTOBOOST_RANGE_CONCEPT_ASSERT((BinaryPredicateConcept<BinaryPredicate,
      AUTOBOOST_DEDUCED_TYPENAME range_value<const SinglePassRange>::type,
      AUTOBOOST_DEDUCED_TYPENAME range_value<const SinglePassRange>::type>));
    return ::autoboost::detail::is_sorted(autoboost::begin(rng), autoboost::end(rng), pred);
}

    } // namespace range

using range::is_sorted;

} // namespace autoboost

#endif // include guard

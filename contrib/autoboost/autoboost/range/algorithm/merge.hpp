//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_MERGE_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_MERGE_HPP_INCLUDED

#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function merge
///
/// range-based version of the merge std algorithm
///
/// \pre SinglePassRange1 is a model of the SinglePassRangeConcept
/// \pre SinglePassRange2 is a model of the SinglePassRangeConcept
/// \pre BinaryPredicate is a model of the BinaryPredicateConcept
///
template<class SinglePassRange1, class SinglePassRange2,
         class OutputIterator>
inline OutputIterator merge(const SinglePassRange1& rng1,
                            const SinglePassRange2& rng2,
                            OutputIterator          out)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange1> ));
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange2> ));
    return std::merge(autoboost::begin(rng1), autoboost::end(rng1),
                      autoboost::begin(rng2), autoboost::end(rng2), out);
}

/// \overload
template<class SinglePassRange1, class SinglePassRange2,
         class OutputIterator, class BinaryPredicate>
inline OutputIterator merge(const SinglePassRange1& rng1,
                            const SinglePassRange2& rng2,
                            OutputIterator          out,
                            BinaryPredicate         pred)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange1> ));
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange2> ));
    return std::merge(autoboost::begin(rng1), autoboost::end(rng1),
                      autoboost::begin(rng2), autoboost::end(rng2), out, pred);
}

    } // namespace range
    using range::merge;
} // namespace autoboost

#endif // include guard

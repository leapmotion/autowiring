//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_REVERSE_COPY_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_REVERSE_COPY_HPP_INCLUDED

#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/iterator/iterator_concepts.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function reverse_copy
///
/// range-based version of the reverse_copy std algorithm
///
/// \pre BidirectionalRange is a model of the BidirectionalRangeConcept
template<class BidirectionalRange, class OutputIterator>
inline OutputIterator reverse_copy(const BidirectionalRange& rng, OutputIterator out)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( BidirectionalRangeConcept<const BidirectionalRange> ));
    return std::reverse_copy(autoboost::begin(rng), autoboost::end(rng), out);
}

    } // namespace range
    using range::reverse_copy;
} // namespace autoboost

#endif // include guard

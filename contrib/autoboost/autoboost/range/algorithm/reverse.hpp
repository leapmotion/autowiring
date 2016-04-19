//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_REVERSE_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_REVERSE_HPP_INCLUDED

#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/detail/range_return.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function reverse
///
/// range-based version of the reverse std algorithm
///
/// \pre BidirectionalRange is a model of the BidirectionalRangeConcept
template<class BidirectionalRange>
inline BidirectionalRange& reverse(BidirectionalRange& rng)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( BidirectionalRangeConcept<BidirectionalRange> ));
    std::reverse(autoboost::begin(rng), autoboost::end(rng));
    return rng;
}

/// \overload
template<class BidirectionalRange>
inline const BidirectionalRange& reverse(const BidirectionalRange& rng)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( BidirectionalRangeConcept<const BidirectionalRange> ));
    std::reverse(autoboost::begin(rng), autoboost::end(rng));
    return rng;
}

    } // namespace range
    using range::reverse;
} // namespace autoboost

#endif // include guard

//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_COPY_BACKWARD_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_COPY_BACKWARD_HPP_INCLUDED

#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function copy_backward
///
/// range-based version of the copy_backwards std algorithm
///
/// \pre BidirectionalRange is a model of the BidirectionalRangeConcept
/// \pre BidirectionalTraversalWriteableIterator is a model of the BidirectionalIteratorConcept
/// \pre BidirectionalTraversalWriteableIterator is a model of the WriteableIteratorConcept
template< class BidirectionalRange, class BidirectionalTraversalWriteableIterator >
inline BidirectionalTraversalWriteableIterator
copy_backward(const BidirectionalRange& rng,
              BidirectionalTraversalWriteableIterator out)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( BidirectionalRangeConcept<const BidirectionalRange> ));
    return std::copy_backward(autoboost::begin(rng), autoboost::end(rng), out);
}

    } // namespace range
    using range::copy_backward;
} // namespace autoboost

#endif // include guard

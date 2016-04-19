//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_COPY_N_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_COPY_N_HPP_INCLUDED

#include <autoboost/assert.hpp>
#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/distance.hpp>
#include <autoboost/range/iterator.hpp>
#include <autoboost/range/iterator_range.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function copy
///
/// range-based version of the copy std algorithm
///
/// \pre SinglePassRange is a model of the SinglePassRangeConcept
/// \pre OutputIterator is a model of the OutputIteratorConcept
/// \pre 0 <= n <= distance(rng)
template< class SinglePassRange, class Size, class OutputIterator >
inline OutputIterator copy_n(const SinglePassRange& rng, Size n, OutputIterator out)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange> ));
    AUTOBOOST_ASSERT( n <= static_cast<Size>(::autoboost::distance(rng)) );
    AUTOBOOST_ASSERT( n >= static_cast<Size>(0) );

    AUTOBOOST_DEDUCED_TYPENAME range_iterator<const SinglePassRange>::type source = ::autoboost::begin(rng);

    for (Size i = 0; i < n; ++i, ++out, ++source)
        *out = *source;

    return out;
}

    } // namespace range
    using ::autoboost::range::copy_n;
} // namespace autoboost

#endif // include guard

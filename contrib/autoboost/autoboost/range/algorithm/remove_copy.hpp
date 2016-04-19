//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_REMOVE_COPY_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_REMOVE_COPY_HPP_INCLUDED

#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function remove_copy
///
/// range-based version of the remove_copy std algorithm
///
/// \pre SinglePassRange is a model of the SinglePassRangeConcept
/// \pre OutputIterator is a model of the OutputIteratorConcept
/// \pre Value is a model of the EqualityComparableConcept
/// \pre Objects of type Value can be compared for equality with objects of
/// InputIterator's value type.
template< class SinglePassRange, class OutputIterator, class Value >
inline OutputIterator
remove_copy(const SinglePassRange& rng, OutputIterator out_it, const Value& val)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange> ));
    return std::remove_copy(autoboost::begin(rng), autoboost::end(rng), out_it, val);
}

    } // namespace range
    using range::remove_copy;
} // namespace autoboost

#endif // include guard

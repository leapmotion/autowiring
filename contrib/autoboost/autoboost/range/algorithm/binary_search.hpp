//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_BINARY_SEARCH_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_BINARY_SEARCH_HPP_INCLUDED

#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function binary_search
///
/// range-based version of the binary_search std algorithm
///
/// \pre ForwardRange is a model of the ForwardRangeConcept
/// \pre BinaryPredicate is a model of the BinaryPredicateConcept
template<class ForwardRange, class Value>
inline bool binary_search(const ForwardRange& rng, const Value& val)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return std::binary_search(autoboost::begin(rng), autoboost::end(rng), val);
}

/// \overload
template<class ForwardRange, class Value, class BinaryPredicate>
inline bool binary_search(const ForwardRange& rng, const Value& val,
                          BinaryPredicate pred)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return std::binary_search(autoboost::begin(rng), autoboost::end(rng), val, pred);
}

    } // namespace range
    using range::binary_search;
} // namespace autoboost

#endif // include guard

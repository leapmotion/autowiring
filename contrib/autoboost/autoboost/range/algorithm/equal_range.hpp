//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_EQUAL_RANGE_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_EQUAL_RANGE_HPP_INCLUDED

#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function equal_range
///
/// range-based version of the equal_range std algorithm
///
/// \pre ForwardRange is a model of the ForwardRangeConcept
/// \pre SortPredicate is a model of the BinaryPredicateConcept
template<class ForwardRange, class Value>
inline std::pair<
        AUTOBOOST_DEDUCED_TYPENAME autoboost::range_iterator<ForwardRange>::type,
        AUTOBOOST_DEDUCED_TYPENAME autoboost::range_iterator<ForwardRange>::type
       >
equal_range(ForwardRange& rng, const Value& val)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return std::equal_range(autoboost::begin(rng), autoboost::end(rng), val);
}

/// \overload
template<class ForwardRange, class Value>
inline std::pair<
        AUTOBOOST_DEDUCED_TYPENAME autoboost::range_iterator<const ForwardRange>::type,
        AUTOBOOST_DEDUCED_TYPENAME autoboost::range_iterator<const ForwardRange>::type
       >
equal_range(const ForwardRange& rng, const Value& val)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return std::equal_range(autoboost::begin(rng), autoboost::end(rng), val);
}

/// \overload
template<class ForwardRange, class Value, class SortPredicate>
inline std::pair<
        AUTOBOOST_DEDUCED_TYPENAME autoboost::range_iterator<ForwardRange>::type,
        AUTOBOOST_DEDUCED_TYPENAME autoboost::range_iterator<ForwardRange>::type
       >
equal_range(ForwardRange& rng, const Value& val, SortPredicate pred)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return std::equal_range(autoboost::begin(rng), autoboost::end(rng), val, pred);
}

/// \overload
template<class ForwardRange, class Value, class SortPredicate>
inline std::pair<
        AUTOBOOST_DEDUCED_TYPENAME autoboost::range_iterator<const ForwardRange>::type,
        AUTOBOOST_DEDUCED_TYPENAME autoboost::range_iterator<const ForwardRange>::type
       >
equal_range(const ForwardRange& rng, const Value& val, SortPredicate pred)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return std::equal_range(autoboost::begin(rng), autoboost::end(rng), val, pred);
}

    } // namespace range
    using range::equal_range;
} // namespace autoboost

#endif // include guard

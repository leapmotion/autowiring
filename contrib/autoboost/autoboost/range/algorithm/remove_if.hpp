//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_REMOVE_IF_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_REMOVE_IF_HPP_INCLUDED

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

/// \brief template function remove_if
///
/// range-based version of the remove_if std algorithm
///
/// \pre ForwardRange is a model of the ForwardRangeConcept
/// \pre UnaryPredicate is a model of the UnaryPredicateConcept
template< class ForwardRange, class UnaryPredicate >
inline AUTOBOOST_DEDUCED_TYPENAME autoboost::range_iterator<ForwardRange>::type
remove_if(ForwardRange& rng, UnaryPredicate pred)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return std::remove_if(autoboost::begin(rng), autoboost::end(rng), pred);
}

/// \overload
template< class ForwardRange, class UnaryPredicate >
inline AUTOBOOST_DEDUCED_TYPENAME autoboost::range_iterator<const ForwardRange>::type
remove_if(const ForwardRange& rng, UnaryPredicate pred)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return std::remove_if(autoboost::begin(rng), autoboost::end(rng), pred);
}

// range_return overloads

/// \overload
template< range_return_value re, class ForwardRange, class UnaryPredicate >
inline AUTOBOOST_DEDUCED_TYPENAME range_return<ForwardRange,re>::type
remove_if(ForwardRange& rng, UnaryPredicate pred)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return range_return<ForwardRange,re>::pack(
        std::remove_if(autoboost::begin(rng), autoboost::end(rng), pred),
        rng);
}

/// \overload
template< range_return_value re, class ForwardRange, class UnaryPredicate >
inline AUTOBOOST_DEDUCED_TYPENAME range_return<const ForwardRange,re>::type
remove_if(const ForwardRange& rng, UnaryPredicate pred)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return range_return<const ForwardRange,re>::pack(
        std::remove_if(autoboost::begin(rng), autoboost::end(rng), pred),
        rng);
}

    } // namespace range
    using range::remove_if;
} // namespace autoboost

#endif // include guard

//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_UPPER_BOUND_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_UPPER_BOUND_HPP_INCLUDED

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

/// \brief template function upper_bound
///
/// range-based version of the upper_bound std algorithm
///
/// \pre ForwardRange is a model of the ForwardRangeConcept
template< class ForwardRange, class Value >
inline
AUTOBOOST_DEDUCED_TYPENAME disable_if<
    is_const<ForwardRange>,
    AUTOBOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type
>::type
upper_bound( ForwardRange& rng, Value val )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return std::upper_bound(autoboost::begin(rng), autoboost::end(rng), val);
}

/// \overload
template< class ForwardRange, class Value >
AUTOBOOST_DEDUCED_TYPENAME range_iterator<const ForwardRange>::type
upper_bound( const ForwardRange& rng, Value val )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return std::upper_bound(autoboost::begin(rng), autoboost::end(rng), val);
}

/// \overload
template< class ForwardRange, class Value, class SortPredicate >
inline AUTOBOOST_DEDUCED_TYPENAME disable_if<
    is_const<ForwardRange>,
    AUTOBOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type
>::type
upper_bound( ForwardRange& rng, Value val, SortPredicate pred )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return std::upper_bound(autoboost::begin(rng), autoboost::end(rng), val, pred);
}

/// \overload
template< class ForwardRange, class Value, class SortPredicate >
inline AUTOBOOST_DEDUCED_TYPENAME range_iterator<const ForwardRange>::type
upper_bound( const ForwardRange& rng, Value val, SortPredicate pred )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return std::upper_bound(autoboost::begin(rng), autoboost::end(rng), val, pred);
}

/// \overload
template< range_return_value re, class ForwardRange, class Value >
inline AUTOBOOST_DEDUCED_TYPENAME disable_if<
    is_const<ForwardRange>,
    AUTOBOOST_DEDUCED_TYPENAME range_return<ForwardRange,re>::type
>::type
upper_bound( ForwardRange& rng, Value val )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return range_return<ForwardRange,re>::
        pack(std::upper_bound(autoboost::begin(rng), autoboost::end(rng), val),
             rng);
}

/// \overload
template< range_return_value re, class ForwardRange, class Value >
inline AUTOBOOST_DEDUCED_TYPENAME range_return<const ForwardRange,re>::type
upper_bound( const ForwardRange& rng, Value val )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return range_return<const ForwardRange,re>::
        pack(std::upper_bound(autoboost::begin(rng), autoboost::end(rng), val),
             rng);
}

/// \overload
template< range_return_value re, class ForwardRange, class Value,
          class SortPredicate >
inline AUTOBOOST_DEDUCED_TYPENAME disable_if<
    is_const<ForwardRange>,
    AUTOBOOST_DEDUCED_TYPENAME range_return<ForwardRange,re>::type
>::type
upper_bound( ForwardRange& rng, Value val, SortPredicate pred )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return range_return<ForwardRange,re>::
        pack(std::upper_bound(autoboost::begin(rng), autoboost::end(rng), val, pred),
             rng);
}

/// \overload
template< range_return_value re, class ForwardRange, class Value,
          class SortPredicate >
inline AUTOBOOST_DEDUCED_TYPENAME range_return<const ForwardRange,re>::type
upper_bound( const ForwardRange& rng, Value val, SortPredicate pred )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return range_return<const ForwardRange,re>::
        pack(std::upper_bound(autoboost::begin(rng), autoboost::end(rng), val, pred),
             rng);
}

    } // namespace range
    using range::upper_bound;
} // namespace autoboost

#endif // include guard

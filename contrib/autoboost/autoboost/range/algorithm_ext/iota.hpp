// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_EXT_IOTA_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_EXT_IOTA_HPP_INCLUDED

#include <autoboost/range/config.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/iterator.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>

namespace autoboost
{
    namespace range
    {

template< class ForwardRange, class Value >
inline ForwardRange& iota( ForwardRange& rng, Value x )
{
    AUTOBOOST_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    typedef AUTOBOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type iterator_t;

    iterator_t last_target = ::autoboost::end(rng);
    for (iterator_t target = ::autoboost::begin(rng); target != last_target; ++target, ++x)
        *target = x;

    return rng;
}

template< class ForwardRange, class Value >
inline const ForwardRange& iota( const ForwardRange& rng, Value x )
{
    AUTOBOOST_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    typedef AUTOBOOST_DEDUCED_TYPENAME range_iterator<const ForwardRange>::type iterator_t;

    iterator_t last_target = ::autoboost::end(rng);
    for (iterator_t target = ::autoboost::begin(rng); target != last_target; ++target, ++x)
        *target = x;

    return rng;
}

    } // namespace range
    using range::iota;
} // namespace autoboost

#endif // include guard

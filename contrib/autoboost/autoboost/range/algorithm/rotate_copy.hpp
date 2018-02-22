//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_ROTATE_COPY_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_ROTATE_COPY_HPP_INCLUDED

#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/iterator.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

    /// \brief template function rotate
    ///
    /// range-based version of the rotate std algorithm
    ///
    /// \pre Rng meets the requirements for a Forward range
    template<typename ForwardRange, typename OutputIterator>
    inline OutputIterator rotate_copy(
        const ForwardRange&                                             rng,
        AUTOBOOST_DEDUCED_TYPENAME range_iterator<const ForwardRange>::type middle,
        OutputIterator                                                  target
        )
    {
        AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
        return std::rotate_copy(autoboost::begin(rng), middle, autoboost::end(rng), target);
    }

    } // namespace range
    using range::rotate_copy;
} // namespace autoboost

#endif // include guard

//  Copyright Neil Groves 2010. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_COUNTING_RANGE_HPP_INCLUDED
#define AUTOBOOST_RANGE_COUNTING_RANGE_HPP_INCLUDED

#include <autoboost/config.hpp>
#if AUTOBOOST_MSVC >= 1400
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

#include <autoboost/range/iterator_range_core.hpp>
#include <autoboost/range/value_type.hpp>
#include <autoboost/range/iterator.hpp>
#include <autoboost/iterator/counting_iterator.hpp>

namespace autoboost
{
    template<class Value>
    inline iterator_range<counting_iterator<Value> >
    counting_range(Value first, Value last)
    {
        typedef counting_iterator<Value> counting_iterator_t;
        typedef iterator_range<counting_iterator_t> result_t;
        return result_t(counting_iterator_t(first),
                        counting_iterator_t(last));
    }

    template<class Range>
    inline iterator_range<
        counting_iterator<
            AUTOBOOST_DEDUCED_TYPENAME range_iterator<const Range>::type
        >
    >
    counting_range(const Range& rng)
    {
        typedef counting_iterator<
            AUTOBOOST_DEDUCED_TYPENAME range_iterator<const Range>::type
        > counting_iterator_t;

        typedef iterator_range<counting_iterator_t> result_t;

        return result_t(counting_iterator_t(autoboost::begin(rng)),
                        counting_iterator_t(autoboost::end(rng)));
    }

    template<class Range>
    inline iterator_range<
        counting_iterator<
            AUTOBOOST_DEDUCED_TYPENAME range_iterator<Range>::type
        >
    >
    counting_range(Range& rng)
    {
        typedef counting_iterator<
            AUTOBOOST_DEDUCED_TYPENAME range_iterator<Range>::type
        > counting_iterator_t;

        typedef iterator_range<counting_iterator_t> result_t;

        return result_t(counting_iterator_t(autoboost::begin(rng)),
                        counting_iterator_t(autoboost::end(rng)));
    }
} // namespace autoboost

#if AUTOBOOST_MSVC >= 1400
#pragma warning(pop)
#endif

#endif // include guard

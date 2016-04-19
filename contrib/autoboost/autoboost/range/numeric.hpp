// Copyright 2009-2014 Neil Groves.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Copyright 2006 Thorsten Ottosen.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Copyright 2004 Eric Niebler.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//   Contains range-based versions of the numeric std algorithms
//
#if defined(_MSC_VER)
    #pragma once
#endif

#ifndef AUTOBOOST_RANGE_NUMERIC_HPP
#define AUTOBOOST_RANGE_NUMERIC_HPP

#include <autoboost/config.hpp>
#include <autoboost/assert.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/category.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/distance.hpp>
#include <autoboost/range/size.hpp>
#include <numeric>


namespace autoboost
{
    template<class SinglePassRange, class Value>
    inline Value accumulate(const SinglePassRange& rng, Value init)
    {
        AUTOBOOST_RANGE_CONCEPT_ASSERT((
            SinglePassRangeConcept<const SinglePassRange>));

        return std::accumulate(autoboost::begin(rng), autoboost::end(rng), init);
    }

    template<class SinglePassRange, class Value, class BinaryOperation>
    inline Value accumulate(const SinglePassRange& rng, Value init,
                            BinaryOperation op)
    {
        AUTOBOOST_RANGE_CONCEPT_ASSERT((
                SinglePassRangeConcept<const SinglePassRange> ));

        return std::accumulate(autoboost::begin(rng), autoboost::end(rng), init, op);
    }

    namespace range_detail
    {
        template<class SinglePassRange1, class SinglePassRange2>
        inline bool inner_product_precondition(
            const SinglePassRange1&,
            const SinglePassRange2&,
            std::input_iterator_tag,
            std::input_iterator_tag)
        {
            return true;
        }

        template<class SinglePassRange1, class SinglePassRange2>
        inline bool inner_product_precondition(
            const SinglePassRange1& rng1,
            const SinglePassRange2& rng2,
            std::forward_iterator_tag,
            std::forward_iterator_tag)
        {
            return autoboost::size(rng2) >= autoboost::size(rng1);
        }

    } // namespace range_detail

    template<
        class SinglePassRange1,
        class SinglePassRange2,
        class Value
    >
    inline Value inner_product(
        const SinglePassRange1& rng1,
        const SinglePassRange2& rng2,
        Value                   init)
    {
        AUTOBOOST_RANGE_CONCEPT_ASSERT((
            SinglePassRangeConcept<const SinglePassRange1>));

        AUTOBOOST_RANGE_CONCEPT_ASSERT((
            SinglePassRangeConcept<const SinglePassRange2>));

        AUTOBOOST_ASSERT(
            range_detail::inner_product_precondition(
                    rng1, rng2,
                    typename range_category<const SinglePassRange1>::type(),
                    typename range_category<const SinglePassRange2>::type()));

        return std::inner_product(
            autoboost::begin(rng1), autoboost::end(rng1),
            autoboost::begin(rng2), init);
    }

    template<
        class SinglePassRange1,
        class SinglePassRange2,
        class Value,
        class BinaryOperation1,
        class BinaryOperation2
    >
    inline Value inner_product(
        const SinglePassRange1& rng1,
        const SinglePassRange2& rng2,
        Value                   init,
        BinaryOperation1        op1,
        BinaryOperation2        op2)
    {
        AUTOBOOST_RANGE_CONCEPT_ASSERT((
                SinglePassRangeConcept<const SinglePassRange1>));

        AUTOBOOST_RANGE_CONCEPT_ASSERT((
                SinglePassRangeConcept<const SinglePassRange2>));

        AUTOBOOST_ASSERT(
            range_detail::inner_product_precondition(
                rng1, rng2,
                typename range_category<const SinglePassRange1>::type(),
                typename range_category<const SinglePassRange2>::type()));

        return std::inner_product(
            autoboost::begin(rng1), autoboost::end(rng1),
            autoboost::begin(rng2), init, op1, op2);
    }

    template<class SinglePassRange, class OutputIterator>
    inline OutputIterator partial_sum(const SinglePassRange& rng,
                                      OutputIterator result)
    {
        AUTOBOOST_RANGE_CONCEPT_ASSERT((
                SinglePassRangeConcept<const SinglePassRange>));

        return std::partial_sum(autoboost::begin(rng), autoboost::end(rng), result);
    }

    template<class SinglePassRange, class OutputIterator, class BinaryOperation>
    inline OutputIterator partial_sum(
        const SinglePassRange&  rng,
        OutputIterator          result,
        BinaryOperation         op)
    {
        AUTOBOOST_RANGE_CONCEPT_ASSERT((
                SinglePassRangeConcept<const SinglePassRange>));

        return std::partial_sum(autoboost::begin(rng), autoboost::end(rng), result, op);
    }

    template<class SinglePassRange, class OutputIterator>
    inline OutputIterator adjacent_difference(
        const SinglePassRange&  rng,
        OutputIterator          result)
    {
        AUTOBOOST_RANGE_CONCEPT_ASSERT((
                SinglePassRangeConcept<const SinglePassRange>));

        return std::adjacent_difference(autoboost::begin(rng), autoboost::end(rng),
                                        result);
    }

    template<class SinglePassRange, class OutputIterator, class BinaryOperation>
    inline OutputIterator adjacent_difference(
        const SinglePassRange&  rng,
        OutputIterator          result,
        BinaryOperation         op)
    {
        AUTOBOOST_RANGE_CONCEPT_ASSERT((
                SinglePassRangeConcept<const SinglePassRange>));

        return std::adjacent_difference(autoboost::begin(rng), autoboost::end(rng),
                                        result, op);
    }

} // namespace autoboost

#endif

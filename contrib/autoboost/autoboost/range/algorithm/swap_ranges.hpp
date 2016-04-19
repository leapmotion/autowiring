//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_SWAP_RANGES_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_SWAP_RANGES_HPP_INCLUDED

#include <autoboost/assert.hpp>
#include <autoboost/concept_check.hpp>
#include <autoboost/iterator/iterator_categories.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/iterator.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range_detail
    {
        template<class Iterator1, class Iterator2>
        void swap_ranges_impl(Iterator1 it1, Iterator1 last1,
                              Iterator2 it2, Iterator2 last2,
                              single_pass_traversal_tag,
                              single_pass_traversal_tag)
        {
            ignore_unused_variable_warning(last2);
            for (; it1 != last1; ++it1, ++it2)
            {
                AUTOBOOST_ASSERT( it2 != last2 );
                std::iter_swap(it1, it2);
            }
        }

        template<class Iterator1, class Iterator2>
        void swap_ranges_impl(Iterator1 it1, Iterator1 last1,
                              Iterator2 it2, Iterator2 last2,
                              random_access_traversal_tag,
                              random_access_traversal_tag)
        {
            ignore_unused_variable_warning(last2);
            AUTOBOOST_ASSERT( last2 - it2 >= last1 - it1 );
            std::swap_ranges(it1, last1, it2);
        }

        template<class Iterator1, class Iterator2>
        void swap_ranges_impl(Iterator1 first1, Iterator1 last1,
                              Iterator2 first2, Iterator2 last2)
        {
            swap_ranges_impl(first1, last1, first2, last2,
                AUTOBOOST_DEDUCED_TYPENAME iterator_traversal<Iterator1>::type(),
                AUTOBOOST_DEDUCED_TYPENAME iterator_traversal<Iterator2>::type());
        }
    } // namespace range_detail

    namespace range
    {

/// \brief template function swap_ranges
///
/// range-based version of the swap_ranges std algorithm
///
/// \pre SinglePassRange1 is a model of the SinglePassRangeConcept
/// \pre SinglePassRange2 is a model of the SinglePassRangeConcept
template< class SinglePassRange1, class SinglePassRange2 >
inline SinglePassRange2&
swap_ranges(SinglePassRange1& range1, SinglePassRange2& range2)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<SinglePassRange1>));
    AUTOBOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<SinglePassRange2>));

    autoboost::range_detail::swap_ranges_impl(
        autoboost::begin(range1), autoboost::end(range1),
        autoboost::begin(range2), autoboost::end(range2));

    return range2;
}

/// \overload
template< class SinglePassRange1, class SinglePassRange2 >
inline SinglePassRange2&
swap_ranges(const SinglePassRange1& range1, SinglePassRange2& range2)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<const SinglePassRange1>));
    AUTOBOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<SinglePassRange2>));

    autoboost::range_detail::swap_ranges_impl(
        autoboost::begin(range1), autoboost::end(range1),
        autoboost::begin(range2), autoboost::end(range2));

    return range2;
}

/// \overload
template< class SinglePassRange1, class SinglePassRange2 >
inline const SinglePassRange2&
swap_ranges(SinglePassRange1& range1, const SinglePassRange2& range2)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<SinglePassRange1>));
    AUTOBOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<const SinglePassRange2>));

    autoboost::range_detail::swap_ranges_impl(
        autoboost::begin(range1), autoboost::end(range1),
        autoboost::begin(range2), autoboost::end(range2));

    return range2;
}

/// \overload
template< class SinglePassRange1, class SinglePassRange2 >
inline const SinglePassRange2&
swap_ranges(const SinglePassRange1& range1, const SinglePassRange2& range2)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<const SinglePassRange1>));
    AUTOBOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<const SinglePassRange2>));

    autoboost::range_detail::swap_ranges_impl(
        autoboost::begin(range1), autoboost::end(range1),
        autoboost::begin(range2), autoboost::end(range2));

    return range2;
}

    } // namespace range
    using range::swap_ranges;
} // namespace autoboost

#endif // include guard

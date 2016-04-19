//  Boost string_algo library iter_find.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef AUTOBOOST_STRING_ITER_FIND_HPP
#define AUTOBOOST_STRING_ITER_FIND_HPP

#include <autoboost/algorithm/string/config.hpp>
#include <algorithm>
#include <iterator>
#include <autoboost/iterator/transform_iterator.hpp>

#include <autoboost/range/iterator_range_core.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/iterator.hpp>
#include <autoboost/range/value_type.hpp>
#include <autoboost/range/as_literal.hpp>

#include <autoboost/algorithm/string/concept.hpp>
#include <autoboost/algorithm/string/find_iterator.hpp>
#include <autoboost/algorithm/string/detail/util.hpp>

/*! \file
    Defines generic split algorithms. Split algorithms can be
    used to divide a sequence into several part according
    to a given criteria. Result is given as a 'container
    of containers' where elements are copies or references
    to extracted parts.

    There are two algorithms provided. One iterates over matching
    substrings, the other one over the gaps between these matches.
*/

namespace autoboost {
    namespace algorithm {

//  iterate find ---------------------------------------------------//

        //! Iter find algorithm
        /*!
            This algorithm executes a given finder in iteration on the input,
            until the end of input is reached, or no match is found.
            Iteration is done using built-in find_iterator, so the real
            searching is performed only when needed.
            In each iteration new match is found and added to the result.

            \param Result A 'container container' to contain the result of search.
                Both outer and inner container must have constructor taking a pair
                of iterators as an argument.
                Typical type of the result is
                    \c std::vector<autoboost::iterator_range<iterator>>
                (each element of such a vector will container a range delimiting
                a match).
            \param Input A container which will be searched.
            \param Finder A Finder object used for searching
            \return A reference to the result

            \note Prior content of the result will be overwritten.
        */
        template<
            typename SequenceSequenceT,
            typename RangeT,
            typename FinderT >
        inline SequenceSequenceT&
        iter_find(
            SequenceSequenceT& Result,
            RangeT& Input,
            FinderT Finder )
        {
            AUTOBOOST_CONCEPT_ASSERT((
                FinderConcept<
                    FinderT,
                    AUTOBOOST_STRING_TYPENAME range_iterator<RangeT>::type>
                ));

            iterator_range<AUTOBOOST_STRING_TYPENAME range_iterator<RangeT>::type> lit_input(::autoboost::as_literal(Input));

            typedef AUTOBOOST_STRING_TYPENAME
                range_iterator<RangeT>::type input_iterator_type;
            typedef find_iterator<input_iterator_type> find_iterator_type;
            typedef detail::copy_iterator_rangeF<
                AUTOBOOST_STRING_TYPENAME
                    range_value<SequenceSequenceT>::type,
                input_iterator_type> copy_range_type;

            input_iterator_type InputEnd=::autoboost::end(lit_input);

            typedef transform_iterator<copy_range_type, find_iterator_type>
                transform_iter_type;

            transform_iter_type itBegin=
                ::autoboost::make_transform_iterator(
                    find_iterator_type( ::autoboost::begin(lit_input), InputEnd, Finder ),
                    copy_range_type());

            transform_iter_type itEnd=
                ::autoboost::make_transform_iterator(
                    find_iterator_type(),
                    copy_range_type());

            SequenceSequenceT Tmp(itBegin, itEnd);

            Result.swap(Tmp);
            return Result;
        }

//  iterate split ---------------------------------------------------//

        //! Split find algorithm
        /*!
            This algorithm executes a given finder in iteration on the input,
            until the end of input is reached, or no match is found.
            Iteration is done using built-in find_iterator, so the real
            searching is performed only when needed.
            Each match is used as a separator of segments. These segments are then
            returned in the result.

            \param Result A 'container container' to contain the result of search.
                Both outer and inner container must have constructor taking a pair
                of iterators as an argument.
                Typical type of the result is
                    \c std::vector<autoboost::iterator_range<iterator>>
                (each element of such a vector will container a range delimiting
                a match).
            \param Input A container which will be searched.
            \param Finder A finder object used for searching
            \return A reference to the result

            \note Prior content of the result will be overwritten.
        */
        template<
            typename SequenceSequenceT,
            typename RangeT,
            typename FinderT >
        inline SequenceSequenceT&
        iter_split(
            SequenceSequenceT& Result,
            RangeT& Input,
            FinderT Finder )
        {
            AUTOBOOST_CONCEPT_ASSERT((
                FinderConcept<FinderT,
                AUTOBOOST_STRING_TYPENAME range_iterator<RangeT>::type>
                ));

            iterator_range<AUTOBOOST_STRING_TYPENAME range_iterator<RangeT>::type> lit_input(::autoboost::as_literal(Input));

            typedef AUTOBOOST_STRING_TYPENAME
                range_iterator<RangeT>::type input_iterator_type;
            typedef split_iterator<input_iterator_type> find_iterator_type;
            typedef detail::copy_iterator_rangeF<
                AUTOBOOST_STRING_TYPENAME
                    range_value<SequenceSequenceT>::type,
                input_iterator_type> copy_range_type;

            input_iterator_type InputEnd=::autoboost::end(lit_input);

            typedef transform_iterator<copy_range_type, find_iterator_type>
                transform_iter_type;

            transform_iter_type itBegin=
                ::autoboost::make_transform_iterator(
                    find_iterator_type( ::autoboost::begin(lit_input), InputEnd, Finder ),
                    copy_range_type() );

            transform_iter_type itEnd=
                ::autoboost::make_transform_iterator(
                    find_iterator_type(),
                    copy_range_type() );

            SequenceSequenceT Tmp(itBegin, itEnd);

            Result.swap(Tmp);
            return Result;
        }

    } // namespace algorithm

    // pull names to the autoboost namespace
    using algorithm::iter_find;
    using algorithm::iter_split;

} // namespace autoboost


#endif  // AUTOBOOST_STRING_ITER_FIND_HPP

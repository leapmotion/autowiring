//  Boost string_algo library trim.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef AUTOBOOST_STRING_TRIM_HPP
#define AUTOBOOST_STRING_TRIM_HPP

#include <autoboost/algorithm/string/config.hpp>

#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/const_iterator.hpp>
#include <autoboost/range/as_literal.hpp>
#include <autoboost/range/iterator_range_core.hpp>

#include <autoboost/algorithm/string/detail/trim.hpp>
#include <autoboost/algorithm/string/classification.hpp>
#include <locale>

/*! \file
    Defines trim algorithms.
    Trim algorithms are used to remove trailing and leading spaces from a
    sequence (string). Space is recognized using given locales.

    Parametric (\c _if) variants use a predicate (functor) to select which characters
    are to be trimmed..
    Functions take a selection predicate as a parameter, which is used to determine
    whether a character is a space. Common predicates are provided in classification.hpp header.

*/

namespace autoboost {
    namespace algorithm {

    //  left trim  -----------------------------------------------//


        //! Left trim - parametric
        /*!
            Remove all leading spaces from the input.
            The supplied predicate is used to determine which characters are considered spaces.
            The result is a trimmed copy of the input. It is returned as a sequence
            or copied to the output iterator

            \param Output An output iterator to which the result will be copied
            \param Input An input range
            \param IsSpace A unary predicate identifying spaces
            \return
                An output iterator pointing just after the last inserted character or
                a copy of the input

               \note The second variant of this function provides the strong exception-safety guarantee
        */
        template<typename OutputIteratorT, typename RangeT, typename PredicateT>
        inline OutputIteratorT trim_left_copy_if(
            OutputIteratorT Output,
            const RangeT& Input,
            PredicateT IsSpace)
        {
            iterator_range<AUTOBOOST_STRING_TYPENAME range_const_iterator<RangeT>::type> lit_range(::autoboost::as_literal(Input));

            std::copy(
                ::autoboost::algorithm::detail::trim_begin(
                    ::autoboost::begin(lit_range),
                    ::autoboost::end(lit_range),
                    IsSpace ),
                ::autoboost::end(lit_range),
                Output);

            return Output;
        }

        //! Left trim - parametric
        /*!
            \overload
        */
        template<typename SequenceT, typename PredicateT>
        inline SequenceT trim_left_copy_if(const SequenceT& Input, PredicateT IsSpace)
        {
            return SequenceT(
                ::autoboost::algorithm::detail::trim_begin(
                    ::autoboost::begin(Input),
                    ::autoboost::end(Input),
                    IsSpace ),
                ::autoboost::end(Input));
        }

        //! Left trim - parametric
        /*!
            Remove all leading spaces from the input.
            The result is a trimmed copy of the input.

            \param Input An input sequence
            \param Loc a locale used for 'space' classification
            \return A trimmed copy of the input

            \note This function provides the strong exception-safety guarantee
        */
        template<typename SequenceT>
        inline SequenceT trim_left_copy(const SequenceT& Input, const std::locale& Loc=std::locale())
        {
            return
                ::autoboost::algorithm::trim_left_copy_if(
                    Input,
                    is_space(Loc));
        }

        //! Left trim
        /*!
            Remove all leading spaces from the input. The supplied predicate is
            used to determine which characters are considered spaces.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param IsSpace A unary predicate identifying spaces
        */
        template<typename SequenceT, typename PredicateT>
        inline void trim_left_if(SequenceT& Input, PredicateT IsSpace)
        {
            Input.erase(
                ::autoboost::begin(Input),
                ::autoboost::algorithm::detail::trim_begin(
                    ::autoboost::begin(Input),
                    ::autoboost::end(Input),
                    IsSpace));
        }

        //! Left trim
        /*!
            Remove all leading spaces from the input.
            The Input sequence is modified in-place.

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
        */
        template<typename SequenceT>
        inline void trim_left(SequenceT& Input, const std::locale& Loc=std::locale())
        {
            ::autoboost::algorithm::trim_left_if(
                Input,
                is_space(Loc));
        }

    //  right trim  -----------------------------------------------//

        //! Right trim - parametric
        /*!
            Remove all trailing spaces from the input.
            The supplied predicate is used to determine which characters are considered spaces.
            The result is a trimmed copy of the input. It is returned as a sequence
            or copied to the output iterator

            \param Output An output iterator to which the result will be copied
            \param Input An input range
            \param IsSpace A unary predicate identifying spaces
            \return
                An output iterator pointing just after the last inserted character or
                a copy of the input

             \note The second variant of this function provides the strong exception-safety guarantee
        */
        template<typename OutputIteratorT, typename RangeT, typename PredicateT>
        inline OutputIteratorT trim_right_copy_if(
            OutputIteratorT Output,
            const RangeT& Input,
            PredicateT IsSpace )
        {
            iterator_range<AUTOBOOST_STRING_TYPENAME range_const_iterator<RangeT>::type> lit_range(::autoboost::as_literal(Input));

            std::copy(
                ::autoboost::begin(lit_range),
                ::autoboost::algorithm::detail::trim_end(
                    ::autoboost::begin(lit_range),
                    ::autoboost::end(lit_range),
                    IsSpace ),
                Output );

            return Output;
        }

        //! Right trim - parametric
        /*!
            \overload
         */
        template<typename SequenceT, typename PredicateT>
        inline SequenceT trim_right_copy_if(const SequenceT& Input, PredicateT IsSpace)
        {
            return SequenceT(
                ::autoboost::begin(Input),
                ::autoboost::algorithm::detail::trim_end(
                    ::autoboost::begin(Input),
                    ::autoboost::end(Input),
                    IsSpace)
                );
        }

        //! Right trim
        /*!
            Remove all trailing spaces from the input.
            The result is a trimmed copy of the input

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
            \return A trimmed copy of the input

            \note This function provides the strong exception-safety guarantee
        */
        template<typename SequenceT>
        inline SequenceT trim_right_copy(const SequenceT& Input, const std::locale& Loc=std::locale())
        {
            return
                ::autoboost::algorithm::trim_right_copy_if(
                    Input,
                    is_space(Loc));
        }


        //! Right trim - parametric
        /*!
            Remove all trailing spaces from the input.
            The supplied predicate is used to determine which characters are considered spaces.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param IsSpace A unary predicate identifying spaces
        */
        template<typename SequenceT, typename PredicateT>
        inline void trim_right_if(SequenceT& Input, PredicateT IsSpace)
        {
            Input.erase(
                ::autoboost::algorithm::detail::trim_end(
                    ::autoboost::begin(Input),
                    ::autoboost::end(Input),
                    IsSpace ),
                ::autoboost::end(Input)
                );
        }


        //! Right trim
        /*!
            Remove all trailing spaces from the input.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
        */
        template<typename SequenceT>
        inline void trim_right(SequenceT& Input, const std::locale& Loc=std::locale())
        {
            ::autoboost::algorithm::trim_right_if(
                Input,
                is_space(Loc) );
        }

    //  both side trim  -----------------------------------------------//

        //! Trim - parametric
        /*!
            Remove all trailing and leading spaces from the input.
            The supplied predicate is used to determine which characters are considered spaces.
            The result is a trimmed copy of the input. It is returned as a sequence
            or copied to the output iterator

            \param Output An output iterator to which the result will be copied
            \param Input An input range
            \param IsSpace A unary predicate identifying spaces
            \return
                An output iterator pointing just after the last inserted character or
                a copy of the input

             \note The second variant of this function provides the strong exception-safety guarantee
        */
        template<typename OutputIteratorT, typename RangeT, typename PredicateT>
        inline OutputIteratorT trim_copy_if(
            OutputIteratorT Output,
            const RangeT& Input,
            PredicateT IsSpace)
        {
            iterator_range<AUTOBOOST_STRING_TYPENAME range_const_iterator<RangeT>::type> lit_range(::autoboost::as_literal(Input));

            AUTOBOOST_STRING_TYPENAME
                range_const_iterator<RangeT>::type TrimEnd=
                ::autoboost::algorithm::detail::trim_end(
                    ::autoboost::begin(lit_range),
                    ::autoboost::end(lit_range),
                    IsSpace);

            std::copy(
                detail::trim_begin(
                    ::autoboost::begin(lit_range), TrimEnd, IsSpace),
                TrimEnd,
                Output
                );

            return Output;
        }

        //! Trim - parametric
        /*!
            \overload
         */
        template<typename SequenceT, typename PredicateT>
        inline SequenceT trim_copy_if(const SequenceT& Input, PredicateT IsSpace)
        {
            AUTOBOOST_STRING_TYPENAME
                range_const_iterator<SequenceT>::type TrimEnd=
                    ::autoboost::algorithm::detail::trim_end(
                        ::autoboost::begin(Input),
                        ::autoboost::end(Input),
                        IsSpace);

            return SequenceT(
                detail::trim_begin(
                    ::autoboost::begin(Input),
                    TrimEnd,
                    IsSpace),
                TrimEnd
                );
        }

        //! Trim
        /*!
            Remove all leading and trailing spaces from the input.
            The result is a trimmed copy of the input

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
            \return A trimmed copy of the input

            \note This function provides the strong exception-safety guarantee
        */
        template<typename SequenceT>
        inline SequenceT trim_copy( const SequenceT& Input, const std::locale& Loc=std::locale() )
        {
            return
                ::autoboost::algorithm::trim_copy_if(
                    Input,
                    is_space(Loc) );
        }

        //! Trim
        /*!
            Remove all leading and trailing spaces from the input.
            The supplied predicate is used to determine which characters are considered spaces.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param IsSpace A unary predicate identifying spaces
        */
        template<typename SequenceT, typename PredicateT>
        inline void trim_if(SequenceT& Input, PredicateT IsSpace)
        {
            ::autoboost::algorithm::trim_right_if( Input, IsSpace );
            ::autoboost::algorithm::trim_left_if( Input, IsSpace );
        }

        //! Trim
        /*!
            Remove all leading and trailing spaces from the input.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
        */
        template<typename SequenceT>
        inline void trim(SequenceT& Input, const std::locale& Loc=std::locale())
        {
            ::autoboost::algorithm::trim_if(
                Input,
                is_space( Loc ) );
        }

    } // namespace algorithm

    // pull names to the autoboost namespace
    using algorithm::trim_left;
    using algorithm::trim_left_if;
    using algorithm::trim_left_copy;
    using algorithm::trim_left_copy_if;
    using algorithm::trim_right;
    using algorithm::trim_right_if;
    using algorithm::trim_right_copy;
    using algorithm::trim_right_copy_if;
    using algorithm::trim;
    using algorithm::trim_if;
    using algorithm::trim_copy;
    using algorithm::trim_copy_if;

} // namespace autoboost

#endif  // AUTOBOOST_STRING_TRIM_HPP

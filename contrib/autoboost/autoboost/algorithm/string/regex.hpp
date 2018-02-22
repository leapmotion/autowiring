//  Boost string_algo library regex.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef AUTOBOOST_STRING_REGEX_HPP
#define AUTOBOOST_STRING_REGEX_HPP

#include <autoboost/algorithm/string/config.hpp>
#include <autoboost/regex.hpp>

#include <autoboost/range/iterator_range_core.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/iterator.hpp>
#include <autoboost/range/as_literal.hpp>

#include <autoboost/algorithm/string/find_format.hpp>
#include <autoboost/algorithm/string/regex_find_format.hpp>
#include <autoboost/algorithm/string/formatter.hpp>
#include <autoboost/algorithm/string/iter_find.hpp>

/*! \file
    Defines regex variants of the algorithms.
*/

namespace autoboost {
    namespace algorithm {

//  find_regex  -----------------------------------------------//

        //! Find regex algorithm
        /*!
            Search for a substring matching the given regex in the input.

            \param Input A container which will be searched.
            \param Rx A regular expression
            \param Flags Regex options
            \return
                An \c iterator_range delimiting the match.
                Returned iterator is either \c RangeT::iterator or
                \c RangeT::const_iterator, depending on the constness of
                the input parameter.

              \note This function provides the strong exception-safety guarantee
        */
        template<
            typename RangeT,
            typename CharT,
            typename RegexTraitsT>
        inline iterator_range<
            AUTOBOOST_STRING_TYPENAME range_iterator<RangeT>::type >
        find_regex(
            RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            iterator_range<AUTOBOOST_STRING_TYPENAME range_iterator<RangeT>::type> lit_input(::autoboost::as_literal(Input));

            return ::autoboost::algorithm::regex_finder(Rx,Flags)(
                ::autoboost::begin(lit_input), ::autoboost::end(lit_input) );
        }

//  replace_regex --------------------------------------------------------------------//

        //! Replace regex algorithm
        /*!
            Search for a substring matching given regex and format it with
            the specified format.
            The result is a modified copy of the input. It is returned as a sequence
            or copied to the output iterator.

            \param Output An output iterator to which the result will be copied
            \param Input An input string
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
            \return An output iterator pointing just after the last inserted character or
                    a modified copy of the input

             \note The second variant of this function provides the strong exception-safety guarantee
        */
        template<
            typename OutputIteratorT,
            typename RangeT,
            typename CharT,
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline OutputIteratorT replace_regex_copy(
            OutputIteratorT Output,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return ::autoboost::algorithm::find_format_copy(
                Output,
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::regex_formatter( Format, Flags ) );
        }

        //! Replace regex algorithm
        /*!
            \overload
        */
        template<
            typename SequenceT,
            typename CharT,
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline SequenceT replace_regex_copy(
            const SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return ::autoboost::algorithm::find_format_copy(
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::regex_formatter( Format, Flags ) );
        }

        //! Replace regex algorithm
        /*!
            Search for a substring matching given regex and format it with
            the specified format. The input string is modified in-place.

            \param Input An input string
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
        */
        template<
            typename SequenceT,
            typename CharT,
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline void replace_regex(
            SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            ::autoboost::algorithm::find_format(
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::regex_formatter( Format, Flags ) );
        }

//  replace_all_regex --------------------------------------------------------------------//

        //! Replace all regex algorithm
        /*!
            Format all substrings, matching given regex, with the specified format.
            The result is a modified copy of the input. It is returned as a sequence
            or copied to the output iterator.

            \param Output An output iterator to which the result will be copied
            \param Input An input string
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
            \return An output iterator pointing just after the last inserted character or
                    a modified copy of the input

              \note The second variant of this function provides the strong exception-safety guarantee
        */
        template<
            typename OutputIteratorT,
            typename RangeT,
            typename CharT,
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline OutputIteratorT replace_all_regex_copy(
            OutputIteratorT Output,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return ::autoboost::algorithm::find_format_all_copy(
                Output,
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::regex_formatter( Format, Flags ) );
        }

        //! Replace all regex algorithm
        /*!
            \overload
        */
        template<
            typename SequenceT,
            typename CharT,
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline SequenceT replace_all_regex_copy(
            const SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return ::autoboost::algorithm::find_format_all_copy(
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::regex_formatter( Format, Flags ) );
        }

        //! Replace all regex algorithm
        /*!
            Format all substrings, matching given regex, with the specified format.
            The input string is modified in-place.

            \param Input An input string
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
        */
        template<
            typename SequenceT,
            typename CharT,
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline void replace_all_regex(
            SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            ::autoboost::algorithm::find_format_all(
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::regex_formatter( Format, Flags ) );
        }

//  erase_regex --------------------------------------------------------------------//

        //! Erase regex algorithm
        /*!
            Remove a substring matching given regex from the input.
            The result is a modified copy of the input. It is returned as a sequence
            or copied to the output iterator.

            \param Output An output iterator to which the result will be copied
            \param Input An input string
            \param Rx A regular expression
            \param Flags Regex options
            \return An output iterator pointing just after the last inserted character or
                       a modified copy of the input

             \note The second variant of this function provides the strong exception-safety guarantee
       */
        template<
            typename OutputIteratorT,
            typename RangeT,
            typename CharT,
            typename RegexTraitsT >
        inline OutputIteratorT erase_regex_copy(
            OutputIteratorT Output,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::autoboost::algorithm::find_format_copy(
                Output,
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::empty_formatter( Input ) );
        }

        //! Erase regex algorithm
        /*!
            \overload
        */
        template<
            typename SequenceT,
            typename CharT,
            typename RegexTraitsT >
        inline SequenceT erase_regex_copy(
            const SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::autoboost::algorithm::find_format_copy(
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::empty_formatter( Input ) );
        }

        //! Erase regex algorithm
        /*!
            Remove a substring matching given regex from the input.
            The input string is modified in-place.

            \param Input An input string
            \param Rx A regular expression
            \param Flags Regex options
        */
        template<
            typename SequenceT,
            typename CharT,
            typename RegexTraitsT >
        inline void erase_regex(
            SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            ::autoboost::algorithm::find_format(
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::empty_formatter( Input ) );
        }

//  erase_all_regex --------------------------------------------------------------------//

        //! Erase all regex algorithm
        /*!
            Erase all substrings, matching given regex, from the input.
            The result is a modified copy of the input. It is returned as a sequence
            or copied to the output iterator.


            \param Output An output iterator to which the result will be copied
            \param Input An input string
            \param Rx A regular expression
            \param Flags Regex options
            \return An output iterator pointing just after the last inserted character or
                    a modified copy of the input

             \note The second variant of this function provides the strong exception-safety guarantee
        */
        template<
            typename OutputIteratorT,
            typename RangeT,
            typename CharT,
            typename RegexTraitsT >
        inline OutputIteratorT erase_all_regex_copy(
            OutputIteratorT Output,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::autoboost::algorithm::find_format_all_copy(
                Output,
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::empty_formatter( Input ) );
        }

        //! Erase all regex algorithm
        /*!
            \overload
        */
        template<
            typename SequenceT,
            typename CharT,
            typename RegexTraitsT >
        inline SequenceT erase_all_regex_copy(
            const SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::autoboost::algorithm::find_format_all_copy(
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::empty_formatter( Input ) );
        }

        //! Erase all regex algorithm
        /*!
            Erase all substrings, matching given regex, from the input.
            The input string is modified in-place.

            \param Input An input string
            \param Rx A regular expression
            \param Flags Regex options
        */
        template<
            typename SequenceT,
            typename CharT,
            typename RegexTraitsT>
        inline void erase_all_regex(
            SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            ::autoboost::algorithm::find_format_all(
                Input,
                ::autoboost::algorithm::regex_finder( Rx, Flags ),
                ::autoboost::algorithm::empty_formatter( Input ) );
        }

//  find_all_regex ------------------------------------------------------------------//

        //! Find all regex algorithm
        /*!
            This algorithm finds all substrings matching the give regex
            in the input.

            Each part is copied and added as a new element to the output container.
            Thus the result container must be able to hold copies
            of the matches (in a compatible structure like std::string) or
            a reference to it (e.g. using the iterator range class).
            Examples of such a container are \c std::vector<std::string>
            or \c std::list<autoboost::iterator_range<std::string::iterator>>

            \param Result A container that can hold copies of references to the substrings.
            \param Input A container which will be searched.
            \param Rx A regular expression
            \param Flags Regex options
            \return A reference to the result

            \note Prior content of the result will be overwritten.

             \note This function provides the strong exception-safety guarantee
        */
        template<
            typename SequenceSequenceT,
            typename RangeT,
            typename CharT,
            typename RegexTraitsT >
        inline SequenceSequenceT& find_all_regex(
            SequenceSequenceT& Result,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::autoboost::algorithm::iter_find(
                Result,
                Input,
                ::autoboost::algorithm::regex_finder(Rx,Flags) );
        }

//  split_regex ------------------------------------------------------------------//

        //! Split regex algorithm
        /*!
            Tokenize expression. This function is equivalent to C strtok. Input
            sequence is split into tokens, separated  by separators. Separator
            is an every match of the given regex.
            Each part is copied and added as a new element to the output container.
            Thus the result container must be able to hold copies
            of the matches (in a compatible structure like std::string) or
            a reference to it (e.g. using the iterator range class).
            Examples of such a container are \c std::vector<std::string>
            or \c std::list<autoboost::iterator_range<std::string::iterator>>

            \param Result A container that can hold copies of references to the substrings.
            \param Input A container which will be searched.
            \param Rx A regular expression
            \param Flags Regex options
            \return A reference to the result

            \note Prior content of the result will be overwritten.

               \note This function provides the strong exception-safety guarantee
        */
        template<
            typename SequenceSequenceT,
            typename RangeT,
            typename CharT,
            typename RegexTraitsT >
        inline SequenceSequenceT& split_regex(
            SequenceSequenceT& Result,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::autoboost::algorithm::iter_split(
                Result,
                Input,
                ::autoboost::algorithm::regex_finder(Rx,Flags) );
        }

//  join_if ------------------------------------------------------------------//

#ifndef AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING

        //! Conditional join algorithm
        /*!
            This algorithm joins all strings in a 'list' into one long string.
            Segments are concatenated by given separator. Only segments that
            match the given regular expression will be added to the result

            This is a specialization of join_if algorithm.

            \param Input A container that holds the input strings. It must be a container-of-containers.
            \param Separator A string that will separate the joined segments.
            \param Rx A regular expression
            \param Flags Regex options
            \return Concatenated string.

            \note This function provides the strong exception-safety guarantee
        */
        template<
            typename SequenceSequenceT,
            typename Range1T,
            typename CharT,
            typename RegexTraitsT >
        inline typename range_value<SequenceSequenceT>::type
        join_if(
            const SequenceSequenceT& Input,
            const Range1T& Separator,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            // Define working types
            typedef typename range_value<SequenceSequenceT>::type ResultT;
            typedef typename range_const_iterator<SequenceSequenceT>::type InputIteratorT;

            // Parse input
            InputIteratorT itBegin=::autoboost::begin(Input);
            InputIteratorT itEnd=::autoboost::end(Input);

            // Construct container to hold the result
            ResultT Result;


            // Roll to the first element that will be added
            while(
                itBegin!=itEnd &&
                !::autoboost::regex_match(::autoboost::begin(*itBegin), ::autoboost::end(*itBegin), Rx, Flags)) ++itBegin;

            // Add this element
            if(itBegin!=itEnd)
            {
                detail::insert(Result, ::autoboost::end(Result), *itBegin);
                ++itBegin;
            }

            for(;itBegin!=itEnd; ++itBegin)
            {
                if(::autoboost::regex_match(::autoboost::begin(*itBegin), ::autoboost::end(*itBegin), Rx, Flags))
                {
                    // Add separator
                    detail::insert(Result, ::autoboost::end(Result), ::autoboost::as_literal(Separator));
                    // Add element
                    detail::insert(Result, ::autoboost::end(Result), *itBegin);
                }
            }

            return Result;
        }

#else  // AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING

                //! Conditional join algorithm
        /*!
            This algorithm joins all strings in a 'list' into one long string.
            Segments are concatenated by given separator. Only segments that
            match the given regular expression will be added to the result

            This is a specialization of join_if algorithm.

            \param Input A container that holds the input strings. It must be a container-of-containers.
            \param Separator A string that will separate the joined segments.
            \param Rx A regular expression
            \param Flags Regex options
            \return Concatenated string.

            \note This function provides the strong exception-safety guarantee
        */
        template<
            typename SequenceSequenceT,
            typename Range1T,
            typename CharT,
            typename RegexTraitsT >
        inline typename range_value<SequenceSequenceT>::type
        join_if_regex(
            const SequenceSequenceT& Input,
            const Range1T& Separator,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            // Define working types
            typedef typename range_value<SequenceSequenceT>::type ResultT;
            typedef typename range_const_iterator<SequenceSequenceT>::type InputIteratorT;

            // Parse input
            InputIteratorT itBegin=::autoboost::begin(Input);
            InputIteratorT itEnd=::autoboost::end(Input);

            // Construct container to hold the result
            ResultT Result;


            // Roll to the first element that will be added
            while(
                itBegin!=itEnd &&
                !::autoboost::regex_match(::autoboost::begin(*itBegin), ::autoboost::end(*itBegin), Rx, Flags)) ++itBegin;

            // Add this element
            if(itBegin!=itEnd)
            {
                detail::insert(Result, ::autoboost::end(Result), *itBegin);
                ++itBegin;
            }

            for(;itBegin!=itEnd; ++itBegin)
            {
                if(::autoboost::regex_match(::autoboost::begin(*itBegin), ::autoboost::end(*itBegin), Rx, Flags))
                {
                    // Add separator
                    detail::insert(Result, ::autoboost::end(Result), ::autoboost::as_literal(Separator));
                    // Add element
                    detail::insert(Result, ::autoboost::end(Result), *itBegin);
                }
            }

            return Result;
        }


#endif // AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING

    } // namespace algorithm

    // pull names into the autoboost namespace
    using algorithm::find_regex;
    using algorithm::replace_regex;
    using algorithm::replace_regex_copy;
    using algorithm::replace_all_regex;
    using algorithm::replace_all_regex_copy;
    using algorithm::erase_regex;
    using algorithm::erase_regex_copy;
    using algorithm::erase_all_regex;
    using algorithm::erase_all_regex_copy;
    using algorithm::find_all_regex;
    using algorithm::split_regex;

#ifndef AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING
    using algorithm::join_if;
#else  // AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING
    using algorithm::join_if_regex;
#endif // AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING

} // namespace autoboost


#endif  // AUTOBOOST_STRING_REGEX_HPP

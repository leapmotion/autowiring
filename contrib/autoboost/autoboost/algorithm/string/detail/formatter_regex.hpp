//  Boost string_algo library formatter_regex.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef AUTOBOOST_STRING_FORMATTER_REGEX_DETAIL_HPP
#define AUTOBOOST_STRING_FORMATTER_REGEX_DETAIL_HPP

#include <autoboost/algorithm/string/config.hpp>
#include <string>
#include <autoboost/regex.hpp>
#include <autoboost/algorithm/string/detail/finder_regex.hpp>

namespace autoboost {
    namespace algorithm {
        namespace detail {

//  regex format functor -----------------------------------------//

            // regex format functor
            template<typename StringT>
            struct regex_formatF
            {
            private:
                typedef StringT result_type;
                typedef AUTOBOOST_STRING_TYPENAME StringT::value_type char_type;

            public:
                // Construction
                regex_formatF( const StringT& Fmt, match_flag_type Flags=format_default ) :
                    m_Fmt(Fmt), m_Flags( Flags ) {}

                template<typename InputIteratorT>
                result_type operator()(
                    const regex_search_result<InputIteratorT>& Replace ) const
                {
                    if ( Replace.empty() )
                    {
                        return result_type();
                    }
                    else
                    {
                        return Replace.match_results().format( m_Fmt, m_Flags );
                    }
                }
            private:
                const StringT& m_Fmt;
                match_flag_type m_Flags;
            };


        } // namespace detail
    } // namespace algorithm
} // namespace autoboost

#endif  // AUTOBOOST_STRING_FORMATTER_DETAIL_HPP

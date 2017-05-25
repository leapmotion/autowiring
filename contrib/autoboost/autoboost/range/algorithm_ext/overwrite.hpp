// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_EXT_OVERWRITE_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_EXT_OVERWRITE_HPP_INCLUDED

#include <autoboost/range/config.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/difference_type.hpp>
#include <autoboost/range/iterator.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/assert.hpp>

namespace autoboost
{
    namespace range
    {

template< class SinglePassRange1, class SinglePassRange2 >
inline void overwrite( const SinglePassRange1& from, SinglePassRange2& to )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange1> ));
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<SinglePassRange2> ));

    AUTOBOOST_DEDUCED_TYPENAME range_iterator<const SinglePassRange1>::type
        i = autoboost::begin(from), e = autoboost::end(from);

    AUTOBOOST_DEDUCED_TYPENAME range_iterator<SinglePassRange2>::type
        out = autoboost::begin(to);

#ifndef NDEBUG
    AUTOBOOST_DEDUCED_TYPENAME range_iterator<SinglePassRange2>::type
        last_out = autoboost::end(to);
#endif

    for( ; i != e; ++out, ++i )
    {
#ifndef NDEBUG
        AUTOBOOST_ASSERT( out != last_out
            && "out of bounds in autoboost::overwrite()" );
#endif
        *out = *i;
    }
}

template< class SinglePassRange1, class SinglePassRange2 >
inline void overwrite( const SinglePassRange1& from, const SinglePassRange2& to )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange1> ));
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange2> ));

    AUTOBOOST_DEDUCED_TYPENAME range_iterator<const SinglePassRange1>::type
        i = autoboost::begin(from), e = autoboost::end(from);

    AUTOBOOST_DEDUCED_TYPENAME range_iterator<const SinglePassRange2>::type
        out = autoboost::begin(to);

#ifndef NDEBUG
    AUTOBOOST_DEDUCED_TYPENAME range_iterator<const SinglePassRange2>::type
        last_out = autoboost::end(to);
#endif

    for( ; i != e; ++out, ++i )
    {
#ifndef NDEBUG
        AUTOBOOST_ASSERT( out != last_out
            && "out of bounds in autoboost::overwrite()" );
#endif
        *out = *i;
    }
}

    } // namespace range
    using range::overwrite;
} // namespace autoboost

#endif // include guard

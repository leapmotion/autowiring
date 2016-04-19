// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_SIZE_HPP
#define AUTOBOOST_RANGE_SIZE_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/range/config.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/size_type.hpp>
#include <autoboost/range/detail/has_member_size.hpp>
#include <autoboost/assert.hpp>
#include <autoboost/cstdint.hpp>
#include <autoboost/utility.hpp>

namespace autoboost
{
    namespace range_detail
    {

        template<class SinglePassRange>
        inline typename ::autoboost::enable_if<
            has_member_size<SinglePassRange>,
            typename range_size<const SinglePassRange>::type
        >::type
        range_calculate_size(const SinglePassRange& rng)
        {
            return rng.size();
        }

        template<class SinglePassRange>
        inline typename disable_if<
            has_member_size<SinglePassRange>,
            typename range_size<const SinglePassRange>::type
        >::type
        range_calculate_size(const SinglePassRange& rng)
        {
            return std::distance(autoboost::begin(rng), autoboost::end(rng));
        }
    }

    template<class SinglePassRange>
    inline typename range_size<const SinglePassRange>::type
    size(const SinglePassRange& rng)
    {
// Very strange things happen on some compilers that have the range concept
// asserts disabled. This preprocessor condition is clearly redundant on a
// working compiler but is vital for at least some compilers such as clang 4.2
// but only on the Mac!
#if AUTOBOOST_RANGE_ENABLE_CONCEPT_ASSERT == 1
        AUTOBOOST_RANGE_CONCEPT_ASSERT((autoboost::SinglePassRangeConcept<SinglePassRange>));
#endif

#if !AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564)) && \
    !AUTOBOOST_WORKAROUND(__GNUC__, < 3) \
    /**/
        using namespace range_detail;
#endif

        return range_calculate_size(rng);
    }

} // namespace 'autoboost'

#endif

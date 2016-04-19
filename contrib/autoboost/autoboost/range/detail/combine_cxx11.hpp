//  Copyright Neil Groves 2014. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_DETAIL_COMBINE_CXX11_HPP
#define AUTOBOOST_RANGE_DETAIL_COMBINE_CXX11_HPP

#include <autoboost/range/iterator_range_core.hpp>
#include <autoboost/range/iterator.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/iterator/zip_iterator.hpp>

#include <fstream>

namespace autoboost
{
    namespace range
    {

template<typename... Ranges>
auto combine(Ranges&&... rngs) ->
    combined_range<decltype(autoboost::make_tuple(autoboost::begin(rngs)...))>
{
    return combined_range<decltype(autoboost::make_tuple(autoboost::begin(rngs)...))>(
                autoboost::make_tuple(autoboost::begin(rngs)...),
                autoboost::make_tuple(autoboost::end(rngs)...));
}

    } // namespace range

using range::combine;

} // namespace autoboost

#endif // include guard

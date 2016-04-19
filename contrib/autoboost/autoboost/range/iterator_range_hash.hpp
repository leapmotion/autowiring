// Boost.Range library
//
//  Copyright Neil Groves 2014
//  Use, modification and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#include <autoboost/range/iterator_range_core.hpp>
#include <autoboost/functional/hash.hpp>

namespace autoboost
{

template<class T>
std::size_t hash_value(const iterator_range<T>& rng)
{
    return autoboost::hash_range(rng.begin(), rng.end());
}

} // namespace autoboost

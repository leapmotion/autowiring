// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_EXT_ERASE_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_EXT_ERASE_HPP_INCLUDED

#include <autoboost/range/config.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/difference_type.hpp>
#include <autoboost/range/iterator_range_core.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/assert.hpp>

namespace autoboost
{
    namespace range
    {

template< class Container >
inline Container& erase( Container& on,
      iterator_range<AUTOBOOST_DEDUCED_TYPENAME Container::iterator> to_erase )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<Container> ));
    on.erase( autoboost::begin(to_erase), autoboost::end(to_erase) );
    return on;
}

template< class Container, class T >
inline Container& remove_erase( Container& on, const T& val )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<Container> ));
    on.erase(
        std::remove(autoboost::begin(on), autoboost::end(on), val),
        autoboost::end(on));
    return on;
}

template< class Container, class Pred >
inline Container& remove_erase_if( Container& on, Pred pred )
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<Container> ));
    on.erase(
        std::remove_if(autoboost::begin(on), autoboost::end(on), pred),
        autoboost::end(on));
    return on;
}

    } // namespace range
    using range::erase;
    using range::remove_erase;
    using range::remove_erase_if;
} // namespace autoboost

#endif // include guard

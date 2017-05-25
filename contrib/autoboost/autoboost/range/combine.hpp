//  Copyright Neil Groves 2010. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_COMBINE_HPP
#define AUTOBOOST_RANGE_COMBINE_HPP

#include <autoboost/config.hpp>
#include <autoboost/range/iterator_range_core.hpp>
#include <autoboost/iterator/zip_iterator.hpp>

namespace autoboost
{
    namespace range
    {

template<typename IterTuple>
class combined_range
        : public iterator_range<zip_iterator<IterTuple> >
{
    typedef iterator_range<zip_iterator<IterTuple> > base;
public:
    combined_range(IterTuple first, IterTuple last)
        : base(first, last)
    {
    }
};

    } // namespace range
} // namespace autoboost

#if defined(AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS) || \
    defined(AUTOBOOST_NO_CXX11_DECLTYPE) || \
    defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) || \
    defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
#   include <autoboost/range/detail/combine_cxx03.hpp>
#else
#   include <autoboost/range/detail/combine_cxx11.hpp>
#endif

#endif

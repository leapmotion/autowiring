///////////////////////////////////////////////////////////////////////////////
// max.hpp
//
//  Copyright 2005 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_STATISTICS_MAX_HPP_EAN_28_10_2005
#define AUTOBOOST_ACCUMULATORS_STATISTICS_MAX_HPP_EAN_28_10_2005

#include <limits>
#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/accumulators/framework/accumulator_base.hpp>
#include <autoboost/accumulators/framework/extractor.hpp>
#include <autoboost/accumulators/framework/parameters/sample.hpp>
#include <autoboost/accumulators/numeric/functional.hpp>
#include <autoboost/accumulators/framework/depends_on.hpp>
#include <autoboost/accumulators/statistics_fwd.hpp>

namespace autoboost { namespace accumulators
{

namespace impl
{
    ///////////////////////////////////////////////////////////////////////////////
    // max_impl
    template<typename Sample>
    struct max_impl
      : accumulator_base
    {
        // for autoboost::result_of
        typedef Sample result_type;

        template<typename Args>
        max_impl(Args const &args)
          : max_(numeric::as_min(args[sample | Sample()]))
        {
        }

        template<typename Args>
        void operator ()(Args const &args)
        {
            numeric::max_assign(this->max_, args[sample]);
        }

        result_type result(dont_care) const
        {
            return this->max_;
        }

    private:
        Sample max_;
    };

} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::max
//
namespace tag
{
    struct max
      : depends_on<>
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::max_impl<mpl::_1> impl;
    };
}

///////////////////////////////////////////////////////////////////////////////
// extract::max
//
namespace extract
{
    extractor<tag::max> const max = {};

    AUTOBOOST_ACCUMULATORS_IGNORE_GLOBAL(max)
}

using extract::max;

}} // namespace autoboost::accumulators

#endif

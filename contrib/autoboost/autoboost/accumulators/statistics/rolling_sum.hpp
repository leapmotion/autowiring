///////////////////////////////////////////////////////////////////////////////
// rolling_sum.hpp
//
// Copyright 2008 Eric Niebler. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_STATISTICS_ROLLING_SUM_HPP_EAN_26_12_2008
#define AUTOBOOST_ACCUMULATORS_STATISTICS_ROLLING_SUM_HPP_EAN_26_12_2008

#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/accumulators/framework/accumulator_base.hpp>
#include <autoboost/accumulators/framework/extractor.hpp>
#include <autoboost/accumulators/numeric/functional.hpp>
#include <autoboost/accumulators/framework/parameters/sample.hpp>
#include <autoboost/accumulators/framework/depends_on.hpp>
#include <autoboost/accumulators/statistics_fwd.hpp>
#include <autoboost/accumulators/statistics/rolling_window.hpp>

namespace autoboost { namespace accumulators
{
namespace impl
{
    ///////////////////////////////////////////////////////////////////////////////
    // rolling_sum_impl
    //    returns the sum of the samples in the rolling window
    template<typename Sample>
    struct rolling_sum_impl
      : accumulator_base
    {
        typedef Sample result_type;

        template<typename Args>
        rolling_sum_impl(Args const &args)
          : sum_(args[sample | Sample()])
        {}

        template<typename Args>
        void operator ()(Args const &args)
        {
            if(is_rolling_window_plus1_full(args))
            {
                this->sum_ -= rolling_window_plus1(args).front();
            }
            this->sum_ += args[sample];
        }

        template<typename Args>
        result_type result(Args const & /*args*/) const
        {
            return this->sum_;
        }

    private:
        Sample sum_;
    };
} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::rolling_sum
//
namespace tag
{
    struct rolling_sum
      : depends_on< rolling_window_plus1 >
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::rolling_sum_impl< mpl::_1 > impl;

        #ifdef AUTOBOOST_ACCUMULATORS_DOXYGEN_INVOKED
        /// tag::rolling_window::window_size named parameter
        static autoboost::parameter::keyword<tag::rolling_window_size> const window_size;
        #endif
    };
} // namespace tag

///////////////////////////////////////////////////////////////////////////////
// extract::rolling_sum
//
namespace extract
{
    extractor<tag::rolling_sum> const rolling_sum = {};

    AUTOBOOST_ACCUMULATORS_IGNORE_GLOBAL(rolling_sum)
}

using extract::rolling_sum;
}} // namespace autoboost::accumulators

#endif

///////////////////////////////////////////////////////////////////////////////
// weighted_sum.hpp
//
//  Copyright 2006 Eric Niebler, Olivier Gygi. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_STATISTICS_WEIGHTED_SUM_HPP_EAN_28_10_2005
#define AUTOBOOST_ACCUMULATORS_STATISTICS_WEIGHTED_SUM_HPP_EAN_28_10_2005

#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/accumulators/framework/accumulator_base.hpp>
#include <autoboost/accumulators/framework/extractor.hpp>
#include <autoboost/accumulators/numeric/functional.hpp>
#include <autoboost/accumulators/framework/parameters/sample.hpp>
#include <autoboost/accumulators/framework/parameters/weight.hpp>
#include <autoboost/accumulators/framework/accumulators/external_accumulator.hpp>
#include <autoboost/accumulators/framework/depends_on.hpp>
#include <autoboost/accumulators/statistics_fwd.hpp>

namespace autoboost { namespace accumulators
{

namespace impl
{
    ///////////////////////////////////////////////////////////////////////////////
    // weighted_sum_impl
    template<typename Sample, typename Weight, typename Tag>
    struct weighted_sum_impl
      : accumulator_base
    {
        typedef typename numeric::functional::multiplies<Sample, Weight>::result_type weighted_sample;

        // for autoboost::result_of
        typedef weighted_sample result_type;

        template<typename Args>
        weighted_sum_impl(Args const &args)
          : weighted_sum_(
                args[parameter::keyword<Tag>::get() | Sample()]
                  * numeric::one<Weight>::value
            )
        {
        }

        template<typename Args>
        void operator ()(Args const &args)
        {
            // what about overflow?
            this->weighted_sum_ += args[parameter::keyword<Tag>::get()] * args[weight];
        }

        result_type result(dont_care) const
        {
            return this->weighted_sum_;
        }

    private:

        weighted_sample weighted_sum_;
    };

} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::weighted_sum
//
namespace tag
{
    struct weighted_sum
      : depends_on<>
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::weighted_sum_impl<mpl::_1, mpl::_2, tag::sample> impl;
    };

    template<typename VariateType, typename VariateTag>
    struct weighted_sum_of_variates
      : depends_on<>
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::weighted_sum_impl<VariateType, mpl::_2, VariateTag> impl;
    };

    struct abstract_weighted_sum_of_variates
      : depends_on<>
    {
    };
}

///////////////////////////////////////////////////////////////////////////////
// extract::weighted_sum
//
namespace extract
{
    extractor<tag::weighted_sum> const weighted_sum = {};
    extractor<tag::abstract_weighted_sum_of_variates> const weighted_sum_of_variates = {};

    AUTOBOOST_ACCUMULATORS_IGNORE_GLOBAL(weighted_sum)
    AUTOBOOST_ACCUMULATORS_IGNORE_GLOBAL(weighted_sum_of_variates)
}

using extract::weighted_sum;
using extract::weighted_sum_of_variates;

template<typename VariateType, typename VariateTag>
struct feature_of<tag::weighted_sum_of_variates<VariateType, VariateTag> >
  : feature_of<tag::abstract_weighted_sum_of_variates>
{
};

}} // namespace autoboost::accumulators

#endif

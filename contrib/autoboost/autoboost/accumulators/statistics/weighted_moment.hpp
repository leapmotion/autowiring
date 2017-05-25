///////////////////////////////////////////////////////////////////////////////
// weighted_moment.hpp
//
//  Copyright 2006, Eric Niebler, Olivier Gygi. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_STATISTICS_WEIGHTED_MOMENT_HPP_EAN_15_11_2005
#define AUTOBOOST_ACCUMULATORS_STATISTICS_WEIGHTED_MOMENT_HPP_EAN_15_11_2005

#include <autoboost/config/no_tr1/cmath.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/assert.hpp>
#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/preprocessor/arithmetic/inc.hpp>
#include <autoboost/preprocessor/repetition/repeat_from_to.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing_binary_params.hpp>
#include <autoboost/accumulators/framework/accumulator_base.hpp>
#include <autoboost/accumulators/framework/extractor.hpp>
#include <autoboost/accumulators/numeric/functional.hpp>
#include <autoboost/accumulators/framework/parameters/sample.hpp>
#include <autoboost/accumulators/framework/depends_on.hpp>
#include <autoboost/accumulators/statistics_fwd.hpp>
#include <autoboost/accumulators/statistics/count.hpp>
#include <autoboost/accumulators/statistics/moment.hpp> // for pow()
#include <autoboost/accumulators/statistics/sum.hpp>

namespace autoboost { namespace accumulators
{

namespace impl
{
    ///////////////////////////////////////////////////////////////////////////////
    // weighted_moment_impl
    template<typename N, typename Sample, typename Weight>
    struct weighted_moment_impl
      : accumulator_base // TODO: also depends_on sum of powers
    {
        AUTOBOOST_MPL_ASSERT_RELATION(N::value, >, 0);
        typedef typename numeric::functional::multiplies<Sample, Weight>::result_type weighted_sample;
        // for autoboost::result_of
        typedef typename numeric::functional::fdiv<weighted_sample, Weight>::result_type result_type;

        template<typename Args>
        weighted_moment_impl(Args const &args)
          : sum(args[sample | Sample()] * numeric::one<Weight>::value)
        {
        }

        template<typename Args>
        void operator ()(Args const &args)
        {
            this->sum += args[weight] * numeric::pow(args[sample], N());
        }

        template<typename Args>
        result_type result(Args const &args) const
        {
            return numeric::fdiv(this->sum, sum_of_weights(args));
        }

    private:
        weighted_sample sum;
    };

} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::weighted_moment
//
namespace tag
{
    template<int N>
    struct weighted_moment
      : depends_on<count, sum_of_weights>
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::weighted_moment_impl<mpl::int_<N>, mpl::_1, mpl::_2> impl;
    };
}

///////////////////////////////////////////////////////////////////////////////
// extract::weighted_moment
//
namespace extract
{
    AUTOBOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, weighted_moment, (int))
}

using extract::weighted_moment;

}} // namespace autoboost::accumulators

#endif

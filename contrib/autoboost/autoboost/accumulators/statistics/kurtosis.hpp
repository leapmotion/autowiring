///////////////////////////////////////////////////////////////////////////////
// kurtosis.hpp
//
//  Copyright 2006 Olivier Gygi, Daniel Egloff. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_STATISTICS_KURTOSIS_HPP_EAN_28_10_2005
#define AUTOBOOST_ACCUMULATORS_STATISTICS_KURTOSIS_HPP_EAN_28_10_2005

#include <limits>
#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/accumulators/framework/accumulator_base.hpp>
#include <autoboost/accumulators/framework/extractor.hpp>
#include <autoboost/accumulators/framework/parameters/sample.hpp>
#include <autoboost/accumulators/numeric/functional.hpp>
#include <autoboost/accumulators/framework/depends_on.hpp>
#include <autoboost/accumulators/statistics/mean.hpp>
#include <autoboost/accumulators/statistics/moment.hpp>

namespace autoboost { namespace accumulators
{

namespace impl
{
    ///////////////////////////////////////////////////////////////////////////////
    // kurtosis_impl
    /**
        @brief Kurtosis estimation

        The kurtosis of a sample distribution is defined as the ratio of the 4th central moment and the square of the 2nd central
        moment (the variance) of the samples, minus 3. The term \f$ -3 \f$ is added in order to ensure that the normal distribution
        has zero kurtosis. The kurtosis can also be expressed by the simple moments:

        \f[
            \hat{g}_2 =
                \frac
                {\widehat{m}_n^{(4)}-4\widehat{m}_n^{(3)}\hat{\mu}_n+6\widehat{m}_n^{(2)}\hat{\mu}_n^2-3\hat{\mu}_n^4}
                {\left(\widehat{m}_n^{(2)} - \hat{\mu}_n^{2}\right)^2} - 3,
        \f]

        where \f$ \widehat{m}_n^{(i)} \f$ are the \f$ i \f$-th moment and \f$ \hat{\mu}_n \f$ the mean (first moment) of the
        \f$ n \f$ samples.
    */
    template<typename Sample>
    struct kurtosis_impl
      : accumulator_base
    {
        // for autoboost::result_of
        typedef typename numeric::functional::fdiv<Sample, Sample>::result_type result_type;

        kurtosis_impl(dont_care) {}

        template<typename Args>
        result_type result(Args const &args) const
        {
            return numeric::fdiv(
                        accumulators::moment<4>(args)
                        - 4. * accumulators::moment<3>(args) * mean(args)
                        + 6. * accumulators::moment<2>(args) * mean(args) * mean(args)
                        - 3. * mean(args) * mean(args) * mean(args) * mean(args)
                      , ( accumulators::moment<2>(args) - mean(args) * mean(args) )
                        * ( accumulators::moment<2>(args) - mean(args) * mean(args) )
                    ) - 3.;
        }
    };

} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::kurtosis
//
namespace tag
{
    struct kurtosis
      : depends_on<mean, moment<2>, moment<3>, moment<4> >
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::kurtosis_impl<mpl::_1> impl;
    };
}

///////////////////////////////////////////////////////////////////////////////
// extract::kurtosis
//
namespace extract
{
    extractor<tag::kurtosis> const kurtosis = {};

    AUTOBOOST_ACCUMULATORS_IGNORE_GLOBAL(kurtosis)
}

using extract::kurtosis;

// So that kurtosis can be automatically substituted with
// weighted_kurtosis when the weight parameter is non-void
template<>
struct as_weighted_feature<tag::kurtosis>
{
    typedef tag::weighted_kurtosis type;
};

template<>
struct feature_of<tag::weighted_kurtosis>
  : feature_of<tag::kurtosis>
{
};

}} // namespace autoboost::accumulators

#endif

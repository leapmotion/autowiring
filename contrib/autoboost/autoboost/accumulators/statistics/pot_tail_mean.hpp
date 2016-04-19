///////////////////////////////////////////////////////////////////////////////
// pot_tail_mean.hpp
//
//  Copyright 2006 Daniel Egloff, Olivier Gygi. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_STATISTICS_POT_TAIL_MEAN_HPP_DE_01_01_2006
#define AUTOBOOST_ACCUMULATORS_STATISTICS_POT_TAIL_MEAN_HPP_DE_01_01_2006

#include <vector>
#include <limits>
#include <numeric>
#include <functional>
#include <autoboost/range.hpp>
#include <autoboost/parameter/keyword.hpp>
#include <autoboost/tuple/tuple.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/type_traits/is_same.hpp>
#include <autoboost/accumulators/framework/accumulator_base.hpp>
#include <autoboost/accumulators/framework/extractor.hpp>
#include <autoboost/accumulators/numeric/functional.hpp>
#include <autoboost/accumulators/framework/parameters/sample.hpp>
#include <autoboost/accumulators/statistics_fwd.hpp>
#include <autoboost/accumulators/statistics/peaks_over_threshold.hpp>
#include <autoboost/accumulators/statistics/weighted_peaks_over_threshold.hpp>
#include <autoboost/accumulators/statistics/pot_quantile.hpp>
#include <autoboost/accumulators/statistics/tail_mean.hpp>

namespace autoboost { namespace accumulators
{

namespace impl
{
    ///////////////////////////////////////////////////////////////////////////////
    // pot_tail_mean_impl
    //
    /**
        @brief Estimation of the (coherent) tail mean based on the peaks over threshold method (for both left and right tails)

        Computes an estimate for the (coherent) tail mean
        \f[
            \widehat{CTM}_{\alpha} = \hat{q}_{\alpha} - \frac{\bar{\beta}}{\xi-1}(1-\alpha)^{-\xi},
        \f]
        where \f$\bar[u]\f$, \f$\bar{\beta}\f$ and \f$\xi\f$ are the parameters of the
        generalized Pareto distribution that approximates the right tail of the distribution (or the
        mirrored left tail, in case the left tail is used). In the latter case, the result is mirrored
        back, yielding the correct result.
    */
    template<typename Sample, typename Impl, typename LeftRight>
    struct pot_tail_mean_impl
      : accumulator_base
    {
        typedef typename numeric::functional::fdiv<Sample, std::size_t>::result_type float_type;
        // for autoboost::result_of
        typedef float_type result_type;

        pot_tail_mean_impl(dont_care)
          : sign_((is_same<LeftRight, left>::value) ? -1 : 1)
        {
        }

        template<typename Args>
        result_type result(Args const &args) const
        {
            typedef
                typename mpl::if_<
                    is_same<Impl, weighted>
                  , tag::weighted_peaks_over_threshold<LeftRight>
                  , tag::peaks_over_threshold<LeftRight>
                >::type
            peaks_over_threshold_tag;

            typedef
                typename mpl::if_<
                    is_same<Impl, weighted>
                  , tag::weighted_pot_quantile<LeftRight>
                  , tag::pot_quantile<LeftRight>
                >::type
            pot_quantile_tag;

            extractor<peaks_over_threshold_tag> const some_peaks_over_threshold = {};
            extractor<pot_quantile_tag> const some_pot_quantile = {};

            float_type beta_bar = some_peaks_over_threshold(args).template get<1>();
            float_type xi_hat   = some_peaks_over_threshold(args).template get<2>();

            return some_pot_quantile(args) - this->sign_ * beta_bar/( xi_hat - 1. ) * std::pow(
                is_same<LeftRight, left>::value ? args[quantile_probability] : 1. - args[quantile_probability]
              , -xi_hat);
        }
    private:
        short sign_; // if the fit parameters from the mirrored left tail extreme values are used, mirror back the result
    };
} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::pot_tail_mean
// tag::pot_tail_mean_prob
//
namespace tag
{
    template<typename LeftRight>
    struct pot_tail_mean
      : depends_on<peaks_over_threshold<LeftRight>, pot_quantile<LeftRight> >
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::pot_tail_mean_impl<mpl::_1, unweighted, LeftRight> impl;
    };
    template<typename LeftRight>
    struct pot_tail_mean_prob
      : depends_on<peaks_over_threshold_prob<LeftRight>, pot_quantile_prob<LeftRight> >
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::pot_tail_mean_impl<mpl::_1, unweighted, LeftRight> impl;
    };
    template<typename LeftRight>
    struct weighted_pot_tail_mean
      : depends_on<weighted_peaks_over_threshold<LeftRight>, weighted_pot_quantile<LeftRight> >
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::pot_tail_mean_impl<mpl::_1, weighted, LeftRight> impl;
    };
    template<typename LeftRight>
    struct weighted_pot_tail_mean_prob
      : depends_on<weighted_peaks_over_threshold_prob<LeftRight>, weighted_pot_quantile_prob<LeftRight> >
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::pot_tail_mean_impl<mpl::_1, weighted, LeftRight> impl;
    };
}

// pot_tail_mean<LeftRight>(with_threshold_value) -> pot_tail_mean<LeftRight>
template<typename LeftRight>
struct as_feature<tag::pot_tail_mean<LeftRight>(with_threshold_value)>
{
    typedef tag::pot_tail_mean<LeftRight> type;
};

// pot_tail_mean<LeftRight>(with_threshold_probability) -> pot_tail_mean_prob<LeftRight>
template<typename LeftRight>
struct as_feature<tag::pot_tail_mean<LeftRight>(with_threshold_probability)>
{
    typedef tag::pot_tail_mean_prob<LeftRight> type;
};

// weighted_pot_tail_mean<LeftRight>(with_threshold_value) -> weighted_pot_tail_mean<LeftRight>
template<typename LeftRight>
struct as_feature<tag::weighted_pot_tail_mean<LeftRight>(with_threshold_value)>
{
    typedef tag::weighted_pot_tail_mean<LeftRight> type;
};

// weighted_pot_tail_mean<LeftRight>(with_threshold_probability) -> weighted_pot_tail_mean_prob<LeftRight>
template<typename LeftRight>
struct as_feature<tag::weighted_pot_tail_mean<LeftRight>(with_threshold_probability)>
{
    typedef tag::weighted_pot_tail_mean_prob<LeftRight> type;
};

// for the purposes of feature-based dependency resolution,
// pot_tail_mean<LeftRight> and pot_tail_mean_prob<LeftRight> provide
// the same feature as tail_mean
template<typename LeftRight>
struct feature_of<tag::pot_tail_mean<LeftRight> >
  : feature_of<tag::tail_mean>
{
};

template<typename LeftRight>
struct feature_of<tag::pot_tail_mean_prob<LeftRight> >
  : feature_of<tag::tail_mean>
{
};

// So that pot_tail_mean can be automatically substituted
// with weighted_pot_tail_mean when the weight parameter is non-void.
template<typename LeftRight>
struct as_weighted_feature<tag::pot_tail_mean<LeftRight> >
{
    typedef tag::weighted_pot_tail_mean<LeftRight> type;
};

template<typename LeftRight>
struct feature_of<tag::weighted_pot_tail_mean<LeftRight> >
  : feature_of<tag::pot_tail_mean<LeftRight> >
{
};

// So that pot_tail_mean_prob can be automatically substituted
// with weighted_pot_tail_mean_prob when the weight parameter is non-void.
template<typename LeftRight>
struct as_weighted_feature<tag::pot_tail_mean_prob<LeftRight> >
{
    typedef tag::weighted_pot_tail_mean_prob<LeftRight> type;
};

template<typename LeftRight>
struct feature_of<tag::weighted_pot_tail_mean_prob<LeftRight> >
  : feature_of<tag::pot_tail_mean_prob<LeftRight> >
{
};

}} // namespace autoboost::accumulators

#endif

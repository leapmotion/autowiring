///////////////////////////////////////////////////////////////////////////////
// rolling_moment.hpp
// Copyright 2005 Eric Niebler.
// Copyright (C) 2014 Pieter Bastiaan Ober (Integricom).
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_STATISTICS_ROLLING_MOMENT_HPP_EAN_27_11_2005
#define AUTOBOOST_ACCUMULATORS_STATISTICS_ROLLING_MOMENT_HPP_EAN_27_11_2005

#include <autoboost/config/no_tr1/cmath.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/assert.hpp>
#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/accumulators/framework/accumulator_base.hpp>
#include <autoboost/accumulators/framework/extractor.hpp>
#include <autoboost/accumulators/numeric/functional.hpp>
#include <autoboost/accumulators/framework/parameters/sample.hpp>
#include <autoboost/accumulators/framework/depends_on.hpp>
#include <autoboost/accumulators/statistics_fwd.hpp>
#include <autoboost/accumulators/statistics/moment.hpp>
#include <autoboost/accumulators/statistics/rolling_count.hpp>

namespace autoboost { namespace accumulators
{
namespace impl
{
    ///////////////////////////////////////////////////////////////////////////////
    // rolling_moment_impl
    template<typename N, typename Sample>
    struct rolling_moment_impl
      : accumulator_base
    {
        AUTOBOOST_MPL_ASSERT_RELATION(N::value, >, 0);
        // for autoboost::result_of
        typedef typename numeric::functional::fdiv<Sample, std::size_t,void,void>::result_type result_type;

        template<typename Args>
        rolling_moment_impl(Args const &args)
          : sum_(args[sample | Sample()])
        {
        }

        template<typename Args>
        void operator ()(Args const &args)
        {
            if(is_rolling_window_plus1_full(args))
            {
                this->sum_ -= numeric::pow(rolling_window_plus1(args).front(), N());
            }
            this->sum_ += numeric::pow(args[sample], N());
        }

        template<typename Args>
        result_type result(Args const &args) const
        {
            return numeric::fdiv(this->sum_, rolling_count(args));
        }

    private:
        result_type sum_;
    };
} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::rolling_moment
//
namespace tag
{
    template<int N>
    struct rolling_moment
      : depends_on< rolling_window_plus1, rolling_count>
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::rolling_moment_impl<mpl::int_<N>, mpl::_1> impl;

        #ifdef AUTOBOOST_ACCUMULATORS_DOXYGEN_INVOKED
        /// tag::rolling_window::window_size named parameter
        static autoboost::parameter::keyword<tag::rolling_window_size> const window_size;
        #endif
    };
}

///////////////////////////////////////////////////////////////////////////////
// extract::rolling_moment
//
namespace extract
{
    AUTOBOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, rolling_moment, (int))
}

using extract::rolling_moment;

// There is no weighted_rolling_moment (yet)...
//
//// So that rolling_moment<N> can be automatically substituted with
//// weighted_rolling_moment<N> when the weight parameter is non-void
//template<int N>
//struct as_weighted_feature<tag::rolling_moment<N> >
//{
//    typedef tag::weighted_rolling_moment<N> type;
//};
//
//template<int N>
//struct feature_of<tag::weighted_rolling_moment<N> >
//  : feature_of<tag::rolling_moment<N> >
//{
//};
}} // namespace autoboost::accumulators

#endif

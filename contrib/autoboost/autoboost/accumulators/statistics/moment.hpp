///////////////////////////////////////////////////////////////////////////////
// moment.hpp
//
//  Copyright 2005 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_STATISTICS_MOMENT_HPP_EAN_15_11_2005
#define AUTOBOOST_ACCUMULATORS_STATISTICS_MOMENT_HPP_EAN_15_11_2005

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
#include <autoboost/accumulators/statistics/count.hpp>

namespace autoboost { namespace numeric
{
    /// INTERNAL ONLY
    ///
    template<typename T>
    T const &pow(T const &x, mpl::int_<1>)
    {
        return x;
    }

    /// INTERNAL ONLY
    ///
    template<typename T, int N>
    T pow(T const &x, mpl::int_<N>)
    {
        using namespace operators;
        T y = numeric::pow(x, mpl::int_<N/2>());
        T z = y * y;
        return (N % 2) ? (z * x) : z;
    }
}}

namespace autoboost { namespace accumulators
{

namespace impl
{
    ///////////////////////////////////////////////////////////////////////////////
    // moment_impl
    template<typename N, typename Sample>
    struct moment_impl
      : accumulator_base // TODO: also depends_on sum of powers
    {
        AUTOBOOST_MPL_ASSERT_RELATION(N::value, >, 0);
        // for autoboost::result_of
        typedef typename numeric::functional::fdiv<Sample, std::size_t>::result_type result_type;

        template<typename Args>
        moment_impl(Args const &args)
          : sum(args[sample | Sample()])
        {
        }

        template<typename Args>
        void operator ()(Args const &args)
        {
            this->sum += numeric::pow(args[sample], N());
        }

        template<typename Args>
        result_type result(Args const &args) const
        {
            return numeric::fdiv(this->sum, count(args));
        }

    private:
        Sample sum;
    };

} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::moment
//
namespace tag
{
    template<int N>
    struct moment
      : depends_on<count>
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::moment_impl<mpl::int_<N>, mpl::_1> impl;
    };
}

///////////////////////////////////////////////////////////////////////////////
// extract::moment
//
namespace extract
{
    AUTOBOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, moment, (int))
}

using extract::moment;

// So that moment<N> can be automatically substituted with
// weighted_moment<N> when the weight parameter is non-void
template<int N>
struct as_weighted_feature<tag::moment<N> >
{
    typedef tag::weighted_moment<N> type;
};

template<int N>
struct feature_of<tag::weighted_moment<N> >
  : feature_of<tag::moment<N> >
{
};

}} // namespace autoboost::accumulators

#endif

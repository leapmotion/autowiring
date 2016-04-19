///////////////////////////////////////////////////////////////////////////////
// tail_variate.hpp
//
//  Copyright 2005 Eric Niebler, Michael Gauckler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_STAT_STATISTICS_TAIL_VARIATE_HPP_EAN_28_10_2005
#define AUTOBOOST_STAT_STATISTICS_TAIL_VARIATE_HPP_EAN_28_10_2005

#include <autoboost/range.hpp>
#include <autoboost/mpl/always.hpp>
#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/iterator/reverse_iterator.hpp>
#include <autoboost/iterator/permutation_iterator.hpp>
#include <autoboost/accumulators/framework/accumulator_base.hpp>
#include <autoboost/accumulators/framework/extractor.hpp>
#include <autoboost/accumulators/framework/depends_on.hpp>
#include <autoboost/accumulators/statistics_fwd.hpp>
#include <autoboost/accumulators/statistics/tail.hpp>

namespace autoboost { namespace accumulators
{

namespace impl
{
    ///////////////////////////////////////////////////////////////////////////////
    // tail_variate_impl
    template<typename VariateType, typename VariateTag, typename LeftRight>
    struct tail_variate_impl
      : accumulator_base
    {
        // for autoboost::result_of
        typedef
            typename detail::tail_range<
                typename std::vector<VariateType>::const_iterator
              , std::vector<std::size_t>::iterator
            >::type
        result_type;

        template<typename Args>
        tail_variate_impl(Args const &args)
          : variates(args[tag::tail<LeftRight>::cache_size], args[parameter::keyword<VariateTag>::get() | VariateType()])
        {
        }

        template<typename Args>
        void assign(Args const &args, std::size_t index)
        {
            this->variates[index] = args[parameter::keyword<VariateTag>::get()];
        }

        template<typename Args>
        result_type result(Args const &args) const
        {
            // getting the order result causes the indices vector to be sorted.
            extractor<tag::tail<LeftRight> > const some_tail = {};
            return this->do_result(some_tail(args));
        }

    private:
        template<typename TailRng>
        result_type do_result(TailRng const &rng) const
        {
            return detail::make_tail_range(
                this->variates.begin()
              , rng.end().base().base()   // the index iterator
              , rng.begin().base().base() // (begin and end reversed because these are reverse iterators)
            );
        }

        std::vector<VariateType> variates;
    };

} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::tail_variate<>
//
namespace tag
{
    template<typename VariateType, typename VariateTag, typename LeftRight>
    struct tail_variate
      : depends_on<tail<LeftRight> >
    {
        /// INTERNAL ONLY
        ///
        typedef mpl::always<accumulators::impl::tail_variate_impl<VariateType, VariateTag, LeftRight> > impl;
    };

    struct abstract_tail_variate
      : depends_on<>
    {
    };

    template<typename LeftRight>
    struct tail_weights
      : depends_on<tail<LeftRight> >
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::tail_variate_impl<mpl::_2, tag::weight, LeftRight> impl;
    };

    struct abstract_tail_weights
      : depends_on<>
    {
    };
}

///////////////////////////////////////////////////////////////////////////////
// extract::tail_variate
// extract::tail_weights
//
namespace extract
{
    extractor<tag::abstract_tail_variate> const tail_variate = {};
    extractor<tag::abstract_tail_weights> const tail_weights = {};

    AUTOBOOST_ACCUMULATORS_IGNORE_GLOBAL(tail_variate)
    AUTOBOOST_ACCUMULATORS_IGNORE_GLOBAL(tail_weights)
}

using extract::tail_variate;
using extract::tail_weights;

template<typename VariateType, typename VariateTag, typename LeftRight>
struct feature_of<tag::tail_variate<VariateType, VariateTag, LeftRight> >
  : feature_of<tag::abstract_tail_variate>
{
};

template<typename LeftRight>
struct feature_of<tag::tail_weights<LeftRight> >
{
    typedef tag::abstract_tail_weights type;
};

}} // namespace autoboost::accumulators

#endif

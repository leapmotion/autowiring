///////////////////////////////////////////////////////////////////////////////
// rolling_window.hpp
//
// Copyright 2008 Eric Niebler. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_STATISTICS_ROLLING_WINDOW_HPP_EAN_26_12_2008
#define AUTOBOOST_ACCUMULATORS_STATISTICS_ROLLING_WINDOW_HPP_EAN_26_12_2008

#include <cstddef>
#include <autoboost/version.hpp>
#include <autoboost/assert.hpp>
#include <autoboost/circular_buffer.hpp>
#include <autoboost/range/iterator_range.hpp>
#include <autoboost/accumulators/accumulators_fwd.hpp>
#include <autoboost/accumulators/framework/extractor.hpp>
#include <autoboost/accumulators/framework/depends_on.hpp>
#include <autoboost/accumulators/framework/accumulator_base.hpp>
#include <autoboost/accumulators/framework/parameters/sample.hpp>
#include <autoboost/accumulators/framework/parameters/accumulator.hpp>
#include <autoboost/accumulators/numeric/functional.hpp>
#include <autoboost/accumulators/statistics_fwd.hpp>

namespace autoboost { namespace accumulators
{

///////////////////////////////////////////////////////////////////////////////
// tag::rolling_window::size named parameter
AUTOBOOST_PARAMETER_NESTED_KEYWORD(tag, rolling_window_size, window_size)

AUTOBOOST_ACCUMULATORS_IGNORE_GLOBAL(rolling_window_size)

namespace impl
{
    ///////////////////////////////////////////////////////////////////////////////
    // rolling_window_plus1_impl
    //    stores the latest N+1 samples, where N is specified at construction time
    //    with the rolling_window_size named parameter
    template<typename Sample>
    struct rolling_window_plus1_impl
      : accumulator_base
    {
        typedef typename circular_buffer<Sample>::const_iterator const_iterator;
        typedef iterator_range<const_iterator> result_type;

        template<typename Args>
        rolling_window_plus1_impl(Args const & args)
          : buffer_(args[rolling_window_size] + 1)
        {}

        #if AUTOBOOST_VERSION < 103600
        // Before Boost 1.36, copying a circular buffer didn't copy
        // it's capacity, and we need that behavior.
        rolling_window_plus1_impl(rolling_window_plus1_impl const &that)
          : buffer_(that.buffer_)
        {
            this->buffer_.set_capacity(that.buffer_.capacity());
        }

        rolling_window_plus1_impl &operator =(rolling_window_plus1_impl const &that)
        {
            this->buffer_ = that.buffer_;
            this->buffer_.set_capacity(that.buffer_.capacity());
        }
        #endif

        template<typename Args>
        void operator ()(Args const &args)
        {
            this->buffer_.push_back(args[sample]);
        }

        bool full() const
        {
            return this->buffer_.full();
        }

        // The result of a shifted rolling window is the range including
        // everything except the most recently added element.
        result_type result(dont_care) const
        {
            return result_type(this->buffer_.begin(), this->buffer_.end());
        }

    private:
        circular_buffer<Sample> buffer_;
    };

    template<typename Args>
    bool is_rolling_window_plus1_full(Args const &args)
    {
        return find_accumulator<tag::rolling_window_plus1>(args[accumulator]).full();
    }

    ///////////////////////////////////////////////////////////////////////////////
    // rolling_window_impl
    //    stores the latest N samples, where N is specified at construction type
    //    with the rolling_window_size named parameter
    template<typename Sample>
    struct rolling_window_impl
      : accumulator_base
    {
        typedef typename circular_buffer<Sample>::const_iterator const_iterator;
        typedef iterator_range<const_iterator> result_type;

        rolling_window_impl(dont_care)
        {}

        template<typename Args>
        result_type result(Args const &args) const
        {
            return rolling_window_plus1(args).advance_begin(is_rolling_window_plus1_full(args));
        }
    };

} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::rolling_window_plus1
// tag::rolling_window
//
namespace tag
{
    struct rolling_window_plus1
      : depends_on<>
      , tag::rolling_window_size
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::rolling_window_plus1_impl< mpl::_1 > impl;

        #ifdef AUTOBOOST_ACCUMULATORS_DOXYGEN_INVOKED
        /// tag::rolling_window::size named parameter
        static autoboost::parameter::keyword<tag::rolling_window_size> const window_size;
        #endif
    };

    struct rolling_window
      : depends_on< rolling_window_plus1 >
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::rolling_window_impl< mpl::_1 > impl;

        #ifdef AUTOBOOST_ACCUMULATORS_DOXYGEN_INVOKED
        /// tag::rolling_window::size named parameter
        static autoboost::parameter::keyword<tag::rolling_window_size> const window_size;
        #endif
    };

} // namespace tag

///////////////////////////////////////////////////////////////////////////////
// extract::rolling_window_plus1
// extract::rolling_window
//
namespace extract
{
    extractor<tag::rolling_window_plus1> const rolling_window_plus1 = {};
    extractor<tag::rolling_window> const rolling_window = {};

    AUTOBOOST_ACCUMULATORS_IGNORE_GLOBAL(rolling_window_plus1)
    AUTOBOOST_ACCUMULATORS_IGNORE_GLOBAL(rolling_window)
}

using extract::rolling_window_plus1;
using extract::rolling_window;

}} // namespace autoboost::accumulators

#endif

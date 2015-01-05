///////////////////////////////////////////////////////////////////////////////
// value_accumulator.hpp
//
//  Copyright 2005 Eric Niebler, Daniel Egloff. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_FRAMEWORK_ACCUMULATORS_VALUE_ACCUMULATOR_HPP_EAN_03_23_2006
#define AUTOBOOST_ACCUMULATORS_FRAMEWORK_ACCUMULATORS_VALUE_ACCUMULATOR_HPP_EAN_03_23_2006

#include <autoboost/mpl/always.hpp>
#include <autoboost/parameter/keyword.hpp>
#include <autoboost/accumulators/framework/depends_on.hpp> // for feature_tag
#include <autoboost/accumulators/framework/accumulator_base.hpp>
#include <autoboost/accumulators/framework/extractor.hpp>

namespace autoboost { namespace accumulators
{

namespace impl
{

    //////////////////////////////////////////////////////////////////////////
    // value_accumulator_impl
    template<typename ValueType, typename Tag>
    struct value_accumulator_impl
      : accumulator_base
    {
        typedef ValueType result_type;

        template<typename Args>
        value_accumulator_impl(Args const &args)
          : val(args[parameter::keyword<Tag>::get()])
        {
        }

        result_type result(dont_care) const
        {
            return this->val;
        }

    private:
        ValueType val;
    };

} // namespace impl

namespace tag
{
    //////////////////////////////////////////////////////////////////////////
    // value_tag
    template<typename Tag>
    struct value_tag
    {
    };

    //////////////////////////////////////////////////////////////////////////
    // value
    template<typename ValueType, typename Tag>
    struct value
      : depends_on<>
    {
        /// INTERNAL ONLY
        ///
        typedef mpl::always<accumulators::impl::value_accumulator_impl<ValueType, Tag> > impl;
    };
}

namespace extract
{
    AUTOBOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, value, (typename)(typename))
    AUTOBOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, value_tag, (typename))
}

using extract::value;
using extract::value_tag;

// Map all value<V,T> features to value_tag<T> so
// that values can be extracted using value_tag<T>
// without specifying the value type.
template<typename ValueType, typename Tag>
struct feature_of<tag::value<ValueType, Tag> >
  : feature_of<tag::value_tag<Tag> >
{
};

}} // namespace autoboost::accumulators

#endif

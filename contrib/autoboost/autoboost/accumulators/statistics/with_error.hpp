///////////////////////////////////////////////////////////////////////////////
// with_error.hpp
//
//  Copyright 2005 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_STATISTICS_WITH_ERROR_HPP_EAN_01_11_2005
#define AUTOBOOST_ACCUMULATORS_STATISTICS_WITH_ERROR_HPP_EAN_01_11_2005

#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/mpl/vector.hpp>
#include <autoboost/mpl/transform_view.hpp>
#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/accumulators/statistics_fwd.hpp>
#include <autoboost/accumulators/statistics/error_of.hpp>

namespace autoboost { namespace accumulators
{

namespace detail
{
    template<typename Feature>
    struct error_of_tag
    {
        typedef tag::error_of<Feature> type;
    };
}

///////////////////////////////////////////////////////////////////////////////
// with_error
//
template<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_ACCUMULATORS_MAX_FEATURES, typename Feature)>
struct with_error
  : mpl::transform_view<
        mpl::vector<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_ACCUMULATORS_MAX_FEATURES, Feature)>
      , detail::error_of_tag<mpl::_1>
    >
{
};

}} // namespace autoboost::accumulators

#endif

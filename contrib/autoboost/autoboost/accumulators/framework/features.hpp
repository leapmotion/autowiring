///////////////////////////////////////////////////////////////////////////////
// features.hpp
//
//  Copyright 2005 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_STATISTICS_STATS_HPP_EAN_08_12_2005
#define AUTOBOOST_ACCUMULATORS_STATISTICS_STATS_HPP_EAN_08_12_2005

#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/mpl/vector.hpp>
#include <autoboost/accumulators/accumulators_fwd.hpp>

namespace autoboost { namespace accumulators
{

///////////////////////////////////////////////////////////////////////////////
// features
//
template<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_ACCUMULATORS_MAX_FEATURES, typename Feature)>
struct features
  : mpl::vector<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_ACCUMULATORS_MAX_FEATURES, Feature)>
{
};

}} // namespace autoboost::accumulators

#endif

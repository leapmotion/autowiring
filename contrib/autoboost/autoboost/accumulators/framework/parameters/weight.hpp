///////////////////////////////////////////////////////////////////////////////
// weight.hpp
//
//  Copyright 2005 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_FRAMEWORK_PARAMETERS_WEIGHT_HPP_EAN_31_10_2005
#define AUTOBOOST_ACCUMULATORS_FRAMEWORK_PARAMETERS_WEIGHT_HPP_EAN_31_10_2005

#include <autoboost/parameter/keyword.hpp>
#include <autoboost/accumulators/accumulators_fwd.hpp>

namespace autoboost { namespace accumulators
{

// The weight of a single sample
AUTOBOOST_PARAMETER_KEYWORD(tag, weight)
AUTOBOOST_ACCUMULATORS_IGNORE_GLOBAL(weight)

}} // namespace autoboost::accumulators

#endif

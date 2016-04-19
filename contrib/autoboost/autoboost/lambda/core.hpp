// -- core.hpp -- Boost Lambda Library -------------------------------------
//
// Copyright (C) 2000 Gary Powell (powellg@amazon.com)
// Copyright (C) 1999, 2000 Jaakko Jarvi (jaakko.jarvi@cs.utu.fi)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see www.boost.org
//
// Includes the core of LL, without any real features for client:
//
// tuples, lambda functors, return type deduction templates,
// argument substitution mechanism (select functions)
//
// Some functionality comes as well:
// Assignment and subscript operators, as well as function
// call operator for placeholder variables.
// -------------------------------------------------------------------------

#ifndef AUTOBOOST_LAMBDA_CORE_HPP
#define AUTOBOOST_LAMBDA_CORE_HPP

#include "autoboost/type_traits/transform_traits.hpp"
#include "autoboost/type_traits/cv_traits.hpp"

#include "autoboost/tuple/tuple.hpp"

// inject some of the tuple names into lambda
namespace autoboost {
namespace lambda {

using ::autoboost::tuples::tuple;
using ::autoboost::tuples::null_type;

} // lambda
} // autoboost

#include "autoboost/lambda/detail/lambda_config.hpp"
#include "autoboost/lambda/detail/lambda_fwd.hpp"

#include "autoboost/lambda/detail/arity_code.hpp"
#include "autoboost/lambda/detail/actions.hpp"

#include "autoboost/lambda/detail/lambda_traits.hpp"

#include "autoboost/lambda/detail/function_adaptors.hpp"
#include "autoboost/lambda/detail/return_type_traits.hpp"

#include "autoboost/lambda/detail/select_functions.hpp"

#include "autoboost/lambda/detail/lambda_functor_base.hpp"

#include "autoboost/lambda/detail/lambda_functors.hpp"

#include "autoboost/lambda/detail/ret.hpp"

namespace autoboost {
namespace lambda {

namespace {

  // These are constants types and need to be initialised
  autoboost::lambda::placeholder1_type free1 = autoboost::lambda::placeholder1_type();
  autoboost::lambda::placeholder2_type free2 = autoboost::lambda::placeholder2_type();
  autoboost::lambda::placeholder3_type free3 = autoboost::lambda::placeholder3_type();

  autoboost::lambda::placeholder1_type& _1 = free1;
  autoboost::lambda::placeholder2_type& _2 = free2;
  autoboost::lambda::placeholder3_type& _3 = free3;
  // _1, _2, ... naming scheme by Peter Dimov
} // unnamed

} // lambda
} // autoboost


#endif //AUTOBOOST_LAMBDA_CORE_HPP

// Boost.Function library - Typeof support
//  Copyright (C) Douglas Gregor 2008
//
//  Use, modification and distribution is subject to the Boost
//  Software License, Version 1.0.  (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org
#ifndef AUTOBOOST_FUNCTION_TYPEOF_HPP
#define AUTOBOOST_FUNCTION_TYPEOF_HPP
#include <autoboost/function/function_fwd.hpp>
#include <autoboost/typeof/typeof.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TYPE(autoboost::bad_function_call)

#if !defined(AUTOBOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function, (typename))
#endif

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function0, (typename))
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function1, (typename)(typename))
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function2, (typename)(typename)(typename))
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function3,
  (typename)(typename)(typename)(typename))
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function4,
  (typename)(typename)(typename)(typename)(typename))
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function5,
  (typename)(typename)(typename)(typename)(typename)(typename))
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function6,
  (typename)(typename)(typename)(typename)(typename)(typename)(typename))
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function7,
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename))
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function8,
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename)(typename))
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function9,
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename)(typename)(typename))
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::function10,
  (typename)(typename)(typename)(typename)(typename)(typename)(typename)
  (typename)(typename)(typename)(typename))
#endif

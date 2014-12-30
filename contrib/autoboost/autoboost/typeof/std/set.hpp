// Copyright (C) 2005 Arkadiy Vertleyb, Peder Holt.
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_STD_set_hpp_INCLUDED
#define AUTOBOOST_TYPEOF_STD_set_hpp_INCLUDED

#include <set>
#include <autoboost/typeof/typeof.hpp>
#include <autoboost/typeof/std/memory.hpp>
#include <autoboost/typeof/std/functional.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::set, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::set, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::set, 3)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::multiset, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::multiset, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::multiset, 3)

#endif//AUTOBOOST_TYPEOF_STD_set_hpp_INCLUDED

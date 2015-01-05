// Copyright (C) 2005 Arkadiy Vertleyb, Peder Holt.
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_STD_queue_hpp_INCLUDED
#define AUTOBOOST_TYPEOF_STD_queue_hpp_INCLUDED

#include <queue>
#include <autoboost/typeof/typeof.hpp>
#include <autoboost/typeof/std/memory.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::queue, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::queue, 2)

#endif//AUTOBOOST_TYPEOF_STD_queue_hpp_INCLUDED

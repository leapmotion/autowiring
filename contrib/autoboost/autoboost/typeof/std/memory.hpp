// Copyright (C) 2005 Arkadiy Vertleyb, Peder Holt.
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_STD_memory_hpp_INCLUDED
#define AUTOBOOST_TYPEOF_STD_memory_hpp_INCLUDED

#include <memory>
#include <autoboost/typeof/typeof.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::allocator, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::raw_storage_iterator, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::auto_ptr, 1)

#endif//AUTOBOOST_TYPEOF_STD_memory_hpp_INCLUDED

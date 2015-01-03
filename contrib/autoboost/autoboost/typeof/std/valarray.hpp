// Copyright (C) 2005 Arkadiy Vertleyb, Peder Holt.
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_STD_valarray_hpp_INCLUDED
#define AUTOBOOST_TYPEOF_STD_valarray_hpp_INCLUDED

#include <valarray>
#include <autoboost/typeof/typeof.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::valarray, 1)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::slice)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::slice_array, 1)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::gslice)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::gslice_array, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::mask_array, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::indirect_array, 1)

#endif//AUTOBOOST_TYPEOF_STD_valarray_hpp_INCLUDED

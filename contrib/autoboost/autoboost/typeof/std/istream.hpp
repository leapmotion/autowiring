// Copyright (C) 2005 Arkadiy Vertleyb, Peder Holt.
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_STD_istream_hpp_INCLUDED
#define AUTOBOOST_TYPEOF_STD_istream_hpp_INCLUDED

#include <istream>
#include <autoboost/typeof/typeof.hpp>
#include <autoboost/typeof/std/string.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_istream, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_istream, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_iostream, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_iostream, 2)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::istream)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::iostream)

#endif//AUTOBOOST_TYPEOF_STD_istream_hpp_INCLUDED

// Copyright (C) 2005 Arkadiy Vertleyb, Peder Holt.
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_STD_sstream_hpp_INCLUDED
#define AUTOBOOST_TYPEOF_STD_sstream_hpp_INCLUDED

#include <sstream>
#include <autoboost/typeof/typeof.hpp>
#include <autoboost/typeof/std/string.hpp>
#include <autoboost/typeof/std/memory.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_stringbuf, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_stringbuf, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_stringbuf, 3)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_istringstream, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_istringstream, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_istringstream, 3)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_ostringstream, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_ostringstream, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_ostringstream, 3)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_stringstream, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_stringstream, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_stringstream, 3)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::stringbuf)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::istringstream)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::ostringstream)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::stringstream)

#endif//AUTOBOOST_TYPEOF_STD_sstream_hpp_INCLUDED

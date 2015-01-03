// Copyright (C) 2005 Arkadiy Vertleyb, Peder Holt.
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_STD_string_hpp_INCLUDED
#define AUTOBOOST_TYPEOF_STD_string_hpp_INCLUDED

#include <string>
#include <autoboost/typeof/typeof.hpp>
#include <autoboost/typeof/std/memory.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::char_traits, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_string, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_string, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_string, 3)

#ifndef __BORLANDC__
//Borland chokes on this "double definition" of string
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::string)
#endif

#endif//AUTOBOOST_TYPEOF_STD_string_hpp_INCLUDED

// Copyright (C) 2005 Arkadiy Vertleyb, Peder Holt.
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_STD_fstream_hpp_INCLUDED
#define AUTOBOOST_TYPEOF_STD_fstream_hpp_INCLUDED

#include <fstream>
#include <autoboost/typeof/typeof.hpp>
#include <autoboost/typeof/std/string.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_filebuf, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_filebuf, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_ifstream, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_ifstream, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_ofstream, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_ofstream, 2)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_fstream, 1)
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(std::basic_fstream, 2)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::filebuf)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::ifstream)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::ofstream)
AUTOBOOST_TYPEOF_REGISTER_TYPE(std::fstream)

#endif//AUTOBOOST_TYPEOF_STD_fstream_hpp_INCLUDED

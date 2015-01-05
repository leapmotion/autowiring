//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 20010.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or
// copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Based on the unique_threader/unique_joiner design from of Kevlin Henney (n1883)
//
// See http://www.boost.org/libs/chrono for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CHRONO_TYPEOF_RATIO_HPP
#define AUTOBOOST_CHRONO_TYPEOF_RATIO_HPP

#include <autoboost/ratio/ratio.hpp>
#include <autoboost/typeof/typeof.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::ratio, (autoboost::intmax_t)(autoboost::intmax_t))

#endif

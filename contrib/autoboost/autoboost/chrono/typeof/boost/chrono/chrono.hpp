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

#ifndef AUTOBOOST_CHRONO_TYPEOF_CHRONO_HPP
#define AUTOBOOST_CHRONO_TYPEOF_CHRONO_HPP

#include <autoboost/chrono/chrono.hpp>
#include <autoboost/typeof/typeof.hpp>

#include AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::chrono::duration, (typename)(typename))
AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(autoboost::chrono::time_point, (typename)(typename))
#if 0
AUTOBOOST_TYPEOF_REGISTER_TYPE(autoboost::chrono::system_clock)
#ifdef AUTOBOOST_CHRONO_HAS_CLOCK_STEADY
AUTOBOOST_TYPEOF_REGISTER_TYPE(autoboost::chrono::steady_clock)
#endif
AUTOBOOST_TYPEOF_REGISTER_TYPE(autoboost::chrono::high_resolution_clock)

#endif
#endif

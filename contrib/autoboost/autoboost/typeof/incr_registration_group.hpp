// Copyright (C) 2004, 2005 Arkadiy Vertleyb
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

// Inclusion of this file increments AUTOBOOST_TYPEOF_REGISTRATION_GROUP
// This method was suggested by Paul Mensonides

#ifdef AUTOBOOST_TYPEOF_EMULATION
#   undef AUTOBOOST_TYPEOF_REGISTRATION_GROUP

#   include <autoboost/preprocessor/slot/counter.hpp>
#   include AUTOBOOST_PP_UPDATE_COUNTER()
#   define AUTOBOOST_TYPEOF_REGISTRATION_GROUP AUTOBOOST_PP_COUNTER
#endif

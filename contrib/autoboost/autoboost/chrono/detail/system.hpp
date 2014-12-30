//  Copyright 2009-2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_CHRONO_DETAIL_SYSTEM_HPP
#define AUTOBOOST_CHRONO_DETAIL_SYSTEM_HPP

#if !defined AUTOBOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING

#include <autoboost/version.hpp>
#include <autoboost/system/error_code.hpp>

#if ((AUTOBOOST_VERSION / 100000) < 2) && ((AUTOBOOST_VERSION / 100 % 1000) < 44)
#define AUTOBOOST_CHRONO_SYSTEM_CATEGORY autoboost::system::system_category
#else
#define AUTOBOOST_CHRONO_SYSTEM_CATEGORY autoboost::system::system_category()
#endif

#ifdef AUTOBOOST_SYSTEM_NO_DEPRECATED
#define AUTOBOOST_CHRONO_THROWS autoboost::throws()
#define AUTOBOOST_CHRONO_IS_THROWS(EC) (&EC==&autoboost::throws())
#else
#define AUTOBOOST_CHRONO_THROWS autoboost::system::throws
#define AUTOBOOST_CHRONO_IS_THROWS(EC) (&EC==&autoboost::system::throws)
#endif

#endif
#endif

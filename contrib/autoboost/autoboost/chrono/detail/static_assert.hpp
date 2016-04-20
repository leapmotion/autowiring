//  static_assert.hpp  --------------------------------------------------------------//

//  Copyright 2009-2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_CHRONO_DETAIL_STATIC_ASSERT_HPP
#define AUTOBOOST_CHRONO_DETAIL_STATIC_ASSERT_HPP

#include <autoboost/chrono/config.hpp>

#ifndef AUTOBOOST_NO_CXX11_STATIC_ASSERT
#define AUTOBOOST_CHRONO_STATIC_ASSERT(CND, MSG, TYPES) static_assert(CND,MSG)
#elif defined(AUTOBOOST_CHRONO_USES_STATIC_ASSERT)
#include <autoboost/static_assert.hpp>
#define AUTOBOOST_CHRONO_STATIC_ASSERT(CND, MSG, TYPES) AUTOBOOST_STATIC_ASSERT(CND)
#elif defined(AUTOBOOST_CHRONO_USES_MPL_ASSERT)
#include <autoboost/mpl/assert.hpp>
#include <autoboost/mpl/bool.hpp>
#define AUTOBOOST_CHRONO_STATIC_ASSERT(CND, MSG, TYPES)                                 \
    AUTOBOOST_MPL_ASSERT_MSG(autoboost::mpl::bool_< (CND) >::type::value, MSG, TYPES)
#else
//~ #elif defined(AUTOBOOST_CHRONO_USES_ARRAY_ASSERT)
#define AUTOBOOST_CHRONO_STATIC_ASSERT(CND, MSG, TYPES) static char AUTOBOOST_JOIN(autoboost_chrono_test_,__LINE__)[(CND)?1:-1]
//~ #define AUTOBOOST_CHRONO_STATIC_ASSERT(CND, MSG, TYPES)
#endif

#endif // AUTOBOOST_CHRONO_DETAIL_STATIC_ASSERT_HPP

//  Boost string_algo library config.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef AUTOBOOST_STRING_CONFIG_HPP
#define AUTOBOOST_STRING_CONFIG_HPP

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

#ifdef AUTOBOOST_STRING_DEDUCED_TYPENAME
#   error "macro already defined!"
#endif

#define AUTOBOOST_STRING_TYPENAME AUTOBOOST_DEDUCED_TYPENAME

// Metrowerks workaround
#if AUTOBOOST_WORKAROUND(__MWERKS__, <= 0x3003) // 8.x
#pragma parse_func_templ off
#endif

#endif  // AUTOBOOST_STRING_CONFIG_HPP

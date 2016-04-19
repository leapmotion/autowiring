// ----------------------------------------------------------------------------
// format.hpp :  primary header
// ----------------------------------------------------------------------------

//  Copyright Samuel Krempp 2003. Use, modification, and distribution are
//  subject to the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/format for library home page


// ----------------------------------------------------------------------------

#ifndef AUTOBOOST_FORMAT_HPP
#define AUTOBOOST_FORMAT_HPP

#include <vector>
#include <string>
#include <autoboost/detail/workaround.hpp>
#include <autoboost/config.hpp>

#ifndef AUTOBOOST_NO_STD_LOCALE
#include <locale>
#endif

// ***   Compatibility framework
#include <autoboost/format/detail/compat_workarounds.hpp>

#ifdef AUTOBOOST_NO_LOCALE_ISIDIGIT
#include <cctype>  // we'll use the non-locale  <cctype>'s std::isdigit(int)
#endif

// ****  Forward declarations ----------------------------------
#include <autoboost/format/format_fwd.hpp>     // basic_format<Ch,Tr>, and other frontends
#include <autoboost/format/internals_fwd.hpp>  // misc forward declarations for internal use

// ****  Auxiliary structs (stream_format_state<Ch,Tr> , and format_item<Ch,Tr> )
#include <autoboost/format/internals.hpp>

// ****  Format  class  interface --------------------------------
#include <autoboost/format/format_class.hpp>

// **** Exceptions -----------------------------------------------
#include <autoboost/format/exceptions.hpp>

// **** Implementation -------------------------------------------
#include <autoboost/format/format_implementation.hpp>   // member functions
#include <autoboost/format/group.hpp>                   // class for grouping arguments
#include <autoboost/format/feed_args.hpp>               // argument-feeding functions
#include <autoboost/format/parsing.hpp>                 // format-string parsing (member-)functions

// **** Implementation of the free functions ----------------------
#include <autoboost/format/free_funcs.hpp>


// *** Undefine 'local' macros :
#include <autoboost/format/detail/unset_macros.hpp>

#endif // AUTOBOOST_FORMAT_HPP

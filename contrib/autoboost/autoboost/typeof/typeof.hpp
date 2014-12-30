// Copyright (C) 2004 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_TYPEOF_HPP_INCLUDED
#define AUTOBOOST_TYPEOF_TYPEOF_HPP_INCLUDED

#if defined(AUTOBOOST_TYPEOF_COMPLIANT)
#   define AUTOBOOST_TYPEOF_EMULATION
#endif

#if defined(AUTOBOOST_TYPEOF_EMULATION) && defined(AUTOBOOST_TYPEOF_NATIVE)
#   error both typeof emulation and native mode requested
#endif

#if defined(__COMO__)
#   ifdef __GNUG__
#       ifndef AUTOBOOST_TYPEOF_EMULATION
#           ifndef AUTOBOOST_TYPEOF_NATIVE
#               define AUTOBOOST_TYPEOF_NATIVE
#           endif
#           define AUTOBOOST_TYPEOF_KEYWORD typeof
#       endif
#   else
#       ifndef AUTOBOOST_TYPEOF_NATIVE
#           ifndef AUTOBOOST_TYPEOF_EMULATION
#               define AUTOBOOST_TYPEOF_EMULATION
#           endif
#       else
#           error native typeof is not supported
#       endif
#   endif

#elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)
#   ifdef __GNUC__
#       ifndef AUTOBOOST_TYPEOF_EMULATION
#           ifndef AUTOBOOST_TYPEOF_NATIVE
#               define AUTOBOOST_TYPEOF_NATIVE
#           endif
#           define AUTOBOOST_TYPEOF_KEYWORD __typeof__
#       endif
#   else
#       ifndef AUTOBOOST_TYPEOF_NATIVE
#           ifndef AUTOBOOST_TYPEOF_EMULATION
#               define AUTOBOOST_TYPEOF_EMULATION
#           endif
#       else
#           error native typeof is not supported
#       endif
#   endif

#elif defined(__GNUC__)
#   ifndef AUTOBOOST_TYPEOF_EMULATION
#       ifndef AUTOBOOST_TYPEOF_NATIVE
#           define AUTOBOOST_TYPEOF_NATIVE
#       endif
#       define AUTOBOOST_TYPEOF_KEYWORD __typeof__
#   endif

#elif defined(__MWERKS__)
#   if(__MWERKS__ <= 0x3003)  // 8.x
#       ifndef AUTOBOOST_TYPEOF_EMULATION
#           ifndef AUTOBOOST_TYPEOF_NATIVE
#               define AUTOBOOST_TYPEOF_NATIVE
#           endif
#           define AUTOBOOST_TYPEOF_KEYWORD __typeof__
#       else
#           define AUTOBOOST_TYPEOF_EMULATION_UNSUPPORTED
#       endif
#   else // 9.x
#       ifndef AUTOBOOST_TYPEOF_EMULATION
#           ifndef AUTOBOOST_TYPEOF_NATIVE
#               define AUTOBOOST_TYPEOF_NATIVE
#           endif
#           define AUTOBOOST_TYPEOF_KEYWORD __typeof__
#       endif
#   endif
#elif defined __CODEGEARC__
#   ifndef AUTOBOOST_TYPEOF_EMULATION
#       ifndef AUTOBOOST_TYPEOF_NATIVE
#           define AUTOBOOST_TYPEOF_EMULATION_UNSUPPORTED
#       endif
#   else
#       define AUTOBOOST_TYPEOF_EMULATION_UNSUPPORTED
#   endif
#elif defined __BORLANDC__
#   ifndef AUTOBOOST_TYPEOF_EMULATION
#       ifndef AUTOBOOST_TYPEOF_NATIVE
#           define AUTOBOOST_TYPEOF_EMULATION_UNSUPPORTED
#       endif
#   else
#       define AUTOBOOST_TYPEOF_EMULATION_UNSUPPORTED
#   endif
#elif defined __DMC__
#   ifndef AUTOBOOST_TYPEOF_EMULATION
#       ifndef AUTOBOOST_TYPEOF_NATIVE
#           define AUTOBOOST_TYPEOF_NATIVE
#       endif
#       include <autoboost/typeof/dmc/typeof_impl.hpp>
#       define MSVC_TYPEOF_HACK
#   endif
#elif defined(_MSC_VER)
#   if (_MSC_VER <= 1300)  // 6.5, 7.0
#       ifndef AUTOBOOST_TYPEOF_EMULATION
#           ifndef AUTOBOOST_TYPEOF_NATIVE
#               define AUTOBOOST_TYPEOF_NATIVE
#           endif
#           include <autoboost/typeof/msvc/typeof_impl.hpp>
#           define MSVC_TYPEOF_HACK
#       else
#           error typeof emulation is not supported
#       endif
#   elif (_MSC_VER >= 1310)  // 7.1 ->
#       ifndef AUTOBOOST_TYPEOF_EMULATION
#           ifndef AUTOBOOST_TYPEOF_NATIVE
#               ifndef _MSC_EXTENSIONS
#                   define AUTOBOOST_TYPEOF_EMULATION
#               else
#                   define AUTOBOOST_TYPEOF_NATIVE
#               endif
#           endif
#       endif
#       ifdef AUTOBOOST_TYPEOF_NATIVE
#           include <autoboost/typeof/msvc/typeof_impl.hpp>
#           define MSVC_TYPEOF_HACK
#       endif
#   endif
#elif defined(__HP_aCC)
#   ifndef AUTOBOOST_TYPEOF_NATIVE
#       ifndef AUTOBOOST_TYPEOF_EMULATION
#           define AUTOBOOST_TYPEOF_EMULATION
#       endif
#   else
#       error native typeof is not supported
#   endif

#elif defined(__DECCXX)
#   ifndef AUTOBOOST_TYPEOF_NATIVE
#       ifndef AUTOBOOST_TYPEOF_EMULATION
#           define AUTOBOOST_TYPEOF_EMULATION
#       endif
#   else
#       error native typeof is not supported
#   endif

#elif defined(__BORLANDC__)
#   if (__BORLANDC__ < 0x590)
#       define AUTOBOOST_TYPEOF_NO_FUNCTION_TYPES
#       define AUTOBOOST_TYPEOF_NO_MEMBER_FUNCTION_TYPES
#   endif
#   ifndef AUTOBOOST_TYPEOF_NATIVE
#       ifndef AUTOBOOST_TYPEOF_EMULATION
#           define AUTOBOOST_TYPEOF_EMULATION
#       endif
#   else
#       error native typeof is not supported
#   endif
#elif defined(__SUNPRO_CC)
#   if (__SUNPRO_CC < 0x590 )
#     ifdef AUTOBOOST_TYPEOF_NATIVE
#         error native typeof is not supported
#     endif
#     ifndef AUTOBOOST_TYPEOF_EMULATION
#         define AUTOBOOST_TYPEOF_EMULATION
#     endif
#   else
#     ifndef AUTOBOOST_TYPEOF_EMULATION
#         ifndef AUTOBOOST_TYPEOF_NATIVE
#             define AUTOBOOST_TYPEOF_NATIVE
#         endif
#         define AUTOBOOST_TYPEOF_KEYWORD __typeof__
#     endif
#   endif
#else //unknown compiler
#   ifndef AUTOBOOST_TYPEOF_NATIVE
#       ifndef AUTOBOOST_TYPEOF_EMULATION
#           define AUTOBOOST_TYPEOF_EMULATION
#       endif
#   else
#       ifndef AUTOBOOST_TYPEOF_KEYWORD
#           define AUTOBOOST_TYPEOF_KEYWORD typeof
#       endif
#   endif

#endif

#define AUTOBOOST_TYPEOF_UNIQUE_ID()\
     AUTOBOOST_TYPEOF_REGISTRATION_GROUP * 0x10000 + __LINE__

#define AUTOBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()\
     <autoboost/typeof/incr_registration_group.hpp>

#ifdef AUTOBOOST_TYPEOF_EMULATION_UNSUPPORTED
#   include <autoboost/typeof/unsupported.hpp>
#elif defined AUTOBOOST_TYPEOF_EMULATION
#   define AUTOBOOST_TYPEOF_TEXT "using typeof emulation"
#   include <autoboost/typeof/message.hpp>
#   include <autoboost/typeof/typeof_impl.hpp>
#   include <autoboost/typeof/type_encoding.hpp>
#   include <autoboost/typeof/template_encoding.hpp>
#   include <autoboost/typeof/modifiers.hpp>
#   include <autoboost/typeof/pointers_data_members.hpp>
#   include <autoboost/typeof/register_functions.hpp>
#   include <autoboost/typeof/register_fundamental.hpp>

#elif defined(AUTOBOOST_TYPEOF_NATIVE)
#   define AUTOBOOST_TYPEOF_TEXT "using native typeof"
#   include <autoboost/typeof/message.hpp>
#   include <autoboost/typeof/native.hpp>
#else
#   error typeof configuration error
#endif

// auto
#define AUTOBOOST_AUTO(Var, Expr) AUTOBOOST_TYPEOF(Expr) Var = Expr
#define AUTOBOOST_AUTO_TPL(Var, Expr) AUTOBOOST_TYPEOF_TPL(Expr) Var = Expr

#endif//AUTOBOOST_TYPEOF_TYPEOF_HPP_INCLUDED


//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <autoboost/config.hpp>

#if defined(AUTOBOOST_WINDOWS)
# include <autoboost/context/windows/fixedsize_stack.hpp>
#else
# include <autoboost/context/posix/fixedsize_stack.hpp>
#endif

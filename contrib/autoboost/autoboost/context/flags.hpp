
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CONTEXT_FLAGS_H
#define AUTOBOOST_CONTEXT_FLAGS_H

# include <autoboost/config.hpp>

# ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_PREFIX
# endif

namespace autoboost {
namespace context {

struct exec_ontop_arg_t {};
const exec_ontop_arg_t exec_ontop_arg{};

}}

# ifdef AUTOBOOST_HAS_ABI_HEADERS
# include AUTOBOOST_ABI_SUFFIX
# endif

#endif // AUTOBOOST_CONTEXT_FLAGS_H

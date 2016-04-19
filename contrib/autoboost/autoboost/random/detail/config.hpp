/* autoboost random/detail/config.hpp header file
 *
 * Copyright Steven Watanabe 2009
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id$
 */

#include <autoboost/config.hpp>

#if (defined(AUTOBOOST_NO_OPERATORS_IN_NAMESPACE) || defined(AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS)) \
    && !defined(AUTOBOOST_MSVC)
    #define AUTOBOOST_RANDOM_NO_STREAM_OPERATORS
#endif

// Copyright (c) 2001 Ronald Garcia, Indiana University (garcia@osl.iu.edu)
// Andrew Lumsdaine, Indiana University (lums@osl.iu.edu).

// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_FILESYSTEM_UTF8_CODECVT_FACET_HPP
#define AUTOBOOST_FILESYSTEM_UTF8_CODECVT_FACET_HPP

#include <autoboost/filesystem/config.hpp>

#define AUTOBOOST_UTF8_BEGIN_NAMESPACE \
     namespace autoboost { namespace filesystem { namespace detail {

#define AUTOBOOST_UTF8_END_NAMESPACE }}}
#define AUTOBOOST_UTF8_DECL AUTOBOOST_FILESYSTEM_DECL

#include <autoboost/detail/utf8_codecvt_facet.hpp>

#undef AUTOBOOST_UTF8_BEGIN_NAMESPACE
#undef AUTOBOOST_UTF8_END_NAMESPACE
#undef AUTOBOOST_UTF8_DECL

#endif

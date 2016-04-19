// Copyright Vladimir Prus 2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For HP-UX, request that WCHAR_MAX and WCHAR_MIN be defined as macros,
// not casts. See ticket 5048
#define _INCLUDE_STDCSOURCE_199901

#ifndef AUTOBOOST_SYSTEM_NO_DEPRECATED
# define AUTOBOOST_SYSTEM_NO_DEPRECATED
#endif

#define AUTOBOOST_FILESYSTEM_SOURCE
#include <autoboost/filesystem/config.hpp>

#define AUTOBOOST_UTF8_BEGIN_NAMESPACE \
     namespace autoboost { namespace filesystem { namespace detail {

#define AUTOBOOST_UTF8_END_NAMESPACE }}}
#define AUTOBOOST_UTF8_DECL AUTOBOOST_FILESYSTEM_DECL

#include <autoboost/detail/utf8_codecvt_facet.ipp>

#undef AUTOBOOST_UTF8_BEGIN_NAMESPACE
#undef AUTOBOOST_UTF8_END_NAMESPACE
#undef AUTOBOOST_UTF8_DECL

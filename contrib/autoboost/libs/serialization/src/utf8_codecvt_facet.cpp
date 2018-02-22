// Copyright Vladimir Prus 2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <autoboost/config.hpp>

#ifdef AUTOBOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#endif

// std::codecvt_utf8 doesn't seem to work for any versions of msvc
#if defined(_MSC_VER) \
||  defined( AUTOBOOST_NO_CXX11_HDR_CODECVT )
    // include autoboost implementation of utf8 codecvt facet
    # define AUTOBOOST_ARCHIVE_SOURCE
    #include <autoboost/archive/detail/decl.hpp>
    #define AUTOBOOST_UTF8_BEGIN_NAMESPACE \
         namespace autoboost { namespace archive { namespace detail {
    #define AUTOBOOST_UTF8_DECL AUTOBOOST_ARCHIVE_DECL
    #define AUTOBOOST_UTF8_END_NAMESPACE }}}
    #include <autoboost/detail/utf8_codecvt_facet.ipp>
    #undef AUTOBOOST_UTF8_END_NAMESPACE
    #undef AUTOBOOST_UTF8_DECL
    #undef AUTOBOOST_UTF8_BEGIN_NAMESPACE
#endif

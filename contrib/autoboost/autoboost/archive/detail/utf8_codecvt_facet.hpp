// Copyright (c) 2001 Ronald Garcia, Indiana University (garcia@osl.iu.edu)
// Andrew Lumsdaine, Indiana University (lums@osl.iu.edu).
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP
#define AUTOBOOST_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP

#include <autoboost/config.hpp>

#ifdef AUTOBOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#endif

// std::codecvt_utf8 doesn't seem to work for any versions of msvc

#if defined(_MSC_VER) || defined(AUTOBOOST_NO_CXX11_HDR_CODECVT)
    // use autoboost's utf8 codecvt facet
    #include <autoboost/archive/detail/decl.hpp>
    #define AUTOBOOST_UTF8_BEGIN_NAMESPACE \
         namespace autoboost { namespace archive { namespace detail {
    #define AUTOBOOST_UTF8_DECL AUTOBOOST_ARCHIVE_DECL
    #define AUTOBOOST_UTF8_END_NAMESPACE }}}

    #include <autoboost/detail/utf8_codecvt_facet.hpp>

    #undef AUTOBOOST_UTF8_END_NAMESPACE
    #undef AUTOBOOST_UTF8_DECL
    #undef AUTOBOOST_UTF8_BEGIN_NAMESPACE
#else
    // use the standard vendor supplied facet
    #include <codecvt>
    namespace autoboost { namespace archive { namespace detail {
        typedef std::codecvt_utf8<wchar_t> utf8_codecvt_facet;
    } } }
#endif

#endif // AUTOBOOST_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP

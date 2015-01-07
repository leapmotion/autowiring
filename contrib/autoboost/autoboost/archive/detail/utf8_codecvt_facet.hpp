// Copyright (c) 2001 Ronald Garcia, Indiana University (garcia@osl.iu.edu)
// Andrew Lumsdaine, Indiana University (lums@osl.iu.edu).
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP
#define AUTOBOOST_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP

#ifdef AUTOBOOST_NO_CXX11_HDR_CODECVT
    #define AUTOBOOST_UTF8_BEGIN_NAMESPACE \
         namespace autoboost { namespace archive { namespace detail {
    #define AUTOBOOST_UTF8_DECL
    #define AUTOBOOST_UTF8_END_NAMESPACE }}}

    #include <autoboost/detail/utf8_codecvt_facet.hpp>

    #undef AUTOBOOST_UTF8_END_NAMESPACE
    #undef AUTOBOOST_UTF8_DECL
    #undef AUTOBOOST_UTF8_BEGIN_NAMESPACE
#endif // AUTOBOOST_NO_CXX11_HDR_CODECVT
#endif // AUTOBOOST_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP


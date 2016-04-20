#ifndef AUTOBOOST_ARCHIVE_BASIC_TEXT_IPRIMITIVE_HPP
#define AUTOBOOST_ARCHIVE_BASIC_TEXT_IPRIMITIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_text_iprimitive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// archives stored as text - note these are templated on the basic
// stream templates to accommodate wide (and other?) kind of characters
//
// Note the fact that on libraries without wide characters, ostream is
// not a specialization of basic_ostream which in fact is not defined
// in such cases.   So we can't use basic_ostream<IStream::char_type> but rather
// use two template parameters

#include <autoboost/assert.hpp>
#include <locale>
#include <cstddef> // size_t

#include <autoboost/config.hpp>
#if defined(AUTOBOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
    #if ! defined(AUTOBOOST_DINKUMWARE_STDLIB) && ! defined(__SGI_STL_PORT)
        using ::locale;
    #endif
} // namespace std
#endif

#include <autoboost/detail/workaround.hpp>
#if AUTOBOOST_WORKAROUND(AUTOBOOST_DINKUMWARE_STDLIB, == 1)
#include <autoboost/archive/dinkumware.hpp>
#endif

#include <autoboost/limits.hpp>
#include <autoboost/io/ios_state.hpp>
#include <autoboost/scoped_ptr.hpp>
#include <autoboost/static_assert.hpp>

#include <autoboost/serialization/throw_exception.hpp>
#include <autoboost/archive/archive_exception.hpp>
#include <autoboost/archive/basic_streambuf_locale_saver.hpp>
#include <autoboost/archive/detail/abi_prefix.hpp> // must be the last header

namespace autoboost {
namespace archive {

/////////////////////////////////////////////////////////////////////////
// class basic_text_iarchive - load serialized objects from a input text stream
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4244 4267 )
#endif

template<class IStream>
class basic_text_iprimitive {
protected:
    IStream &is;
    io::ios_flags_saver flags_saver;
    io::ios_precision_saver precision_saver;

    #ifndef AUTOBOOST_NO_STD_LOCALE
    autoboost::scoped_ptr<std::locale> archive_locale;
    basic_streambuf_locale_saver<
        typename IStream::char_type,
        typename IStream::traits_type
    > locale_saver;
    #endif

    template<class T>
    void load(T & t)
    {
        if(is >> t)
            return;
        autoboost::serialization::throw_exception(
            archive_exception(archive_exception::input_stream_error)
        );
    }

    void load(char & t)
    {
        short int i;
        load(i);
        t = i;
    }
    void load(signed char & t)
    {
        short int i;
        load(i);
        t = i;
    }
    void load(unsigned char & t)
    {
        unsigned short int i;
        load(i);
        t = i;
    }

    #ifndef AUTOBOOST_NO_INTRINSIC_WCHAR_T
    void load(wchar_t & t)
    {
        AUTOBOOST_STATIC_ASSERT(sizeof(wchar_t) <= sizeof(int));
        int i;
        load(i);
        t = i;
    }
    #endif
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(AUTOBOOST_PP_EMPTY())
    basic_text_iprimitive(IStream  &is, bool no_codecvt);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(AUTOBOOST_PP_EMPTY())
    ~basic_text_iprimitive();
public:
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(void)
    load_binary(void *address, std::size_t count);
};

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

} // namespace archive
} // namespace autoboost

#include <autoboost/archive/detail/abi_suffix.hpp> // pop pragmas

#endif // AUTOBOOST_ARCHIVE_BASIC_TEXT_IPRIMITIVE_HPP

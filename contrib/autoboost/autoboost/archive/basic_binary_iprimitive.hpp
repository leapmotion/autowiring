#ifndef AUTOBOOST_ARCHIVE_BINARY_IPRIMITIVE_HPP
#define AUTOBOOST_ARCHIVE_BINARY_IPRIMITIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

#if defined(_MSC_VER)
#pragma warning( disable : 4800 )
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_binary_iprimitive.hpp
//
// archives stored as native binary - this should be the fastest way
// to archive the state of a group of obects.  It makes no attempt to
// convert to any canonical form.

// IN GENERAL, ARCHIVES CREATED WITH THIS CLASS WILL NOT BE READABLE
// ON PLATFORM APART FROM THE ONE THEY ARE CREATED ON

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <iosfwd>
#include <autoboost/assert.hpp>
#include <locale>
#include <cstring> // std::memcpy
#include <cstddef> // std::size_t
#include <streambuf> // basic_streambuf
#include <string>

#include <autoboost/config.hpp>
#if defined(AUTOBOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::memcpy;
    using ::size_t;
} // namespace std
#endif

#include <autoboost/cstdint.hpp>
#include <autoboost/scoped_ptr.hpp>
#include <autoboost/serialization/throw_exception.hpp>
#include <autoboost/integer.hpp>
#include <autoboost/integer_traits.hpp>

#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/serialization/is_bitwise_serializable.hpp>
#include <autoboost/serialization/array.hpp>

#include <autoboost/archive/basic_streambuf_locale_saver.hpp>
#include <autoboost/archive/archive_exception.hpp>
#include <autoboost/archive/detail/auto_link_archive.hpp>
#include <autoboost/archive/detail/abi_prefix.hpp> // must be the last header

namespace autoboost {
namespace archive {

template<class Ch>
class codecvt_null;

/////////////////////////////////////////////////////////////////////////////
// class binary_iarchive - read serialized objects from a input binary stream
template<class Archive, class Elem, class Tr>
class AUTOBOOST_SYMBOL_VISIBLE basic_binary_iprimitive {
#ifndef AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
    friend class load_access;
protected:
#else
public:
#endif
    std::basic_streambuf<Elem, Tr> & m_sb;
    // return a pointer to the most derived class
    Archive * This(){
        return static_cast<Archive *>(this);
    }

    #ifndef AUTOBOOST_NO_STD_LOCALE
    autoboost::scoped_ptr<codecvt_null<Elem> > codecvt_facet;
    autoboost::scoped_ptr<std::locale> archive_locale;
    basic_streambuf_locale_saver<Elem, Tr> locale_saver;
    #endif

    // main template for serilization of primitive types
    template<class T>
    void load(T & t){
        load_binary(& t, sizeof(T));
    }

    /////////////////////////////////////////////////////////
    // fundamental types that need special treatment

    // trap usage of invalid uninitialized boolean
    void load(bool & t){
        load_binary(& t, sizeof(t));
        int i = t;
        AUTOBOOST_ASSERT(0 == i || 1 == i);
        (void)i; // warning suppression for release builds.
    }
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load(std::string &s);
    #ifndef AUTOBOOST_NO_STD_WSTRING
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load(std::wstring &ws);
    #endif
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load(char * t);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    load(wchar_t * t);

    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    init();
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL
    basic_binary_iprimitive(
        std::basic_streambuf<Elem, Tr> & sb,
        bool no_codecvt
    );
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL
    ~basic_binary_iprimitive();
public:
    // we provide an optimized load for all fundamental types
    // typedef serialization::is_bitwise_serializable<mpl::_1>
    // use_array_optimization;
    struct use_array_optimization {
        template <class T>
        #if defined(AUTOBOOST_NO_DEPENDENT_NESTED_DERIVATIONS)
            struct apply {
                typedef typename autoboost::serialization::is_bitwise_serializable< T >::type type;
            };
        #else
            struct apply : public autoboost::serialization::is_bitwise_serializable< T > {};
        #endif
    };

    // the optimized load_array dispatches to load_binary
    template <class ValueType>
    void load_array(serialization::array<ValueType>& a, unsigned int)
    {
      load_binary(a.address(),a.count()*sizeof(ValueType));
    }

    void
    load_binary(void *address, std::size_t count);
};

template<class Archive, class Elem, class Tr>
inline void
basic_binary_iprimitive<Archive, Elem, Tr>::load_binary(
    void *address,
    std::size_t count
){
    // note: an optimizer should eliminate the following for char files
    AUTOBOOST_ASSERT(
        static_cast<std::streamsize>(count / sizeof(Elem))
        <= autoboost::integer_traits<std::streamsize>::const_max
    );
    std::streamsize s = static_cast<std::streamsize>(count / sizeof(Elem));
    std::streamsize scount = m_sb.sgetn(
        static_cast<Elem *>(address),
        s
    );
    if(scount != s)
        autoboost::serialization::throw_exception(
            archive_exception(archive_exception::input_stream_error)
        );
    // note: an optimizer should eliminate the following for char files
    AUTOBOOST_ASSERT(count % sizeof(Elem) <= autoboost::integer_traits<std::streamsize>::const_max);
    s = static_cast<std::streamsize>(count % sizeof(Elem));
    if(0 < s){
//        if(is.fail())
//            autoboost::serialization::throw_exception(
//                archive_exception(archive_exception::stream_error)
//        );
        Elem t;
        scount = m_sb.sgetn(& t, 1);
        if(scount != 1)
            autoboost::serialization::throw_exception(
                archive_exception(archive_exception::input_stream_error)
            );
        std::memcpy(static_cast<char*>(address) + (count - s), &t, static_cast<std::size_t>(s));
    }
}

} // namespace archive
} // namespace autoboost

#include <autoboost/archive/detail/abi_suffix.hpp> // pop pragmas

#endif // AUTOBOOST_ARCHIVE_BINARY_IPRIMITIVE_HPP

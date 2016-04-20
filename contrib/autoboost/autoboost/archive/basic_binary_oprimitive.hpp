#ifndef AUTOBOOST_ARCHIVE_BASIC_BINARY_OPRIMITIVE_HPP
#define AUTOBOOST_ARCHIVE_BASIC_BINARY_OPRIMITIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_binary_oprimitive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// archives stored as native binary - this should be the fastest way
// to archive the state of a group of obects.  It makes no attempt to
// convert to any canonical form.

// IN GENERAL, ARCHIVES CREATED WITH THIS CLASS WILL NOT BE READABLE
// ON PLATFORM APART FROM THE ONE THEY ARE CREATE ON

#include <iosfwd>
#include <autoboost/assert.hpp>
#include <locale>
#include <streambuf> // basic_streambuf
#include <string>
#include <cstddef> // size_t

#include <autoboost/config.hpp>
#if defined(AUTOBOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif

#include <autoboost/cstdint.hpp>
#include <autoboost/integer.hpp>
#include <autoboost/integer_traits.hpp>
#include <autoboost/scoped_ptr.hpp>
#include <autoboost/serialization/throw_exception.hpp>

#include <autoboost/archive/basic_streambuf_locale_saver.hpp>
#include <autoboost/archive/archive_exception.hpp>
#include <autoboost/serialization/is_bitwise_serializable.hpp>
#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/serialization/array.hpp>
#include <autoboost/archive/detail/auto_link_archive.hpp>
#include <autoboost/archive/detail/abi_prefix.hpp> // must be the last header

namespace autoboost {
namespace archive {

/////////////////////////////////////////////////////////////////////////
// class basic_binary_oprimitive - binary output of prmitives

template<class Archive, class Elem, class Tr>
class basic_binary_oprimitive {
#ifndef AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
    friend class save_access;
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
    autoboost::scoped_ptr<std::locale> archive_locale;
    basic_streambuf_locale_saver<Elem, Tr> locale_saver;
    #endif
    // default saving of primitives.
    template<class T>
    void save(const T & t)
    {
        save_binary(& t, sizeof(T));
    }

    /////////////////////////////////////////////////////////
    // fundamental types that need special treatment

    // trap usage of invalid uninitialized boolean which would
    // otherwise crash on load.
    void save(const bool t){
        AUTOBOOST_ASSERT(0 == static_cast<int>(t) || 1 == static_cast<int>(t));
        save_binary(& t, sizeof(t));
    }
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(void)
    save(const std::string &s);
    #ifndef AUTOBOOST_NO_STD_WSTRING
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(void)
    save(const std::wstring &ws);
    #endif
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(void)
    save(const char * t);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(void)
    save(const wchar_t * t);

    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(void)
    init();

    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(AUTOBOOST_PP_EMPTY())
    basic_binary_oprimitive(
        std::basic_streambuf<Elem, Tr> & sb,
        bool no_codecvt
    );
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL(AUTOBOOST_PP_EMPTY())
    ~basic_binary_oprimitive();
public:

    // we provide an optimized save for all fundamental types
    // typedef serialization::is_bitwise_serializable<mpl::_1>
    // use_array_optimization;
    // workaround without using mpl lambdas
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


    // the optimized save_array dispatches to save_binary
    template <class ValueType>
    void save_array(autoboost::serialization::array<ValueType> const& a, unsigned int)
    {
      save_binary(a.address(),a.count()*sizeof(ValueType));
    }

    void save_binary(const void *address, std::size_t count);
};

template<class Archive, class Elem, class Tr>
inline void
basic_binary_oprimitive<Archive, Elem, Tr>::save_binary(
    const void *address,
    std::size_t count
){
    //AUTOBOOST_ASSERT(
    //    static_cast<std::size_t>((std::numeric_limits<std::streamsize>::max)()) >= count
    //);
    // note: if the following assertions fail
    // a likely cause is that the output stream is set to "text"
    // mode where by cr characters recieve special treatment.
    // be sure that the output stream is opened with ios::binary
    //if(os.fail())
    //    autoboost::serialization::throw_exception(
    //        archive_exception(archive_exception::output_stream_error)
    //    );
    // figure number of elements to output - round up
    count = ( count + sizeof(Elem) - 1)
        / sizeof(Elem);
    AUTOBOOST_ASSERT(count <= std::size_t(autoboost::integer_traits<std::streamsize>::const_max));
    std::streamsize scount = m_sb.sputn(
        static_cast<const Elem *>(address),
        static_cast<std::streamsize>(count)
    );
    if(count != static_cast<std::size_t>(scount))
        autoboost::serialization::throw_exception(
            archive_exception(archive_exception::output_stream_error)
        );
    //os.write(
    //    static_cast<const typename OStream::char_type *>(address),
    //    count
    //);
    //AUTOBOOST_ASSERT(os.good());
}

} //namespace autoboost
} //namespace archive

#include <autoboost/archive/detail/abi_suffix.hpp> // pop pragmas

#endif // AUTOBOOST_ARCHIVE_BASIC_BINARY_OPRIMITIVE_HPP

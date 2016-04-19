#ifndef AUTOBOOST_ARCHIVE_XML_WOARCHIVE_HPP
#define AUTOBOOST_ARCHIVE_XML_WOARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// xml_woarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <autoboost/config.hpp>
#ifdef AUTOBOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else

#include <cstddef> // size_t
#if defined(AUTOBOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif

#include <ostream>

#include <autoboost/smart_ptr/scoped_ptr.hpp>
#include <autoboost/archive/detail/auto_link_warchive.hpp>
#include <autoboost/archive/basic_text_oprimitive.hpp>
#include <autoboost/archive/basic_xml_oarchive.hpp>
#include <autoboost/archive/detail/register_archive.hpp>
#include <autoboost/serialization/item_version_type.hpp>

#ifdef AUTOBOOST_NO_CXX11_HDR_CODECVT
    #include <autoboost/archive/detail/utf8_codecvt_facet.hpp>
#else
    #include <codecvt>
    namespace autoboost { namespace archive { namespace detail {
        typedef std::codecvt_utf8<wchar_t> utf8_codecvt_facet;
    } } }
#endif

#include <autoboost/archive/detail/abi_prefix.hpp> // must be the last header

#ifdef AUTOBOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace autoboost {
namespace archive {

namespace detail {
    template<class Archive> class interface_oarchive;
} // namespace detail

template<class Archive>
class AUTOBOOST_SYMBOL_VISIBLE xml_woarchive_impl :
    public basic_text_oprimitive<std::wostream>,
    public basic_xml_oarchive<Archive>
{
#ifdef AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
    #if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1500)
        // for some inexplicable reason insertion of "class" generates compile erro
        // on msvc 7.1
        friend detail::interface_oarchive<Archive>;
        friend basic_xml_oarchive<Archive>;
        friend save_access;
    #else
        friend class detail::interface_oarchive<Archive>;
        friend class basic_xml_oarchive<Archive>;
        friend class save_access;
    #endif
#endif
    //void end_preamble(){
    //    basic_xml_oarchive<Archive>::end_preamble();
    //}
    template<class T>
    void
    save(const T & t){
        basic_text_oprimitive<std::wostream>::save(t);
    }
    void
    save(const version_type & t){
        save(static_cast<const unsigned int>(t));
    }
    void
    save(const autoboost::serialization::item_version_type & t){
        save(static_cast<const unsigned int>(t));
    }
    AUTOBOOST_WARCHIVE_DECL void
    save(const char * t);
    #ifndef AUTOBOOST_NO_INTRINSIC_WCHAR_T
    AUTOBOOST_WARCHIVE_DECL void
    save(const wchar_t * t);
    #endif
    AUTOBOOST_WARCHIVE_DECL void
    save(const std::string &s);
    #ifndef AUTOBOOST_NO_STD_WSTRING
    AUTOBOOST_WARCHIVE_DECL void
    save(const std::wstring &ws);
    #endif
    AUTOBOOST_WARCHIVE_DECL
    xml_woarchive_impl(std::wostream & os, unsigned int flags);
    AUTOBOOST_WARCHIVE_DECL
    ~xml_woarchive_impl();
public:
    void
    save_binary(const void *address, std::size_t count){
        this->end_preamble();
        #if ! defined(__MWERKS__)
        this->basic_text_oprimitive<std::wostream>::save_binary(
        #else
        this->basic_text_oprimitive::save_binary(
        #endif
            address,
            count
        );
        this->indent_next = true;
    }
};

// we use the following because we can't use
// typedef xml_woarchive_impl<xml_woarchive_impl<...> > xml_woarchive;

// do not derive from this class.  If you want to extend this functionality
// via inhertance, derived from xml_woarchive_impl instead.  This will
// preserve correct static polymorphism.
class AUTOBOOST_SYMBOL_VISIBLE xml_woarchive :
    public xml_woarchive_impl<xml_woarchive>
{
public:
    xml_woarchive(std::wostream & os, unsigned int flags = 0) :
        xml_woarchive_impl<xml_woarchive>(os, flags)
    {}
    ~xml_woarchive(){}
};

} // namespace archive
} // namespace autoboost

// required by export
AUTOBOOST_SERIALIZATION_REGISTER_ARCHIVE(autoboost::archive::xml_woarchive)

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

#include <autoboost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // AUTOBOOST_NO_STD_WSTREAMBUF
#endif // AUTOBOOST_ARCHIVE_XML_OARCHIVE_HPP

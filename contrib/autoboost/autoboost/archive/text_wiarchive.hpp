#ifndef AUTOBOOST_ARCHIVE_TEXT_WIARCHIVE_HPP
#define AUTOBOOST_ARCHIVE_TEXT_WIARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// text_wiarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <autoboost/config.hpp>
#ifdef AUTOBOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else

#include <istream>

#include <autoboost/archive/detail/auto_link_warchive.hpp>
#include <autoboost/archive/basic_text_iprimitive.hpp>
#include <autoboost/archive/basic_text_iarchive.hpp>
#include <autoboost/archive/detail/register_archive.hpp>
#include <autoboost/serialization/item_version_type.hpp>

#include <autoboost/archive/detail/abi_prefix.hpp> // must be the last header

#ifdef AUTOBOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace autoboost {
namespace archive {

namespace detail {
    template<class Archive> class interface_iarchive;
} // namespace detail

template<class Archive>
class AUTOBOOST_SYMBOL_VISIBLE text_wiarchive_impl :
    public basic_text_iprimitive<std::wistream>,
    public basic_text_iarchive<Archive>
{
#ifdef AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
    #if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1500)
        // for some inexplicable reason insertion of "class" generates compile erro
        // on msvc 7.1
        friend detail::interface_iarchive<Archive>;
        friend load_access;
    #else
        friend class detail::interface_iarchive<Archive>;
        friend class load_access;
    #endif
#endif
    template<class T>
    void load(T & t){
        basic_text_iprimitive<std::wistream>::load(t);
    }
    void load(version_type & t){
        unsigned int v;
        load(v);
        t = version_type(v);
    }
    void load(autoboost::serialization::item_version_type & t){
        unsigned int v;
        load(v);
        t = autoboost::serialization::item_version_type(v);
    }
    AUTOBOOST_WARCHIVE_DECL void
    load(char * t);
    #ifndef AUTOBOOST_NO_INTRINSIC_WCHAR_T
    AUTOBOOST_WARCHIVE_DECL void
    load(wchar_t * t);
    #endif
    AUTOBOOST_WARCHIVE_DECL void
    load(std::string &s);
    #ifndef AUTOBOOST_NO_STD_WSTRING
    AUTOBOOST_WARCHIVE_DECL void
    load(std::wstring &ws);
    #endif
    template<class T>
    void load_override(T & t){
        basic_text_iarchive<Archive>::load_override(t);
    }
    AUTOBOOST_WARCHIVE_DECL
    text_wiarchive_impl(std::wistream & is, unsigned int flags);
    ~text_wiarchive_impl(){};
};

} // namespace archive
} // namespace autoboost

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

#include <autoboost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#ifdef AUTOBOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace autoboost {
namespace archive {

class AUTOBOOST_SYMBOL_VISIBLE text_wiarchive :
    public text_wiarchive_impl<text_wiarchive>{
public:
    text_wiarchive(std::wistream & is, unsigned int flags = 0) :
        text_wiarchive_impl<text_wiarchive>(is, flags)
    {}
    ~text_wiarchive(){}
};

} // namespace archive
} // namespace autoboost

// required by export
AUTOBOOST_SERIALIZATION_REGISTER_ARCHIVE(autoboost::archive::text_wiarchive)

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

#endif // AUTOBOOST_NO_STD_WSTREAMBUF
#endif // AUTOBOOST_ARCHIVE_TEXT_WIARCHIVE_HPP

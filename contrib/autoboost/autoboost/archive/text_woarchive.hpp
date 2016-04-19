#ifndef AUTOBOOST_ARCHIVE_TEXT_WOARCHIVE_HPP
#define AUTOBOOST_ARCHIVE_TEXT_WOARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// text_woarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <autoboost/config.hpp>

#ifdef AUTOBOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else

#include <ostream>
#include <cstddef> // size_t

#if defined(AUTOBOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif

#include <autoboost/archive/detail/auto_link_warchive.hpp>
#include <autoboost/archive/basic_text_oprimitive.hpp>
#include <autoboost/archive/basic_text_oarchive.hpp>
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
    template<class Archive> class interface_oarchive;
} // namespace detail

template<class Archive>
class AUTOBOOST_SYMBOL_VISIBLE text_woarchive_impl :
    public basic_text_oprimitive<std::wostream>,
    public basic_text_oarchive<Archive>
{
#ifdef AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
    #if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1500)
        // for some inexplicable reason insertion of "class" generates compile erro
        // on msvc 7.1
        friend detail::interface_oarchive<Archive>;
        friend basic_text_oarchive<Archive>;
        friend save_access;
    #else
        friend class detail::interface_oarchive<Archive>;
        friend class basic_text_oarchive<Archive>;
        friend class save_access;
    #endif
#endif
    template<class T>
    void save(const T & t){
        this->newtoken();
        basic_text_oprimitive<std::wostream>::save(t);
    }
    void save(const version_type & t){
        save(static_cast<const unsigned int>(t));
    }
    void save(const autoboost::serialization::item_version_type & t){
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
    text_woarchive_impl(std::wostream & os, unsigned int flags) :
        basic_text_oprimitive<std::wostream>(
            os,
            0 != (flags & no_codecvt)
        ),
        basic_text_oarchive<Archive>(flags)
    {
        if(0 == (flags & no_header))
            basic_text_oarchive<Archive>::init();
    }
public:
    void save_binary(const void *address, std::size_t count){
        put(static_cast<wchar_t>('\n'));
        this->end_preamble();
        #if ! defined(__MWERKS__)
        this->basic_text_oprimitive<std::wostream>::save_binary(
        #else
        this->basic_text_oprimitive::save_binary(
        #endif
            address,
            count
        );
        put(static_cast<wchar_t>('\n'));
        this->delimiter = this->none;
    }

};

// we use the following because we can't use
// typedef text_oarchive_impl<text_oarchive_impl<...> > text_oarchive;

// do not derive from this class.  If you want to extend this functionality
// via inhertance, derived from text_oarchive_impl instead.  This will
// preserve correct static polymorphism.
class AUTOBOOST_SYMBOL_VISIBLE text_woarchive :
    public text_woarchive_impl<text_woarchive>
{
public:
    text_woarchive(std::wostream & os, unsigned int flags = 0) :
        text_woarchive_impl<text_woarchive>(os, flags)
    {}
    ~text_woarchive(){}
};

} // namespace archive
} // namespace autoboost

// required by export
AUTOBOOST_SERIALIZATION_REGISTER_ARCHIVE(autoboost::archive::text_woarchive)

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

#include <autoboost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // AUTOBOOST_NO_STD_WSTREAMBUF
#endif // AUTOBOOST_ARCHIVE_TEXT_WOARCHIVE_HPP

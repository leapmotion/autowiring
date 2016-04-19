#ifndef AUTOBOOST_ARCHIVE_POLYMORPHIC_IARCHIVE_HPP
#define AUTOBOOST_ARCHIVE_POLYMORPHIC_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_iarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // std::size_t
#include <climits> // ULONG_MAX
#include <string>

#include <autoboost/config.hpp>
#if defined(AUTOBOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif

#include <autoboost/cstdint.hpp>

#include <autoboost/archive/detail/iserializer.hpp>
#include <autoboost/archive/detail/interface_iarchive.hpp>
#include <autoboost/serialization/nvp.hpp>
#include <autoboost/archive/detail/register_archive.hpp>

#include <autoboost/archive/detail/decl.hpp>
#include <autoboost/archive/detail/abi_prefix.hpp> // must be the last header

namespace autoboost {
namespace serialization {
    class extended_type_info;
} // namespace serialization
namespace archive {
namespace detail {
    class basic_iarchive;
    class basic_iserializer;
}

class polymorphic_iarchive;

class AUTOBOOST_SYMBOL_VISIBLE polymorphic_iarchive_impl :
    public detail::interface_iarchive<polymorphic_iarchive>
{
#ifdef AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
    friend class detail::interface_iarchive<polymorphic_iarchive>;
    friend class load_access;
#endif
    // primitive types the only ones permitted by polymorphic archives
    virtual void load(bool & t) = 0;

    virtual void load(char & t) = 0;
    virtual void load(signed char & t) = 0;
    virtual void load(unsigned char & t) = 0;
    #ifndef AUTOBOOST_NO_CWCHAR
    #ifndef AUTOBOOST_NO_INTRINSIC_WCHAR_T
    virtual void load(wchar_t & t) = 0;
    #endif
    #endif
    virtual void load(short & t) = 0;
    virtual void load(unsigned short & t) = 0;
    virtual void load(int & t) = 0;
    virtual void load(unsigned int & t) = 0;
    virtual void load(long & t) = 0;
    virtual void load(unsigned long & t) = 0;

    #if defined(AUTOBOOST_HAS_LONG_LONG)
    virtual void load(autoboost::long_long_type & t) = 0;
    virtual void load(autoboost::ulong_long_type & t) = 0;
    #elif defined(AUTOBOOST_HAS_MS_INT64)
    virtual void load(__int64 & t) = 0;
    virtual void load(unsigned __int64 & t) = 0;
    #endif

    virtual void load(float & t) = 0;
    virtual void load(double & t) = 0;

    // string types are treated as primitives
    virtual void load(std::string & t) = 0;
    #ifndef AUTOBOOST_NO_STD_WSTRING
    virtual void load(std::wstring & t) = 0;
    #endif

    // used for xml and other tagged formats
    virtual void load_start(const char * name) = 0;
    virtual void load_end(const char * name) = 0;
    virtual void register_basic_serializer(const detail::basic_iserializer & bis) = 0;
    virtual detail::helper_collection & get_helper_collection() = 0;

    // msvc and borland won't automatically pass these to the base class so
    // make it explicit here
    template<class T>
    void load_override(T & t)
    {
        archive::load(* this->This(), t);
    }
    // special treatment for name-value pairs.
    template<class T>
    void load_override(
        const autoboost::serialization::nvp< T > & t
    ){
        load_start(t.name());
        archive::load(* this->This(), t.value());
        load_end(t.name());
    }
protected:
    virtual ~polymorphic_iarchive_impl(){};
public:
    // utility function implemented by all legal archives
    virtual void set_library_version(library_version_type archive_library_version) = 0;
    virtual library_version_type get_library_version() const = 0;
    virtual unsigned int get_flags() const = 0;
    virtual void delete_created_pointers() = 0;
    virtual void reset_object_address(
        const void * new_address,
        const void * old_address
    ) = 0;

    virtual void load_binary(void * t, std::size_t size) = 0;

    // these are used by the serialization library implementation.
    virtual void load_object(
        void *t,
        const detail::basic_iserializer & bis
    ) = 0;
    virtual const detail::basic_pointer_iserializer * load_pointer(
        void * & t,
        const detail::basic_pointer_iserializer * bpis_ptr,
        const detail::basic_pointer_iserializer * (*finder)(
            const autoboost::serialization::extended_type_info & type
        )
    ) = 0;
};

} // namespace archive
} // namespace autoboost

#include <autoboost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

namespace autoboost {
namespace archive {

class AUTOBOOST_SYMBOL_VISIBLE polymorphic_iarchive :
    public polymorphic_iarchive_impl
{
public:
    virtual ~polymorphic_iarchive(){};
};

} // namespace archive
} // namespace autoboost

// required by export
AUTOBOOST_SERIALIZATION_REGISTER_ARCHIVE(autoboost::archive::polymorphic_iarchive)

#endif // AUTOBOOST_ARCHIVE_POLYMORPHIC_IARCHIVE_HPP

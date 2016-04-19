#ifndef AUTOBOOST_ARCHIVE_BASIC_XML_OARCHIVE_HPP
#define AUTOBOOST_ARCHIVE_BASIC_XML_OARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_xml_oarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <autoboost/config.hpp>
#include <autoboost/mpl/assert.hpp>
#include <autoboost/detail/workaround.hpp>

#include <autoboost/archive/detail/common_oarchive.hpp>

#include <autoboost/serialization/nvp.hpp>
#include <autoboost/serialization/tracking.hpp>
#include <autoboost/serialization/string.hpp>


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

//////////////////////////////////////////////////////////////////////
// class basic_xml_oarchive - write serialized objects to a xml output stream
template<class Archive>
class AUTOBOOST_SYMBOL_VISIBLE basic_xml_oarchive :
    public detail::common_oarchive<Archive>
{
#ifdef AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
protected:
#endif
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1500)
    // for some inexplicable reason insertion of "class" generates compile erro
    // on msvc 7.1
    friend detail::interface_oarchive<Archive>;
#else
    friend class detail::interface_oarchive<Archive>;
#endif
    friend class save_access;
    // special stuff for xml output
    unsigned int depth;
    bool indent_next;
    bool pending_preamble;
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    indent();
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    init();
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    write_attribute(
        const char *attribute_name,
        int t,
        const char *conjunction = "=\""
    );
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    write_attribute(
        const char *attribute_name,
        const char *key
    );
    // helpers used below
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_start(const char *name);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_end(const char *name);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    end_preamble();

    // Anything not an attribute and not a name-value pair is an
    // error and should be trapped here.
    template<class T>
    void save_override(T & t)
    {
        // If your program fails to compile here, its most likely due to
        // not specifying an nvp wrapper around the variable to
        // be serialized.
        AUTOBOOST_MPL_ASSERT((serialization::is_wrapper< T >));
        this->detail_common_oarchive::save_override(t);
    }

    // special treatment for name-value pairs.
    typedef detail::common_oarchive<Archive> detail_common_oarchive;
    template<class T>
    void save_override(
        const ::autoboost::serialization::nvp< T > & t
    ){
        this->This()->save_start(t.name());
        this->detail_common_oarchive::save_override(t.const_value());
        this->This()->save_end(t.name());
    }

    // specific overrides for attributes - not name value pairs so we
    // want to trap them before the above "fall through"
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const object_id_type & t);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const object_reference_type & t);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const version_type & t);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const class_id_type & t);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const class_id_optional_type & t);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const class_id_reference_type & t);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const class_name_type & t);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
    save_override(const tracking_type & t);

    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL
    basic_xml_oarchive(unsigned int flags);
    AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL
    ~basic_xml_oarchive();
};

} // namespace archive
} // namespace autoboost

#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif

#include <autoboost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // AUTOBOOST_ARCHIVE_BASIC_XML_OARCHIVE_HPP

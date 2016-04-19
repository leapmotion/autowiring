#ifndef AUTOBOOST_ARCHIVE_BASIC_OARCHIVE_HPP
#define AUTOBOOST_ARCHIVE_BASIC_OARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_oarchive.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // NULL
#include <autoboost/config.hpp>
#include <autoboost/noncopyable.hpp>
#include <autoboost/scoped_ptr.hpp>

#include <autoboost/archive/basic_archive.hpp>
#include <autoboost/serialization/tracking_enum.hpp>
#include <autoboost/archive/detail/helper_collection.hpp>
#include <autoboost/archive/detail/abi_prefix.hpp> // must be the last header

namespace autoboost {
namespace serialization {
    class extended_type_info;
} // namespace serialization

namespace archive {
namespace detail {

class basic_oarchive_impl;
class basic_oserializer;
class basic_pointer_oserializer;

//////////////////////////////////////////////////////////////////////
// class basic_oarchive - write serialized objects to an output stream
class AUTOBOOST_SYMBOL_VISIBLE basic_oarchive :
    private autoboost::noncopyable,
    public autoboost::archive::detail::helper_collection
{
    friend class basic_oarchive_impl;
    // hide implementation of this class to minimize header conclusion
    autoboost::scoped_ptr<basic_oarchive_impl> pimpl;

    // overload these to bracket object attributes. Used to implement
    // xml archives
    virtual void vsave(const version_type t) =  0;
    virtual void vsave(const object_id_type t) =  0;
    virtual void vsave(const object_reference_type t) =  0;
    virtual void vsave(const class_id_type t) =  0;
    virtual void vsave(const class_id_optional_type t) = 0;
    virtual void vsave(const class_id_reference_type t) =  0;
    virtual void vsave(const class_name_type & t) = 0;
    virtual void vsave(const tracking_type t) = 0;
protected:
    AUTOBOOST_ARCHIVE_DECL basic_oarchive(unsigned int flags = 0);
    AUTOBOOST_ARCHIVE_DECL autoboost::archive::detail::helper_collection &
    get_helper_collection();
    virtual AUTOBOOST_ARCHIVE_DECL ~basic_oarchive();
public:
    // note: NOT part of the public interface
    AUTOBOOST_ARCHIVE_DECL void register_basic_serializer(
        const basic_oserializer & bos
    );
    AUTOBOOST_ARCHIVE_DECL void save_object(
        const void *x,
        const basic_oserializer & bos
    );
    AUTOBOOST_ARCHIVE_DECL void save_pointer(
        const void * t,
        const basic_pointer_oserializer * bpos_ptr
    );
    void save_null_pointer(){
        vsave(NULL_POINTER_TAG);
    }
    // real public interface starts here
    AUTOBOOST_ARCHIVE_DECL void end_preamble(); // default implementation does nothing
    AUTOBOOST_ARCHIVE_DECL library_version_type get_library_version() const;
    AUTOBOOST_ARCHIVE_DECL unsigned int get_flags() const;
};

} // namespace detail
} // namespace archive
} // namespace autoboost

#include <autoboost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif //AUTOBOOST_ARCHIVE_BASIC_OARCHIVE_HPP

#ifndef AUTOBOOST_ARCHIVE_BASIC_ARCHIVE_HPP
#define AUTOBOOST_ARCHIVE_BASIC_ARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_archive.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
#include <cstring> // count
#include <autoboost/assert.hpp>
#include <autoboost/config.hpp>
#include <autoboost/cstdint.hpp> // size_t
#include <autoboost/noncopyable.hpp>
#include <autoboost/integer_traits.hpp>

#include <autoboost/archive/detail/auto_link_archive.hpp>
#include <autoboost/archive/detail/abi_prefix.hpp> // must be the last header

namespace autoboost {
namespace archive {

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4244 4267 )
#endif

/* NOTE : Warning  : Warning : Warning : Warning : Warning
 * Don't ever changes this.  If you do, they previously created
 * binary archives won't be readable !!!
 */
class library_version_type {
private:
    typedef uint_least16_t base_type;
    base_type t;
public:
    library_version_type(): t(0) {};
    explicit library_version_type(const unsigned int & t_) : t(t_){
        AUTOBOOST_ASSERT(t_ <= autoboost::integer_traits<base_type>::const_max);
    }
    library_version_type(const library_version_type & t_) :
        t(t_.t)
    {}
    library_version_type & operator=(const library_version_type & rhs){
        t = rhs.t;
        return *this;
    }
    // used for text output
    operator base_type () const {
        return t;
    }
    // used for text input
    operator base_type & (){
        return t;
    }
    bool operator==(const library_version_type & rhs) const {
        return t == rhs.t;
    }
    bool operator<(const library_version_type & rhs) const {
        return t < rhs.t;
    }
};

AUTOBOOST_ARCHIVE_DECL library_version_type
AUTOBOOST_ARCHIVE_VERSION();

class version_type {
private:
    typedef uint_least32_t base_type;
    base_type t;
public:
    // should be private - but MPI fails if it's not!!!
    version_type(): t(0) {};
    explicit version_type(const unsigned int & t_) : t(t_){
        AUTOBOOST_ASSERT(t_ <= autoboost::integer_traits<base_type>::const_max);
    }
    version_type(const version_type & t_) :
        t(t_.t)
    {}
    version_type & operator=(const version_type & rhs){
        t = rhs.t;
        return *this;
    }
    // used for text output
    operator base_type () const {
        return t;
    }
    // used for text intput
    operator base_type  & (){
        return t;
    }
    bool operator==(const version_type & rhs) const {
        return t == rhs.t;
    }
    bool operator<(const version_type & rhs) const {
        return t < rhs.t;
    }
};

class class_id_type {
private:
    typedef int_least16_t base_type;
    base_type t;
public:
    // should be private - but then can't use AUTOBOOST_STRONG_TYPE below
    class_id_type() : t(0) {};
    explicit class_id_type(const int t_) : t(t_){
        AUTOBOOST_ASSERT(t_ <= autoboost::integer_traits<base_type>::const_max);
    }
    explicit class_id_type(const std::size_t t_) : t(t_){
 //       AUTOBOOST_ASSERT(t_ <= autoboost::integer_traits<base_type>::const_max);
    }
    class_id_type(const class_id_type & t_) :
        t(t_.t)
    {}
    class_id_type & operator=(const class_id_type & rhs){
        t = rhs.t;
        return *this;
    }

    // used for text output
    operator int () const {
        return t;
    }
    // used for text input
    operator int_least16_t &() {
        return t;
    }
    bool operator==(const class_id_type & rhs) const {
        return t == rhs.t;
    }
    bool operator<(const class_id_type & rhs) const {
        return t < rhs.t;
    }
};

#define NULL_POINTER_TAG autoboost::archive::class_id_type(-1)

class object_id_type {
private:
    typedef uint_least32_t base_type;
    base_type t;
public:
    object_id_type(): t(0) {};
    // note: presumes that size_t >= unsigned int.
    explicit object_id_type(const std::size_t & t_) : t(t_){
        AUTOBOOST_ASSERT(t_ <= autoboost::integer_traits<base_type>::const_max);
    }
    object_id_type(const object_id_type & t_) :
        t(t_.t)
    {}
    object_id_type & operator=(const object_id_type & rhs){
        t = rhs.t;
        return *this;
    }
    // used for text output
    operator uint_least32_t () const {
        return t;
    }
    // used for text input
    operator uint_least32_t & () {
        return t;
    }
    bool operator==(const object_id_type & rhs) const {
        return t == rhs.t;
    }
    bool operator<(const object_id_type & rhs) const {
        return t < rhs.t;
    }
};

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

struct tracking_type {
    bool t;
    explicit tracking_type(const bool t_ = false)
        : t(t_)
    {};
    tracking_type(const tracking_type & t_)
        : t(t_.t)
    {}
    operator bool () const {
        return t;
    };
    operator bool & () {
        return t;
    };
    tracking_type & operator=(const bool t_){
        t = t_;
        return *this;
    }
    bool operator==(const tracking_type & rhs) const {
        return t == rhs.t;
    }
    bool operator==(const bool & rhs) const {
        return t == rhs;
    }
    tracking_type & operator=(const tracking_type & rhs){
        t = rhs.t;
        return *this;
    }
};

struct class_name_type :
    private autoboost::noncopyable
{
    char *t;
    operator const char * & () const {
        return const_cast<const char * &>(t);
    }
    operator char * () {
        return t;
    }
    std::size_t size() const {
        return std::strlen(t);
    }
    explicit class_name_type(const char *key_)
    : t(const_cast<char *>(key_)){}
    explicit class_name_type(char *key_)
    : t(key_){}
    class_name_type & operator=(const class_name_type & rhs){
        t = rhs.t;
        return *this;
    }
};

enum archive_flags {
    no_header = 1,  // suppress archive header info
    no_codecvt = 2,  // suppress alteration of codecvt facet
    no_xml_tag_checking = 4,   // suppress checking of xml tags
    no_tracking = 8,           // suppress ALL tracking
    flags_last = 8
};

AUTOBOOST_ARCHIVE_DECL const char *
AUTOBOOST_ARCHIVE_SIGNATURE();

/* NOTE : Warning  : Warning : Warning : Warning : Warning
 * If any of these are changed to different sized types,
 * binary_iarchive won't be able to read older archives
 * unless you rev the library version and include conditional
 * code based on the library version.  There is nothing
 * inherently wrong in doing this - but you have to be super
 * careful because it's easy to get wrong and start breaking
 * old archives !!!
 */

#define AUTOBOOST_ARCHIVE_STRONG_TYPEDEF(T, D)         \
    class D : public T {                           \
    public:                                        \
        explicit D(const T tt) : T(tt){}           \
    };                                             \
/**/

AUTOBOOST_ARCHIVE_STRONG_TYPEDEF(class_id_type, class_id_reference_type)
AUTOBOOST_ARCHIVE_STRONG_TYPEDEF(class_id_type, class_id_optional_type)
AUTOBOOST_ARCHIVE_STRONG_TYPEDEF(object_id_type, object_reference_type)

}// namespace archive
}// namespace autoboost

#include <autoboost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#include <autoboost/serialization/level.hpp>

// set implementation level to primitive for all types
// used internally by the serialization library

AUTOBOOST_CLASS_IMPLEMENTATION(autoboost::archive::library_version_type, primitive_type)
AUTOBOOST_CLASS_IMPLEMENTATION(autoboost::archive::version_type, primitive_type)
AUTOBOOST_CLASS_IMPLEMENTATION(autoboost::archive::class_id_type, primitive_type)
AUTOBOOST_CLASS_IMPLEMENTATION(autoboost::archive::class_id_reference_type, primitive_type)
AUTOBOOST_CLASS_IMPLEMENTATION(autoboost::archive::class_id_optional_type, primitive_type)
AUTOBOOST_CLASS_IMPLEMENTATION(autoboost::archive::class_name_type, primitive_type)
AUTOBOOST_CLASS_IMPLEMENTATION(autoboost::archive::object_id_type, primitive_type)
AUTOBOOST_CLASS_IMPLEMENTATION(autoboost::archive::object_reference_type, primitive_type)
AUTOBOOST_CLASS_IMPLEMENTATION(autoboost::archive::tracking_type, primitive_type)

#include <autoboost/serialization/is_bitwise_serializable.hpp>

// set types used internally by the serialization library
// to be bitwise serializable

AUTOBOOST_IS_BITWISE_SERIALIZABLE(autoboost::archive::library_version_type)
AUTOBOOST_IS_BITWISE_SERIALIZABLE(autoboost::archive::version_type)
AUTOBOOST_IS_BITWISE_SERIALIZABLE(autoboost::archive::class_id_type)
AUTOBOOST_IS_BITWISE_SERIALIZABLE(autoboost::archive::class_id_reference_type)
AUTOBOOST_IS_BITWISE_SERIALIZABLE(autoboost::archive::class_id_optional_type)
AUTOBOOST_IS_BITWISE_SERIALIZABLE(autoboost::archive::class_name_type)
AUTOBOOST_IS_BITWISE_SERIALIZABLE(autoboost::archive::object_id_type)
AUTOBOOST_IS_BITWISE_SERIALIZABLE(autoboost::archive::object_reference_type)
AUTOBOOST_IS_BITWISE_SERIALIZABLE(autoboost::archive::tracking_type)

#endif //AUTOBOOST_ARCHIVE_BASIC_ARCHIVE_HPP

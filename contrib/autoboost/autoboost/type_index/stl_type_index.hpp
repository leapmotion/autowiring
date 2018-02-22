//
// Copyright (c) Antony Polukhin, 2013-2017.
//
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_TYPE_INDEX_STL_TYPE_INDEX_HPP
#define AUTOBOOST_TYPE_INDEX_STL_TYPE_INDEX_HPP

/// \file stl_type_index.hpp
/// \brief Contains autoboost::typeindex::stl_type_index class.
///
/// autoboost::typeindex::stl_type_index class can be used as a drop-in replacement
/// for std::type_index.
///
/// It is used in situations when RTTI is enabled or typeid() method is available.
/// When typeid() is disabled or AUTOBOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY macro
/// is defined autoboost::typeindex::ctti is usually used instead of autoboost::typeindex::stl_type_index.

#include <autoboost/type_index/type_index_facade.hpp>

// MSVC is capable of calling typeid(T) even when RTTI is off
#if defined(AUTOBOOST_NO_RTTI) && !defined(AUTOBOOST_MSVC)
#error "File autoboost/type_index/stl_type_index.ipp is not usable when typeid() is not available."
#endif

#include <typeinfo>
#include <cstring>                                  // std::strcmp, std::strlen, std::strstr
#include <stdexcept>
#include <autoboost/static_assert.hpp>
#include <autoboost/throw_exception.hpp>
#include <autoboost/core/demangle.hpp>
#include <autoboost/type_traits/is_const.hpp>
#include <autoboost/type_traits/is_reference.hpp>
#include <autoboost/type_traits/is_volatile.hpp>
#include <autoboost/type_traits/remove_cv.hpp>
#include <autoboost/type_traits/remove_reference.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/or.hpp>

#if (defined(__EDG_VERSION__) && __EDG_VERSION__ < 245) \
        || (defined(__sgi) && defined(_COMPILER_VERSION) && _COMPILER_VERSION <= 744)
#   include <autoboost/type_traits/is_signed.hpp>
#   include <autoboost/type_traits/make_signed.hpp>
#   include <autoboost/mpl/identity.hpp>
#endif

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace autoboost { namespace typeindex {

/// \class stl_type_index
/// This class is a wrapper around std::type_info, that workarounds issues and provides
/// much more rich interface. \b For \b description \b of \b functions \b see type_index_facade.
///
/// This class requires typeid() to work. For cases when RTTI is disabled see ctti_type_index.
class stl_type_index
    : public type_index_facade<
        stl_type_index,
        #ifdef AUTOBOOST_NO_STD_TYPEINFO
            type_info
        #else
            std::type_info
        #endif
    >
{
public:
#ifdef AUTOBOOST_NO_STD_TYPEINFO
    typedef type_info type_info_t;
#else
    typedef std::type_info type_info_t;
#endif

private:
    const type_info_t* data_;

public:
    inline stl_type_index() AUTOBOOST_NOEXCEPT
        : data_(&typeid(void))
    {}

    inline stl_type_index(const type_info_t& data) AUTOBOOST_NOEXCEPT
        : data_(&data)
    {}

    inline const type_info_t&  type_info() const AUTOBOOST_NOEXCEPT;

    inline const char*  raw_name() const AUTOBOOST_NOEXCEPT;
    inline const char*  name() const AUTOBOOST_NOEXCEPT;
    inline std::string  pretty_name() const;

    inline std::size_t  hash_code() const AUTOBOOST_NOEXCEPT;
    inline bool         equal(const stl_type_index& rhs) const AUTOBOOST_NOEXCEPT;
    inline bool         before(const stl_type_index& rhs) const AUTOBOOST_NOEXCEPT;

    template <class T>
    inline static stl_type_index type_id() AUTOBOOST_NOEXCEPT;

    template <class T>
    inline static stl_type_index type_id_with_cvr() AUTOBOOST_NOEXCEPT;

    template <class T>
    inline static stl_type_index type_id_runtime(const T& value) AUTOBOOST_NOEXCEPT;
};

inline const stl_type_index::type_info_t& stl_type_index::type_info() const AUTOBOOST_NOEXCEPT {
    return *data_;
}


inline const char* stl_type_index::raw_name() const AUTOBOOST_NOEXCEPT {
#ifdef _MSC_VER
    return data_->raw_name();
#else
    return data_->name();
#endif
}

inline const char* stl_type_index::name() const AUTOBOOST_NOEXCEPT {
    return data_->name();
}

inline std::string stl_type_index::pretty_name() const {
    static const char cvr_saver_name[] = "autoboost::typeindex::detail::cvr_saver<";
    static AUTOBOOST_CONSTEXPR_OR_CONST std::string::size_type cvr_saver_name_len = sizeof(cvr_saver_name) - 1;

    // In case of MSVC demangle() is a no-op, and name() already returns demangled name.
    // In case of GCC and Clang (on non-Windows systems) name() returns mangled name and demangle() undecorates it.
    const autoboost::core::scoped_demangled_name demangled_name(data_->name());

    const char* begin = demangled_name.get();
    if (!begin) {
        autoboost::throw_exception(std::runtime_error("Type name demangling failed"));
    }

    const std::string::size_type len = std::strlen(begin);
    const char* end = begin + len;

    if (len > cvr_saver_name_len) {
        const char* b = std::strstr(begin, cvr_saver_name);
        if (b) {
            b += cvr_saver_name_len;

            // Trim leading spaces
            while (*b == ' ') {         // the string is zero terminated, we won't exceed the buffer size
                ++ b;
            }

            // Skip the closing angle bracket
            const char* e = end - 1;
            while (e > b && *e != '>') {
                -- e;
            }

            // Trim trailing spaces
            while (e > b && *(e - 1) == ' ') {
                -- e;
            }

            if (b < e) {
                // Parsing seems to have succeeded, the type name is not empty
                begin = b;
                end = e;
            }
        }
    }

    return std::string(begin, end);
}


inline std::size_t stl_type_index::hash_code() const AUTOBOOST_NOEXCEPT {
#if (defined(_MSC_VER) && _MSC_VER > 1600) || (__GNUC__ == 4 && __GNUC_MINOR__ > 5 && defined(__GXX_EXPERIMENTAL_CXX0X__))
    return data_->hash_code();
#else
    return autoboost::hash_range(raw_name(), raw_name() + std::strlen(raw_name()));
#endif
}


/// @cond

// for this compiler at least, cross-shared-library type_info
// comparisons don't work, so we are using typeid(x).name() instead.
# if (defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 5))) \
    || defined(_AIX) \
    || (defined(__sgi) && defined(__host_mips)) \
    || (defined(__hpux) && defined(__HP_aCC)) \
    || (defined(linux) && defined(__INTEL_COMPILER) && defined(__ICC))
#  define AUTOBOOST_CLASSINFO_COMPARE_BY_NAMES
# endif

/// @endcond

inline bool stl_type_index::equal(const stl_type_index& rhs) const AUTOBOOST_NOEXCEPT {
#ifdef AUTOBOOST_CLASSINFO_COMPARE_BY_NAMES
    return raw_name() == rhs.raw_name() || !std::strcmp(raw_name(), rhs.raw_name());
#else
    return !!(*data_ == *rhs.data_);
#endif
}

inline bool stl_type_index::before(const stl_type_index& rhs) const AUTOBOOST_NOEXCEPT {
#ifdef AUTOBOOST_CLASSINFO_COMPARE_BY_NAMES
    return raw_name() != rhs.raw_name() && std::strcmp(raw_name(), rhs.raw_name()) < 0;
#else
    return !!data_->before(*rhs.data_);
#endif
}

#ifdef AUTOBOOST_CLASSINFO_COMPARE_BY_NAMES
#undef AUTOBOOST_CLASSINFO_COMPARE_BY_NAMES
#endif



template <class T>
inline stl_type_index stl_type_index::type_id() AUTOBOOST_NOEXCEPT {
    typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::remove_reference<T>::type no_ref_t;
    typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::remove_cv<no_ref_t>::type no_cvr_prefinal_t;

    #  if (defined(__EDG_VERSION__) && __EDG_VERSION__ < 245) \
        || (defined(__sgi) && defined(_COMPILER_VERSION) && _COMPILER_VERSION <= 744)

        // Old EDG-based compilers seem to mistakenly distinguish 'integral' from 'signed integral'
        // in typeid() expressions. Full template specialization for 'integral' fixes that issue:
        typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::mpl::if_<
            autoboost::is_signed<no_cvr_prefinal_t>,
            autoboost::make_signed<no_cvr_prefinal_t>,
            autoboost::mpl::identity<no_cvr_prefinal_t>
        >::type no_cvr_prefinal_lazy_t;

        typedef AUTOBOOST_DEDUCED_TYPENAME no_cvr_prefinal_t::type no_cvr_t;
    #else
        typedef no_cvr_prefinal_t no_cvr_t;
    #endif

    return typeid(no_cvr_t);
}

namespace detail {
    template <class T> class cvr_saver{};
}

template <class T>
inline stl_type_index stl_type_index::type_id_with_cvr() AUTOBOOST_NOEXCEPT {
    typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::mpl::if_<
        autoboost::mpl::or_<autoboost::is_reference<T>, autoboost::is_const<T>, autoboost::is_volatile<T> >,
        detail::cvr_saver<T>,
        T
    >::type type;

    return typeid(type);
}


template <class T>
inline stl_type_index stl_type_index::type_id_runtime(const T& value) AUTOBOOST_NOEXCEPT {
#ifdef AUTOBOOST_NO_RTTI
    return value.boost_type_index_type_id_runtime_();
#else
    return typeid(value);
#endif
}

}} // namespace autoboost::typeindex

#endif // AUTOBOOST_TYPE_INDEX_STL_TYPE_INDEX_HPP


//  (C) Copyright Matthias Troyerk 2006.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_DETAIL_HAS_DEFAULT_CONSTRUCTOR_HPP_INCLUDED
#define AUTOBOOST_DETAIL_HAS_DEFAULT_CONSTRUCTOR_HPP_INCLUDED

#include <autoboost/type_traits/has_trivial_constructor.hpp>

namespace autoboost { namespace detail {

/// type trait to check for a default constructor
///
/// The default implementation just checks for a trivial constructor.
/// Using some compiler magic it might be possible to provide a better default

template <class T>
struct has_default_constructor
 : public has_trivial_constructor<T>
{};

} } // namespace autoboost::detail


#endif // AUTOBOOST_DETAIL_HAS_DEFAULT_CONSTRUCTOR_HPP_INCLUDED

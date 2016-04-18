// Copyright (C) 2004 Arkadiy Vertleyb

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// autoboostinspect:nounnamed

#ifndef AUTOBOOST_TYPEOF_ENCODE_DECODE_HPP_INCLUDED
#define AUTOBOOST_TYPEOF_ENCODE_DECODE_HPP_INCLUDED

#include <autoboost/mpl/deref.hpp>
#include <autoboost/mpl/next.hpp>

#ifndef AUTOBOOST_TYPEOF_SUPPRESS_UNNAMED_NAMESPACE

#   define AUTOBOOST_TYPEOF_BEGIN_ENCODE_NS namespace { namespace autoboost_typeof {
#   define AUTOBOOST_TYPEOF_END_ENCODE_NS }}
#   define AUTOBOOST_TYPEOF_ENCODE_NS_QUALIFIER autoboost_typeof

#else

#   define AUTOBOOST_TYPEOF_BEGIN_ENCODE_NS namespace autoboost { namespace type_of {
#   define AUTOBOOST_TYPEOF_END_ENCODE_NS }}
#   define AUTOBOOST_TYPEOF_ENCODE_NS_QUALIFIER autoboost::type_of

#   define AUTOBOOST_TYPEOF_TEXT "unnamed namespace is off"
#   include <autoboost/typeof/message.hpp>

#endif

AUTOBOOST_TYPEOF_BEGIN_ENCODE_NS

template<class V, class Type_Not_Registered_With_Typeof_System>
struct encode_type_impl;

template<class T, class Iter>
struct decode_type_impl
{
    typedef int type;  // MSVC ETI workaround
};

template<class T>
struct decode_nested_template_helper_impl;

AUTOBOOST_TYPEOF_END_ENCODE_NS

namespace autoboost { namespace type_of {

    template<class V, class T>
    struct encode_type : AUTOBOOST_TYPEOF_ENCODE_NS_QUALIFIER::encode_type_impl<V, T>
    {};

    template<class Iter>
    struct decode_type : AUTOBOOST_TYPEOF_ENCODE_NS_QUALIFIER::decode_type_impl<
        typename Iter::type,
        typename Iter::next
    >
    {};
}}

#endif//AUTOBOOST_TYPEOF_ENCODE_DECODE_HPP_INCLUDED

// Copyright (C) 2004 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_TYPE_ENCODING_HPP_INCLUDED
#define AUTOBOOST_TYPEOF_TYPE_ENCODING_HPP_INCLUDED

#define AUTOBOOST_TYPEOF_REGISTER_TYPE_IMPL(T, Id)                          \
                                                                        \
    template<class V> struct encode_type_impl<V, T >                    \
        : autoboost::type_of::push_back<V, autoboost::mpl::size_t<Id> >         \
    {};                                                                 \
    template<class Iter> struct decode_type_impl<autoboost::mpl::size_t<Id>, Iter> \
    {                                                                   \
        typedef T type;                                                 \
        typedef Iter iter;                                              \
    };

#define AUTOBOOST_TYPEOF_REGISTER_TYPE_EXPLICIT_ID(Type, Id)                \
    AUTOBOOST_TYPEOF_BEGIN_ENCODE_NS                                        \
    AUTOBOOST_TYPEOF_REGISTER_TYPE_IMPL(Type, Id)                           \
    AUTOBOOST_TYPEOF_END_ENCODE_NS

#define AUTOBOOST_TYPEOF_REGISTER_TYPE(Type)                                \
    AUTOBOOST_TYPEOF_REGISTER_TYPE_EXPLICIT_ID(Type, AUTOBOOST_TYPEOF_UNIQUE_ID())

#endif//AUTOBOOST_TYPEOF_TYPE_ENCODING_HPP_INCLUDED

//-----------------------------------------------------------------------------
// autoboost detail/templated_streams.hpp header file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2003
// Eric Friedman
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_DETAIL_TEMPLATED_STREAMS_HPP
#define AUTOBOOST_DETAIL_TEMPLATED_STREAMS_HPP

#include "autoboost/config.hpp"

///////////////////////////////////////////////////////////////////////////////
// (detail) AUTOBOOST_TEMPLATED_STREAM_* macros
//
// Provides workaround platforms without stream class templates.
//

#if !defined(AUTOBOOST_NO_STD_LOCALE)

#define AUTOBOOST_TEMPLATED_STREAM_TEMPLATE(E,T) \
    template < typename E , typename T >

#define AUTOBOOST_TEMPLATED_STREAM_TEMPLATE_ALLOC(E,T,A) \
    template < typename E , typename T , typename A >

#define AUTOBOOST_TEMPLATED_STREAM_ARGS(E,T) \
    typename E , typename T

#define AUTOBOOST_TEMPLATED_STREAM_ARGS_ALLOC(E,T,A) \
    typename E , typename T , typename A

#define AUTOBOOST_TEMPLATED_STREAM_COMMA        ,

#define AUTOBOOST_TEMPLATED_STREAM_ELEM(E)      E
#define AUTOBOOST_TEMPLATED_STREAM_TRAITS(T)    T
#define AUTOBOOST_TEMPLATED_STREAM_ALLOC(A)     A

#define AUTOBOOST_TEMPLATED_STREAM(X,E,T) \
    AUTOBOOST_JOIN(std::basic_,X)< E , T >

#define AUTOBOOST_TEMPLATED_STREAM_WITH_ALLOC(X,E,T,A) \
    AUTOBOOST_JOIN(std::basic_,X)< E , T , A >

#else // defined(AUTOBOOST_NO_STD_LOCALE)

#define AUTOBOOST_TEMPLATED_STREAM_TEMPLATE(E,T) /**/

#define AUTOBOOST_TEMPLATED_STREAM_TEMPLATE_ALLOC(E,T,A) /**/

#define AUTOBOOST_TEMPLATED_STREAM_ARGS(E,T) /**/

#define AUTOBOOST_TEMPLATED_STREAM_ARGS_ALLOC(E,T,A) /**/

#define AUTOBOOST_TEMPLATED_STREAM_COMMA        /**/

#define AUTOBOOST_TEMPLATED_STREAM_ELEM(E)      char
#define AUTOBOOST_TEMPLATED_STREAM_TRAITS(T)    std::char_traits<char>
#define AUTOBOOST_TEMPLATED_STREAM_ALLOC(A)     std::allocator<char>

#define AUTOBOOST_TEMPLATED_STREAM(X,E,T) \
    std::X

#define AUTOBOOST_TEMPLATED_STREAM_WITH_ALLOC(X,E,T,A) \
    std::X

#endif // AUTOBOOST_NO_STD_LOCALE

#endif // AUTOBOOST_DETAIL_TEMPLATED_STREAMS_HPP

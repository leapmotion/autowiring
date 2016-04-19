// Copyright (C) 2005 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_ENCODE_DECODE_PARAMS_HPP_INCLUDED
#define AUTOBOOST_TYPEOF_ENCODE_DECODE_PARAMS_HPP_INCLUDED

#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/repetition/repeat.hpp>

// Assumes iter0 contains initial iterator

#define AUTOBOOST_TYPEOF_DECODE_PARAM(z, n, text)   \
    typedef autoboost::type_of::decode_type<iter##n> decode##n;     \
    typedef typename decode##n::type p##n;      \
    typedef typename decode##n::iter AUTOBOOST_PP_CAT(iter, AUTOBOOST_PP_INC(n));

#define AUTOBOOST_TYPEOF_DECODE_PARAMS(n)\
    AUTOBOOST_PP_REPEAT(n, AUTOBOOST_TYPEOF_DECODE_PARAM, ~)

// The P0, P1, ... PN are encoded and added to V

#define AUTOBOOST_TYPEOF_ENCODE_PARAMS_BEGIN(z, n, text)\
    typename autoboost::type_of::encode_type<

#define AUTOBOOST_TYPEOF_ENCODE_PARAMS_END(z, n, text)\
    , AUTOBOOST_PP_CAT(P, n)>::type

#define AUTOBOOST_TYPEOF_ENCODE_PARAMS(n, ID)                   \
    AUTOBOOST_PP_REPEAT(n, AUTOBOOST_TYPEOF_ENCODE_PARAMS_BEGIN, ~) \
    typename autoboost::type_of::push_back<V, autoboost::mpl::size_t<ID> >::type      \
    AUTOBOOST_PP_REPEAT(n, AUTOBOOST_TYPEOF_ENCODE_PARAMS_END, ~)

#endif//AUTOBOOST_TYPEOF_ENCODE_DECODE_PARAMS_HPP_INCLUDED

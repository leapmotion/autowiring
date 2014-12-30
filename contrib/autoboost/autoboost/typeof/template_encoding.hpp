// Copyright (C) 2004 Arkadiy Vertleyb
// Copyright (C) 2005 Peder Holt
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_TEMPLATE_ENCODING_HPP_INCLUDED
#define AUTOBOOST_TYPEOF_TEMPLATE_ENCODING_HPP_INCLUDED

#include <autoboost/preprocessor/cat.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing.hpp>
#include <autoboost/preprocessor/control/iif.hpp>
#include <autoboost/preprocessor/detail/is_unary.hpp>
#include <autoboost/preprocessor/repetition/repeat.hpp>
#include <autoboost/preprocessor/tuple/eat.hpp>
#include <autoboost/preprocessor/seq/transform.hpp>
#include <autoboost/preprocessor/seq/for_each_i.hpp>
#include <autoboost/preprocessor/seq/cat.hpp>

#include <autoboost/typeof/encode_decode.hpp>
#include <autoboost/typeof/int_encoding.hpp>

#include <autoboost/typeof/type_template_param.hpp>
#include <autoboost/typeof/integral_template_param.hpp>
#include <autoboost/typeof/template_template_param.hpp>

#ifdef __BORLANDC__
#define AUTOBOOST_TYPEOF_QUALIFY(P) self_t::P
#else
#define AUTOBOOST_TYPEOF_QUALIFY(P) P
#endif
// The template parameter description, entered by the user,
// is converted into a polymorphic "object"
// that is used to generate the code responsible for
// encoding/decoding the parameter, etc.

// make sure to cat the sequence first, and only then add the prefix.
#define AUTOBOOST_TYPEOF_MAKE_OBJ(elem) AUTOBOOST_PP_CAT(\
    AUTOBOOST_TYPEOF_MAKE_OBJ,\
    AUTOBOOST_PP_SEQ_CAT((_) AUTOBOOST_TYPEOF_TO_SEQ(elem))\
    )

#define AUTOBOOST_TYPEOF_TO_SEQ(tokens) AUTOBOOST_TYPEOF_ ## tokens ## _AUTOBOOST_TYPEOF

// AUTOBOOST_TYPEOF_REGISTER_TEMPLATE

#define AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_EXPLICIT_ID(Name, Params, Id)\
    AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_IMPL(\
        Name,\
        AUTOBOOST_TYPEOF_MAKE_OBJS(AUTOBOOST_TYPEOF_TOSEQ(Params)),\
        AUTOBOOST_PP_SEQ_SIZE(AUTOBOOST_TYPEOF_TOSEQ(Params)),\
        Id)

#define AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(Name, Params)\
    AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_EXPLICIT_ID(Name, Params, AUTOBOOST_TYPEOF_UNIQUE_ID())

#define AUTOBOOST_TYPEOF_OBJECT_MAKER(s, data, elem)\
    AUTOBOOST_TYPEOF_MAKE_OBJ(elem)

#define AUTOBOOST_TYPEOF_MAKE_OBJS(Params)\
    AUTOBOOST_PP_SEQ_TRANSFORM(AUTOBOOST_TYPEOF_OBJECT_MAKER, ~, Params)

// As suggested by Paul Mensonides:

#define AUTOBOOST_TYPEOF_TOSEQ(x)\
    AUTOBOOST_PP_IIF(\
        AUTOBOOST_PP_IS_UNARY(x),\
        x AUTOBOOST_PP_TUPLE_EAT(3), AUTOBOOST_PP_REPEAT\
    )(x, AUTOBOOST_TYPEOF_TOSEQ_2, ~)

#define AUTOBOOST_TYPEOF_TOSEQ_2(z, n, _) (class)

// AUTOBOOST_TYPEOF_VIRTUAL

#define AUTOBOOST_TYPEOF_CAT_4(a, b, c, d) AUTOBOOST_TYPEOF_CAT_4_I(a, b, c, d)
#define AUTOBOOST_TYPEOF_CAT_4_I(a, b, c, d) a ## b ## c ## d

#define AUTOBOOST_TYPEOF_VIRTUAL(Fun, Obj)\
    AUTOBOOST_TYPEOF_CAT_4(AUTOBOOST_TYPEOF_, AUTOBOOST_PP_SEQ_HEAD(Obj), _, Fun)

// AUTOBOOST_TYPEOF_SEQ_ENUM[_TRAILING][_1]
// Two versions provided due to reentrancy issue

#define AUTOBOOST_TYPEOF_SEQ_EXPAND_ELEMENT(z,n,seq)\
   AUTOBOOST_PP_SEQ_ELEM(0,seq) (z,n,AUTOBOOST_PP_SEQ_ELEM(n,AUTOBOOST_PP_SEQ_ELEM(1,seq)))

#define AUTOBOOST_TYPEOF_SEQ_ENUM(seq,macro)\
    AUTOBOOST_PP_ENUM(AUTOBOOST_PP_SEQ_SIZE(seq),AUTOBOOST_TYPEOF_SEQ_EXPAND_ELEMENT,(macro)(seq))

#define AUTOBOOST_TYPEOF_SEQ_ENUM_TRAILING(seq,macro)\
    AUTOBOOST_PP_ENUM_TRAILING(AUTOBOOST_PP_SEQ_SIZE(seq),AUTOBOOST_TYPEOF_SEQ_EXPAND_ELEMENT,(macro)(seq))

#define AUTOBOOST_TYPEOF_SEQ_EXPAND_ELEMENT_1(z,n,seq)\
    AUTOBOOST_PP_SEQ_ELEM(0,seq) (z,n,AUTOBOOST_PP_SEQ_ELEM(n,AUTOBOOST_PP_SEQ_ELEM(1,seq)))

#define AUTOBOOST_TYPEOF_SEQ_ENUM_1(seq,macro)\
    AUTOBOOST_PP_ENUM(AUTOBOOST_PP_SEQ_SIZE(seq),AUTOBOOST_TYPEOF_SEQ_EXPAND_ELEMENT_1,(macro)(seq))

#define AUTOBOOST_TYPEOF_SEQ_ENUM_TRAILING_1(seq,macro)\
    AUTOBOOST_PP_ENUM_TRAILING(AUTOBOOST_PP_SEQ_SIZE(seq),AUTOBOOST_TYPEOF_SEQ_EXPAND_ELEMENT_1,(macro)(seq))

//

#define AUTOBOOST_TYPEOF_PLACEHOLDER(z, n, elem)\
    AUTOBOOST_TYPEOF_VIRTUAL(PLACEHOLDER, elem)(elem)

#define AUTOBOOST_TYPEOF_PLACEHOLDER_TYPES(z, n, elem)\
    AUTOBOOST_TYPEOF_VIRTUAL(PLACEHOLDER_TYPES, elem)(elem, n)

#define AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_ENCODE_PARAM(r, data, n, elem)\
    AUTOBOOST_TYPEOF_VIRTUAL(ENCODE, elem)(elem, n)

#define AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_DECODE_PARAM(r, data, n, elem)\
    AUTOBOOST_TYPEOF_VIRTUAL(DECODE, elem)(elem, n)

#define AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_PARAM_PAIR(z, n, elem) \
    AUTOBOOST_TYPEOF_VIRTUAL(EXPANDTYPE, elem)(elem) AUTOBOOST_PP_CAT(P, n)

#define AUTOBOOST_TYPEOF_REGISTER_DEFAULT_TEMPLATE_TYPE(Name,Params,ID)\
    Name< AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_SEQ_SIZE(Params), P) >

//Since we are creating an internal decode struct, we need to use different template names, T instead of P.
#define AUTOBOOST_TYPEOF_REGISTER_DECODER_TYPE_PARAM_PAIR(z,n,elem) \
    AUTOBOOST_TYPEOF_VIRTUAL(EXPANDTYPE, elem)(elem) AUTOBOOST_PP_CAT(T, n)

//Default template param decoding

#define AUTOBOOST_TYPEOF_TYPEDEF_DECODED_TEMPLATE_TYPE(Name,Params)\
    typedef Name<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_PP_SEQ_SIZE(Params),AUTOBOOST_TYPEOF_QUALIFY(P))> type;

//Branch the decoding
#define AUTOBOOST_TYPEOF_TYPEDEF_DECODED_TYPE(Name,Params)\
    AUTOBOOST_PP_IF(AUTOBOOST_TYPEOF_HAS_TEMPLATES(Params),\
        AUTOBOOST_TYPEOF_TYPEDEF_DECODED_TEMPLATE_TEMPLATE_TYPE,\
        AUTOBOOST_TYPEOF_TYPEDEF_DECODED_TEMPLATE_TYPE)(Name,Params)

#define AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_IMPL(Name, Params, Size, ID)\
    AUTOBOOST_TYPEOF_BEGIN_ENCODE_NS\
    AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_TEMPLATE_IMPL(Name, Params, ID)\
    template<class V\
        AUTOBOOST_TYPEOF_SEQ_ENUM_TRAILING(Params, AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_PARAM_PAIR)\
    >\
    struct encode_type_impl<V, Name<AUTOBOOST_PP_ENUM_PARAMS(Size, P)> >\
    {\
        typedef typename autoboost::type_of::push_back<V, autoboost::mpl::size_t<ID> >::type V0;\
        AUTOBOOST_PP_SEQ_FOR_EACH_I(AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_ENCODE_PARAM, ~, Params)\
        typedef AUTOBOOST_PP_CAT(V, Size) type;\
    };\
    template<class Iter>\
    struct decode_type_impl<autoboost::mpl::size_t<ID>, Iter>\
    {\
        typedef decode_type_impl<autoboost::mpl::size_t<ID>, Iter> self_t;\
        typedef autoboost::mpl::size_t<ID> self_id;\
        typedef Iter iter0;\
        AUTOBOOST_PP_SEQ_FOR_EACH_I(AUTOBOOST_TYPEOF_REGISTER_TEMPLATE_DECODE_PARAM, ~, Params)\
        AUTOBOOST_TYPEOF_TYPEDEF_DECODED_TYPE(Name, Params)\
        typedef AUTOBOOST_PP_CAT(iter, Size) iter;\
    };\
    AUTOBOOST_TYPEOF_END_ENCODE_NS

#endif//AUTOBOOST_TYPEOF_TEMPLATE_ENCODING_HPP_INCLUDED

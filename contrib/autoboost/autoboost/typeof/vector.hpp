// Copyright (C) 2005 Arkadiy Vertleyb
// Copyright (C) 2005 Peder Holt
//
// Copyright (C) 2006 Tobias Schwinger
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_VECTOR_HPP_INCLUDED

#include <autoboost/mpl/int.hpp>
#include <autoboost/preprocessor/iteration/self.hpp>

#ifndef AUTOBOOST_TYPEOF_LIMIT_SIZE
#   define AUTOBOOST_TYPEOF_LIMIT_SIZE 50
#endif

//
// To recreate the preprocessed versions of this file preprocess and run
//
//   $(AUTOBOOST_ROOT)/libs/typeof/tools/preprocess.pl
//

#if defined(AUTOBOOST_TYPEOF_PP_INCLUDE_EXTERNAL)

#   undef AUTOBOOST_TYPEOF_PP_INCLUDE_EXTERNAL

#elif !defined(AUTOBOOST_TYPEOF_PREPROCESSING_MODE) && !AUTOBOOST_PP_IS_SELFISH

#   define AUTOBOOST_PP_INDIRECT_SELF <autoboost/typeof/vector.hpp>
#   if   AUTOBOOST_TYPEOF_LIMIT_SIZE < 50
#     include AUTOBOOST_PP_INCLUDE_SELF()
#   elif AUTOBOOST_TYPEOF_LIMIT_SIZE < 100
#     include <autoboost/typeof/vector50.hpp>
#     define  AUTOBOOST_TYPEOF_PP_START_SIZE 51
#     include AUTOBOOST_PP_INCLUDE_SELF()
#   elif AUTOBOOST_TYPEOF_LIMIT_SIZE < 150
#     include <autoboost/typeof/vector100.hpp>
#     define  AUTOBOOST_TYPEOF_PP_START_SIZE 101
#     include AUTOBOOST_PP_INCLUDE_SELF()
#   elif AUTOBOOST_TYPEOF_LIMIT_SIZE < 200
#     include <autoboost/typeof/vector150.hpp>
#     define  AUTOBOOST_TYPEOF_PP_START_SIZE 151
#     include AUTOBOOST_PP_INCLUDE_SELF()
#   elif AUTOBOOST_TYPEOF_LIMIT_SIZE <= 250
#     include <autoboost/typeof/vector200.hpp>
#     define  AUTOBOOST_TYPEOF_PP_START_SIZE 201
#     include AUTOBOOST_PP_INCLUDE_SELF()
#   else
#     error "AUTOBOOST_TYPEOF_LIMIT_SIZE too high"
#   endif

#else// defined(AUTOBOOST_TYPEOF_PREPROCESSING_MODE) || AUTOBOOST_PP_IS_SELFISH

#   ifndef AUTOBOOST_TYPEOF_PP_NEXT_SIZE
#     define AUTOBOOST_TYPEOF_PP_NEXT_SIZE AUTOBOOST_TYPEOF_LIMIT_SIZE
#   endif
#   ifndef AUTOBOOST_TYPEOF_PP_START_SIZE
#     define AUTOBOOST_TYPEOF_PP_START_SIZE 0
#   endif

#   if AUTOBOOST_TYPEOF_PP_START_SIZE <= AUTOBOOST_TYPEOF_LIMIT_SIZE

#     include <autoboost/preprocessor/enum_params.hpp>
#     include <autoboost/preprocessor/repeat.hpp>
#     include <autoboost/preprocessor/repeat_from_to.hpp>
#     include <autoboost/preprocessor/cat.hpp>
#     include <autoboost/preprocessor/inc.hpp>
#     include <autoboost/preprocessor/dec.hpp>
#     include <autoboost/preprocessor/comma_if.hpp>
#     include <autoboost/preprocessor/iteration/local.hpp>
#     include <autoboost/preprocessor/control/expr_iif.hpp>
#     include <autoboost/preprocessor/logical/not.hpp>

// iterator

#     define AUTOBOOST_TYPEOF_spec_iter(n)\
        template<class V>\
        struct v_iter<V, mpl::int_<n> >\
        {\
            typedef typename V::item ## n type;\
            typedef v_iter<V, mpl::int_<n + 1> > next;\
        };

namespace autoboost { namespace type_of {

    template<class V, class Increase_AUTOBOOST_TYPEOF_LIMIT_SIZE> struct v_iter; // not defined
#     define  AUTOBOOST_PP_LOCAL_MACRO  AUTOBOOST_TYPEOF_spec_iter
#     define  AUTOBOOST_PP_LOCAL_LIMITS \
        (AUTOBOOST_PP_DEC(AUTOBOOST_TYPEOF_PP_START_SIZE), \
         AUTOBOOST_PP_DEC(AUTOBOOST_TYPEOF_LIMIT_SIZE))
#     include AUTOBOOST_PP_LOCAL_ITERATE()

}}

#     undef AUTOBOOST_TYPEOF_spec_iter

// vector

#     define AUTOBOOST_TYPEOF_typedef_item(z, n, _)\
        typedef P ## n item ## n;

#     define AUTOBOOST_TYPEOF_typedef_fake_item(z, n, _)\
        typedef mpl::int_<1> item ## n;

#     define AUTOBOOST_TYPEOF_define_vector(n)\
        template<AUTOBOOST_PP_ENUM_PARAMS(n, class P) AUTOBOOST_PP_EXPR_IIF(AUTOBOOST_PP_NOT(n), class T = void)>\
        struct vector ## n\
        {\
            typedef v_iter<vector ## n<AUTOBOOST_PP_ENUM_PARAMS(n,P)>, autoboost::mpl::int_<0> > begin;\
            AUTOBOOST_PP_REPEAT(n, AUTOBOOST_TYPEOF_typedef_item, ~)\
            AUTOBOOST_PP_REPEAT_FROM_TO(n, AUTOBOOST_TYPEOF_PP_NEXT_SIZE, AUTOBOOST_TYPEOF_typedef_fake_item, ~)\
        };

namespace autoboost { namespace type_of {

#     define  AUTOBOOST_PP_LOCAL_MACRO  AUTOBOOST_TYPEOF_define_vector
#     define  AUTOBOOST_PP_LOCAL_LIMITS \
        (AUTOBOOST_TYPEOF_PP_START_SIZE,AUTOBOOST_TYPEOF_LIMIT_SIZE)
#     include AUTOBOOST_PP_LOCAL_ITERATE()

}}

#     undef AUTOBOOST_TYPEOF_typedef_item
#     undef AUTOBOOST_TYPEOF_typedef_fake_item
#     undef AUTOBOOST_TYPEOF_define_vector

// push_back

#     define AUTOBOOST_TYPEOF_spec_push_back(n)\
        template<AUTOBOOST_PP_ENUM_PARAMS(n, class P) AUTOBOOST_PP_COMMA_IF(n) class T>\
        struct push_back<AUTOBOOST_PP_CAT(autoboost::type_of::vector, n)<AUTOBOOST_PP_ENUM_PARAMS(n, P)>, T>\
        {\
            typedef AUTOBOOST_PP_CAT(autoboost::type_of::vector, AUTOBOOST_PP_INC(n))<\
                AUTOBOOST_PP_ENUM_PARAMS(n, P) AUTOBOOST_PP_COMMA_IF(n) T\
            > type;\
        };

namespace autoboost { namespace type_of {

#   if   AUTOBOOST_TYPEOF_LIMIT_SIZE < 50
    template<class V, class T> struct push_back {
        typedef V type;
    };
#   endif
    //default behaviour is to let push_back ignore T, and return the input vector.
    //This is to let AUTOBOOST_TYPEOF_NESTED_TYPEDEF work properly with the default vector.
#     define  AUTOBOOST_PP_LOCAL_MACRO  AUTOBOOST_TYPEOF_spec_push_back
#     define  AUTOBOOST_PP_LOCAL_LIMITS \
        (AUTOBOOST_PP_DEC(AUTOBOOST_TYPEOF_PP_START_SIZE), \
         AUTOBOOST_PP_DEC(AUTOBOOST_TYPEOF_LIMIT_SIZE))
#     include AUTOBOOST_PP_LOCAL_ITERATE()

}}

#     undef AUTOBOOST_TYPEOF_spec_push_back

#   endif//AUTOBOOST_TYPEOF_PP_START_SIZE<=AUTOBOOST_TYPEOF_LIMIT_SIZE
#   undef  AUTOBOOST_TYPEOF_PP_START_SIZE
#   undef  AUTOBOOST_TYPEOF_PP_NEXT_SIZE

#endif//AUTOBOOST_TYPEOF_PREPROCESSING_MODE || AUTOBOOST_PP_IS_SELFISH

#define AUTOBOOST_TYPEOF_VECTOR_HPP_INCLUDED
#endif//AUTOBOOST_TYPEOF_VECTOR_HPP_INCLUDED


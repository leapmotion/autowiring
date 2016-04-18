
// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION

#if defined(AUTOBOOST_PP_IS_ITERATING)

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/preprocessor/enum_params.hpp>
#include <autoboost/preprocessor/enum_shifted_params.hpp>
#include <autoboost/preprocessor/comma_if.hpp>
#include <autoboost/preprocessor/repeat.hpp>
#include <autoboost/preprocessor/dec.hpp>
#include <autoboost/preprocessor/cat.hpp>

#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

#if defined(AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES)

#   define AUX778076_VECTOR_TAIL(vector, i_, C) \
    AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(vector,i_),_c)<T \
          AUTOBOOST_PP_COMMA_IF(i_) AUTOBOOST_PP_ENUM_PARAMS(i_, C) \
        > \
    /**/

#if i_ > 0
template<
      typename T
    , AUTOBOOST_PP_ENUM_PARAMS(i_, T C)
    >
struct AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(vector,i_),_c)
    : v_item<
          integral_c<T,AUTOBOOST_PP_CAT(C,AUTOBOOST_PP_DEC(i_))>
        , AUX778076_VECTOR_TAIL(vector,AUTOBOOST_PP_DEC(i_),C)
        >
{
    typedef AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(vector,i_),_c) type;
    typedef T value_type;
};
#endif

#   undef AUX778076_VECTOR_TAIL

#else // "brute force" implementation

#   define AUX778076_VECTOR_C_PARAM_FUNC(unused, i_, param) \
    AUTOBOOST_PP_COMMA_IF(i_) \
    integral_c<T,AUTOBOOST_PP_CAT(param,i_)> \
    /**/

template<
      typename T
    , AUTOBOOST_PP_ENUM_PARAMS(i_, T C)
    >
struct AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(vector,i_),_c)
    : AUTOBOOST_PP_CAT(vector,i_)< AUTOBOOST_PP_REPEAT(i_,AUX778076_VECTOR_C_PARAM_FUNC,C) >
{
    typedef AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(vector,i_),_c) type;
    typedef T value_type;
};

#   undef AUX778076_VECTOR_C_PARAM_FUNC

#endif // AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES

#undef i_

#endif // AUTOBOOST_PP_IS_ITERATING

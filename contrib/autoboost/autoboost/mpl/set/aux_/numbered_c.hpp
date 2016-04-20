
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

#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <autoboost/preprocessor/dec.hpp>
#include <autoboost/preprocessor/cat.hpp>

#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

#   define AUX778076_SET_C_TAIL(set, i_, T, C) \
    AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(set,i_),_c)< \
          T AUTOBOOST_PP_ENUM_TRAILING_PARAMS(i_, C) \
        > \
    /**/

template<
      typename T
    , AUTOBOOST_PP_ENUM_PARAMS(i_, T C)
    >
struct AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(set,i_),_c)
    : s_item<
          integral_c<T,AUTOBOOST_PP_CAT(C,AUTOBOOST_PP_DEC(i_))>
        , AUX778076_SET_C_TAIL(set,AUTOBOOST_PP_DEC(i_), T, C)
        >
{
    typedef AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(set,i_),_c) type;
};

#   undef AUX778076_SET_C_TAIL

#undef i_

#endif // AUTOBOOST_PP_IS_ITERATING

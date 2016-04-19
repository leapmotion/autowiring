
#ifndef AUTOBOOST_MPL_AUX_PREPROCESSOR_RANGE_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_PREPROCESSOR_RANGE_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/preprocessor/seq/subseq.hpp>
#include <autoboost/preprocessor/repetition/repeat.hpp>
#include <autoboost/preprocessor/arithmetic/add.hpp>

#define AUTOBOOST_MPL_PP_RANGE_ITEM(z,n,_) (n)

#define AUTOBOOST_MPL_PP_RANGE(first, length) \
    AUTOBOOST_PP_SEQ_SUBSEQ( \
        AUTOBOOST_PP_REPEAT(AUTOBOOST_PP_ADD(first,length), AUTOBOOST_MPL_PP_RANGE_ITEM, _), \
        first, length \
    ) \
/**/

#endif // AUTOBOOST_MPL_AUX_PREPROCESSOR_RANGE_HPP_INCLUDED

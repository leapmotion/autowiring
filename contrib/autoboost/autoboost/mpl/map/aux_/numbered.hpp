
// NO INCLUDE GUARDS, THE HEADER IS INTENDED FOR MULTIPLE INCLUSION

#if !defined(AUTOBOOST_PP_IS_ITERATING)

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

#else

#include <autoboost/mpl/aux_/config/typeof.hpp>
#include <autoboost/mpl/aux_/config/ctps.hpp>
#include <autoboost/preprocessor/enum_params.hpp>
#include <autoboost/preprocessor/dec.hpp>
#include <autoboost/preprocessor/cat.hpp>

#define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

#   define AUX778076_MAP_TAIL(map, i_, P) \
    AUTOBOOST_PP_CAT(map,i_)< \
          AUTOBOOST_PP_ENUM_PARAMS(i_, P) \
        > \
    /**/


#if defined(AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES)

template<
      AUTOBOOST_PP_ENUM_PARAMS(i_, typename P)
    >
struct AUTOBOOST_PP_CAT(map,i_)
    : m_item<
          typename AUTOBOOST_PP_CAT(P,AUTOBOOST_PP_DEC(i_))::first
        , typename AUTOBOOST_PP_CAT(P,AUTOBOOST_PP_DEC(i_))::second
        , AUX778076_MAP_TAIL(map,AUTOBOOST_PP_DEC(i_),P)
        >
{
    typedef AUTOBOOST_PP_CAT(map,i_) type;
};

#else // "brute force" implementation

#   if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template< typename Map>
struct m_at<Map,AUTOBOOST_PP_DEC(i_)>
{
    typedef typename Map::AUTOBOOST_PP_CAT(item,AUTOBOOST_PP_DEC(i_)) type;
};

template< typename Key, typename T, typename Base >
struct m_item<i_,Key,T,Base>
    : m_item_<Key,T,Base>
{
    typedef pair<Key,T> AUTOBOOST_PP_CAT(item,AUTOBOOST_PP_DEC(i_));
};

#   else

template<>
struct m_at_impl<AUTOBOOST_PP_DEC(i_)>
{
    template< typename Map > struct result_
    {
        typedef typename Map::AUTOBOOST_PP_CAT(item,AUTOBOOST_PP_DEC(i_)) type;
    };
};

template<>
struct m_item_impl<i_>
{
    template< typename Key, typename T, typename Base > struct result_
        : m_item_<Key,T,Base>
    {
        typedef pair<Key,T> AUTOBOOST_PP_CAT(item,AUTOBOOST_PP_DEC(i_));
    };
};

#   endif

template<
      AUTOBOOST_PP_ENUM_PARAMS(i_, typename P)
    >
struct AUTOBOOST_PP_CAT(map,i_)
    : m_item<
          i_
        , typename AUTOBOOST_PP_CAT(P,AUTOBOOST_PP_DEC(i_))::first
        , typename AUTOBOOST_PP_CAT(P,AUTOBOOST_PP_DEC(i_))::second
        , AUX778076_MAP_TAIL(map,AUTOBOOST_PP_DEC(i_),P)
        >
{
    typedef AUTOBOOST_PP_CAT(map,i_) type;
};

#endif // AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES

#   undef AUX778076_MAP_TAIL

#undef i_

#endif // AUTOBOOST_PP_IS_ITERATING

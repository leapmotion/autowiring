
// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

// Preprocessed version of "autoboost/mpl/or.hpp" header
// -- DO NOT modify by hand!

namespace autoboost { namespace mpl {

namespace aux {
template< bool C_ > struct or_impl
{
    template<
          typename T1, typename T2, typename T3, typename T4
        >
    struct result_
        : true_
    {
    };
};

template<> struct or_impl<false>
{
    template<
          typename T1, typename T2, typename T3, typename T4
        >
    struct result_
        : or_impl<
              AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND(T1)::value
            >::template result_< T2,T3,T4,false_ >
    {
    };

    template<> struct result_< false_,false_,false_,false_ >
        : false_
    {
    };
};

} // namespace aux

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(T1)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(T2)
    , typename T3 = false_, typename T4 = false_, typename T5 = false_
    >
struct or_

    : aux::or_impl<
          AUTOBOOST_MPL_AUX_NESTED_TYPE_WKND(T1)::value
        >::template result_< T2,T3,T4,T5 >

{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(
          5
        , or_
        , ( T1, T2, T3, T4, T5)
        )
};

AUTOBOOST_MPL_AUX_NA_SPEC2(
      2
    , 5
    , or_
    )

}}

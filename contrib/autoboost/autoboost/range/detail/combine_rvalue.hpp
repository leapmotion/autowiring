// Boost.Range library
//
//  Copyright Neil Groves 2014. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#define AUTOBOOST_RANGE_combined_args(z, n, i) \
    AUTOBOOST_PP_CAT(R, n)&& AUTOBOOST_PP_CAT(r, n)

#define AUTOBOOST_RANGE_combined_range_iterator(z, n, i) \
    typename range_iterator< \
        typename remove_reference<AUTOBOOST_PP_CAT(R,n)>::type \
  >::type


#define AUTOBOOST_RANGE_combine(z, n, data) \
    template <AUTOBOOST_PP_ENUM_PARAMS(n, typename R)> \
    inline range::combined_range< \
        tuple<AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_range_iterator, ~)> \
    > \
    combine(AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_args, ~)) \
    { \
        typedef tuple< \
            AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_range_iterator, ~) \
        > rng_tuple_t; \
        return range::combined_range<rng_tuple_t>( \
            rng_tuple_t(AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_seq, begin)), \
            rng_tuple_t(AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_seq, end))); \
    }

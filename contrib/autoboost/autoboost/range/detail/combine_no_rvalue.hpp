// Boost.Range library
//
//  Copyright Neil Groves 2014. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#define AUTOBOOST_RANGE_combined_exp_pred(d, data) AUTOBOOST_PP_TUPLE_ELEM(3, 0, data)

#define AUTOBOOST_RANGE_combined_exp_op(d, data) \
 ( \
    AUTOBOOST_PP_DEC( \
       AUTOBOOST_PP_TUPLE_ELEM(3, 0, data) \
    ), \
    AUTOBOOST_PP_TUPLE_ELEM(3, 1, data), \
    AUTOBOOST_PP_MUL_D( \
       d, \
       AUTOBOOST_PP_TUPLE_ELEM(3, 2, data), \
       AUTOBOOST_PP_TUPLE_ELEM(3, 1, data) \
    ) \
 )

#define AUTOBOOST_RANGE_combined_exp(x, n) \
  AUTOBOOST_PP_TUPLE_ELEM(3, 2, \
  AUTOBOOST_PP_WHILE(AUTOBOOST_RANGE_combined_exp_pred, \
                 AUTOBOOST_RANGE_combined_exp_op, (n, x, 1)))

#define AUTOBOOST_RANGE_combined_bitset_pred(n, state) \
    AUTOBOOST_PP_TUPLE_ELEM(2,1,state)

#define AUTOBOOST_RANGE_combined_bitset_op(d, state) \
    (AUTOBOOST_PP_DIV_D(d, AUTOBOOST_PP_TUPLE_ELEM(2,0,state), 2), \
     AUTOBOOST_PP_DEC(AUTOBOOST_PP_TUPLE_ELEM(2,1,state)))

#define AUTOBOOST_RANGE_combined_bitset(i, n) \
AUTOBOOST_PP_MOD(AUTOBOOST_PP_TUPLE_ELEM(2, 0, \
      AUTOBOOST_PP_WHILE(AUTOBOOST_RANGE_combined_bitset_pred, \
                     AUTOBOOST_RANGE_combined_bitset_op, (i,n))), 2)

#define AUTOBOOST_RANGE_combined_range_iterator(z, n, i) \
  typename range_iterator< \
      AUTOBOOST_PP_CAT(R,n)          \
      AUTOBOOST_PP_IF( \
          AUTOBOOST_RANGE_combined_bitset(i,n), \
          AUTOBOOST_PP_IDENTITY(const), \
          AUTOBOOST_PP_EMPTY)() \
  >::type

#define AUTOBOOST_RANGE_combined_args(z, n, i) \
  AUTOBOOST_PP_CAT(R, n) \
  AUTOBOOST_PP_IF(AUTOBOOST_RANGE_combined_bitset(i,n), const&, &)  \
  AUTOBOOST_PP_CAT(r, n)

#define AUTOBOOST_RANGE_combine_impl(z, i, n)\
    template<AUTOBOOST_PP_ENUM_PARAMS(n, typename R)> \
    inline range::combined_range< \
        autoboost::tuple<AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_range_iterator, i)> \
    > \
    combine(AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_args, i)) \
    { \
        typedef tuple< \
            AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_range_iterator, i) \
        > rng_tuple_t;   \
        return range::combined_range<rng_tuple_t>( \
            rng_tuple_t(AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_seq, begin)), \
            rng_tuple_t(AUTOBOOST_PP_ENUM(n, AUTOBOOST_RANGE_combined_seq, end))); \
    }


#define AUTOBOOST_RANGE_combine(z, n, data) \
  AUTOBOOST_PP_REPEAT(AUTOBOOST_RANGE_combined_exp(2,n), AUTOBOOST_RANGE_combine_impl, n)

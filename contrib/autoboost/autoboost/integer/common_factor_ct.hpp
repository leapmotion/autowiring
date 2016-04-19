//  Boost common_factor_ct.hpp header file  ----------------------------------//

//  (C) Copyright Daryle Walker and Stephen Cleary 2001-2002.
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef AUTOBOOST_INTEGER_COMMON_FACTOR_CT_HPP
#define AUTOBOOST_INTEGER_COMMON_FACTOR_CT_HPP

#include <autoboost/integer_fwd.hpp>  // self include
#include <autoboost/config.hpp>  // for AUTOBOOST_STATIC_CONSTANT, etc.

namespace autoboost
{
namespace integer
{

//  Implementation details  --------------------------------------------------//

namespace detail
{
    // Build GCD with Euclid's recursive algorithm
    template < static_gcd_type Value1, static_gcd_type Value2 >
    struct static_gcd_helper_t
    {
    private:
        AUTOBOOST_STATIC_CONSTANT( static_gcd_type, new_value1 = Value2 );
        AUTOBOOST_STATIC_CONSTANT( static_gcd_type, new_value2 = Value1 % Value2 );

        #ifndef __BORLANDC__
        #define AUTOBOOST_DETAIL_GCD_HELPER_VAL(Value) static_cast<static_gcd_type>(Value)
        #else
        typedef static_gcd_helper_t  self_type;
        #define AUTOBOOST_DETAIL_GCD_HELPER_VAL(Value)  (self_type:: Value )
        #endif

        typedef static_gcd_helper_t< AUTOBOOST_DETAIL_GCD_HELPER_VAL(new_value1),
         AUTOBOOST_DETAIL_GCD_HELPER_VAL(new_value2) >  next_step_type;

        #undef AUTOBOOST_DETAIL_GCD_HELPER_VAL

    public:
        AUTOBOOST_STATIC_CONSTANT( static_gcd_type, value = next_step_type::value );
    };

    // Non-recursive case
    template < static_gcd_type Value1 >
    struct static_gcd_helper_t< Value1, 0UL >
    {
        AUTOBOOST_STATIC_CONSTANT( static_gcd_type, value = Value1 );
    };

    // Build the LCM from the GCD
    template < static_gcd_type Value1, static_gcd_type Value2 >
    struct static_lcm_helper_t
    {
        typedef static_gcd_helper_t<Value1, Value2>  gcd_type;

        AUTOBOOST_STATIC_CONSTANT( static_gcd_type, value = Value1 / gcd_type::value
         * Value2 );
    };

    // Special case for zero-GCD values
    template < >
    struct static_lcm_helper_t< 0UL, 0UL >
    {
        AUTOBOOST_STATIC_CONSTANT( static_gcd_type, value = 0UL );
    };

}  // namespace detail


//  Compile-time greatest common divisor evaluator class declaration  --------//

template < static_gcd_type Value1, static_gcd_type Value2 > struct static_gcd
{
    AUTOBOOST_STATIC_CONSTANT( static_gcd_type, value = (detail::static_gcd_helper_t<Value1, Value2>::value) );
};  // autoboost::integer::static_gcd

#if !defined(AUTOBOOST_NO_INCLASS_MEMBER_INITIALIZATION)
template< static_gcd_type Value1, static_gcd_type Value2 > static_gcd_type const static_gcd< Value1, Value2 >::value;
#endif

//  Compile-time least common multiple evaluator class declaration  ----------//

template < static_gcd_type Value1, static_gcd_type Value2 > struct static_lcm
{
    AUTOBOOST_STATIC_CONSTANT( static_gcd_type, value = (detail::static_lcm_helper_t<Value1, Value2>::value) );
};  // autoboost::integer::static_lcm

#if !defined(AUTOBOOST_NO_INCLASS_MEMBER_INITIALIZATION)
template< static_gcd_type Value1, static_gcd_type Value2 > static_gcd_type const static_lcm< Value1, Value2 >::value;
#endif

}  // namespace integer
}  // namespace autoboost


#endif  // AUTOBOOST_INTEGER_COMMON_FACTOR_CT_HPP

//
//! Copyright (c) 2011
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef AUTOBOOST_NUMERIC_CAST_TRAITS_HPP
#define AUTOBOOST_NUMERIC_CAST_TRAITS_HPP

#include <autoboost/numeric/conversion/converter_policies.hpp>

namespace autoboost { namespace numeric {

    template <typename Target, typename Source, typename EnableIf = void>
    struct numeric_cast_traits
    {
        typedef def_overflow_handler    overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<Source>           rounding_policy;
    };

}}//namespace autoboost::numeric;

#if !defined( AUTOBOOST_NUMERIC_CONVERSION_RELAX_BUILT_IN_CAST_TRAITS )
#include <autoboost/cstdint.hpp>
#include <autoboost/numeric/conversion/detail/numeric_cast_traits.hpp>
#endif//!defined AUTOBOOST_NUMERIC_CONVERSION_RELAX_BUILT_IN_CAST_TRAITS

#endif//AUTOBOOST_NUMERIC_CAST_TRAITS_HPP

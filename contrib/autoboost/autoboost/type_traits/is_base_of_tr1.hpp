
//  (C) Copyright Rani Sharoni 2003-2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_BASE_OF_TR1_HPP_INCLUDED
#define AUTOBOOST_TT_IS_BASE_OF_TR1_HPP_INCLUDED

#include <autoboost/type_traits/is_base_and_derived.hpp>
#include <autoboost/type_traits/is_same.hpp>
#include <autoboost/type_traits/is_class.hpp>
#include <autoboost/type_traits/detail/ice_or.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost { namespace tr1{

   namespace detail{
      template <class B, class D>
      struct is_base_of_imp
      {
          typedef typename remove_cv<B>::type ncvB;
          typedef typename remove_cv<D>::type ncvD;
          AUTOBOOST_STATIC_CONSTANT(bool, value = (::autoboost::type_traits::ice_or<
            (::autoboost::detail::is_base_and_derived_impl<ncvB,ncvD>::value),
            (::autoboost::is_same<ncvB,ncvD>::value)>::value));
      };
   }

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF2(
      is_base_of
    , Base
    , Derived
    , (::autoboost::tr1::detail::is_base_of_imp<Base, Derived>::value))

AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_base_of,Base&,Derived,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_base_of,Base,Derived&,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_base_of,Base&,Derived&,false)

} } // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_BASE_OF_TR1_HPP_INCLUDED

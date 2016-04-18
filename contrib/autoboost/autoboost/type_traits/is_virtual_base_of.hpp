//  (C) Copyright Daniel Frey and Robert Ramey 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_VIRTUAL_BASE_OF_HPP_INCLUDED
#define AUTOBOOST_TT_IS_VIRTUAL_BASE_OF_HPP_INCLUDED

#include <autoboost/type_traits/is_base_of.hpp>
#include <autoboost/type_traits/is_same.hpp>
#include <autoboost/mpl/and.hpp>
#include <autoboost/mpl/not.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {
namespace detail {


#ifdef AUTOBOOST_MSVC
#pragma warning( push )
#pragma warning( disable : 4584 4250)
#elif defined(__GNUC__) && (__GNUC__ >= 4)
#pragma GCC system_header
#endif

template<typename Base, typename Derived, typename tag>
struct is_virtual_base_of_impl
{
    AUTOBOOST_STATIC_CONSTANT(bool, value = false);
};

template<typename Base, typename Derived>
struct is_virtual_base_of_impl<Base, Derived, mpl::true_>
{
#ifdef __BORLANDC__
    struct autoboost_type_traits_internal_struct_X : public virtual Derived, public virtual Base
    {
       autoboost_type_traits_internal_struct_X();
       autoboost_type_traits_internal_struct_X(const autoboost_type_traits_internal_struct_X&);
       autoboost_type_traits_internal_struct_X& operator=(const autoboost_type_traits_internal_struct_X&);
       ~autoboost_type_traits_internal_struct_X()throw();
    };
    struct autoboost_type_traits_internal_struct_Y : public virtual Derived
    {
       autoboost_type_traits_internal_struct_Y();
       autoboost_type_traits_internal_struct_Y(const autoboost_type_traits_internal_struct_Y&);
       autoboost_type_traits_internal_struct_Y& operator=(const autoboost_type_traits_internal_struct_Y&);
       ~autoboost_type_traits_internal_struct_Y()throw();
    };
#else
    struct autoboost_type_traits_internal_struct_X : public Derived, virtual Base
    {
       autoboost_type_traits_internal_struct_X();
       autoboost_type_traits_internal_struct_X(const autoboost_type_traits_internal_struct_X&);
       autoboost_type_traits_internal_struct_X& operator=(const autoboost_type_traits_internal_struct_X&);
       ~autoboost_type_traits_internal_struct_X()throw();
    };
    struct autoboost_type_traits_internal_struct_Y : public Derived
    {
       autoboost_type_traits_internal_struct_Y();
       autoboost_type_traits_internal_struct_Y(const autoboost_type_traits_internal_struct_Y&);
       autoboost_type_traits_internal_struct_Y& operator=(const autoboost_type_traits_internal_struct_Y&);
       ~autoboost_type_traits_internal_struct_Y()throw();
    };
#endif
    AUTOBOOST_STATIC_CONSTANT(bool, value = (sizeof(autoboost_type_traits_internal_struct_X)==sizeof(autoboost_type_traits_internal_struct_Y)));
};

template<typename Base, typename Derived>
struct is_virtual_base_of_impl2
{
   typedef typename mpl::and_<is_base_of<Base, Derived>, mpl::not_<is_same<Base, Derived> > >::type tag_type;
   typedef is_virtual_base_of_impl<Base, Derived, tag_type> imp;
   AUTOBOOST_STATIC_CONSTANT(bool, value = imp::value);
};

#ifdef AUTOBOOST_MSVC
#pragma warning( pop )
#endif

} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF2(
      is_virtual_base_of
       , Base
       , Derived
       , (::autoboost::detail::is_virtual_base_of_impl2<Base,Derived>::value)
)

AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_virtual_base_of,Base&,Derived,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_virtual_base_of,Base,Derived&,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_virtual_base_of,Base&,Derived&,false)

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif

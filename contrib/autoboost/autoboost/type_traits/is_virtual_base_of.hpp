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
struct is_virtual_base_of_impl<Base, Derived, true_type>
{
   union max_align
   {
      unsigned u;
      unsigned long ul;
      void* v;
      double d;
      long double ld;
#ifndef AUTOBOOST_NO_LONG_LONG
      long long ll;
#endif
   };
#ifdef __BORLANDC__
    struct autoboost_type_traits_internal_struct_X : public virtual Derived, public virtual Base
    {
       autoboost_type_traits_internal_struct_X();
       autoboost_type_traits_internal_struct_X(const autoboost_type_traits_internal_struct_X&);
       autoboost_type_traits_internal_struct_X& operator=(const autoboost_type_traits_internal_struct_X&);
       ~autoboost_type_traits_internal_struct_X()throw();
       max_align data[4];
    };
    struct autoboost_type_traits_internal_struct_Y : public virtual Derived
    {
       autoboost_type_traits_internal_struct_Y();
       autoboost_type_traits_internal_struct_Y(const autoboost_type_traits_internal_struct_Y&);
       autoboost_type_traits_internal_struct_Y& operator=(const autoboost_type_traits_internal_struct_Y&);
       ~autoboost_type_traits_internal_struct_Y()throw();
       max_align data[4];
    };
#else
    struct autoboost_type_traits_internal_struct_X : public Derived, virtual Base
    {
       autoboost_type_traits_internal_struct_X();
       autoboost_type_traits_internal_struct_X(const autoboost_type_traits_internal_struct_X&);
       autoboost_type_traits_internal_struct_X& operator=(const autoboost_type_traits_internal_struct_X&);
       ~autoboost_type_traits_internal_struct_X()throw();
       max_align data[16];
    };
    struct autoboost_type_traits_internal_struct_Y : public Derived
    {
       autoboost_type_traits_internal_struct_Y();
       autoboost_type_traits_internal_struct_Y(const autoboost_type_traits_internal_struct_Y&);
       autoboost_type_traits_internal_struct_Y& operator=(const autoboost_type_traits_internal_struct_Y&);
       ~autoboost_type_traits_internal_struct_Y()throw();
       max_align data[16];
    };
#endif
    AUTOBOOST_STATIC_CONSTANT(bool, value = (sizeof(autoboost_type_traits_internal_struct_X)==sizeof(autoboost_type_traits_internal_struct_Y)));
};

template<typename Base, typename Derived>
struct is_virtual_base_of_impl2
{
   typedef autoboost::integral_constant<bool, (autoboost::is_base_of<Base, Derived>::value && ! autoboost::is_same<Base, Derived>::value)> tag_type;
   typedef is_virtual_base_of_impl<Base, Derived, tag_type> imp;
   AUTOBOOST_STATIC_CONSTANT(bool, value = imp::value);
};

#ifdef AUTOBOOST_MSVC
#pragma warning( pop )
#endif

} // namespace detail

template <class Base, class Derived> struct is_virtual_base_of : public integral_constant<bool, (::autoboost::detail::is_virtual_base_of_impl2<Base, Derived>::value)>{};

template <class Base, class Derived> struct is_virtual_base_of<Base&, Derived> : public false_type{};
template <class Base, class Derived> struct is_virtual_base_of<Base, Derived&> : public false_type{};
template <class Base, class Derived> struct is_virtual_base_of<Base&, Derived&> : public false_type{};

} // namespace autoboost

#endif

// Copyright David Abrahams 2003. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef IS_LVALUE_ITERATOR_DWA2003112_HPP
# define IS_LVALUE_ITERATOR_DWA2003112_HPP

#include <autoboost/iterator.hpp>

#include <autoboost/detail/workaround.hpp>
#include <autoboost/detail/iterator.hpp>

#include <autoboost/type_traits/add_lvalue_reference.hpp>
#include <autoboost/iterator/detail/any_conversion_eater.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

// should be the last #includes
#include <autoboost/type_traits/integral_constant.hpp>
#include <autoboost/iterator/detail/config_def.hpp>

#ifndef AUTOBOOST_NO_IS_CONVERTIBLE

namespace autoboost {

namespace iterators {

namespace detail
{
#ifndef AUTOBOOST_NO_LVALUE_RETURN_DETECTION
  // Calling lvalue_preserver( <expression>, 0 ) returns a reference
  // to the expression's result if <expression> is an lvalue, or
  // not_an_lvalue() otherwise.
  struct not_an_lvalue {};

  template <class T>
  T& lvalue_preserver(T&, int);

  template <class U>
  not_an_lvalue lvalue_preserver(U const&, ...);

# define AUTOBOOST_LVALUE_PRESERVER(expr) detail::lvalue_preserver(expr,0)

#else

# define AUTOBOOST_LVALUE_PRESERVER(expr) expr

#endif

  // Guts of is_lvalue_iterator.  Value is the iterator's value_type
  // and the result is computed in the nested rebind template.
  template <class Value>
  struct is_lvalue_iterator_impl
  {
      // Eat implicit conversions so we don't report true for things
      // convertible to Value const&
      struct conversion_eater
      {
          conversion_eater(typename add_lvalue_reference<Value>::type);
      };

      static char tester(conversion_eater, int);
      static char (& tester(any_conversion_eater, ...) )[2];

      template <class It>
      struct rebind
      {
          static It& x;

          AUTOBOOST_STATIC_CONSTANT(
              bool
            , value = (
                sizeof(
                    is_lvalue_iterator_impl<Value>::tester(
                        AUTOBOOST_LVALUE_PRESERVER(*x), 0
                    )
                ) == 1
            )
          );
      };
  };

#undef AUTOBOOST_LVALUE_PRESERVER

  //
  // void specializations to handle std input and output iterators
  //
  template <>
  struct is_lvalue_iterator_impl<void>
  {
      template <class It>
      struct rebind : autoboost::mpl::false_
      {};
  };

#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
  template <>
  struct is_lvalue_iterator_impl<const void>
  {
      template <class It>
      struct rebind : autoboost::mpl::false_
      {};
  };

  template <>
  struct is_lvalue_iterator_impl<volatile void>
  {
      template <class It>
      struct rebind : autoboost::mpl::false_
      {};
  };

  template <>
  struct is_lvalue_iterator_impl<const volatile void>
  {
      template <class It>
      struct rebind : autoboost::mpl::false_
      {};
  };
#endif

  //
  // This level of dispatching is required for Borland.  We might save
  // an instantiation by removing it for others.
  //
  template <class It>
  struct is_readable_lvalue_iterator_impl
    : is_lvalue_iterator_impl<
          AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::iterator_traits<It>::value_type const
      >::template rebind<It>
  {};

  template <class It>
  struct is_non_const_lvalue_iterator_impl
    : is_lvalue_iterator_impl<
          AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::iterator_traits<It>::value_type
      >::template rebind<It>
  {};
} // namespace detail

template< typename T > struct is_lvalue_iterator
: public ::autoboost::integral_constant<bool,::autoboost::iterators::detail::is_readable_lvalue_iterator_impl<T>::value>
{
public:
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,is_lvalue_iterator,(T))
};

template< typename T > struct is_non_const_lvalue_iterator
: public ::autoboost::integral_constant<bool,::autoboost::iterators::detail::is_non_const_lvalue_iterator_impl<T>::value>
{
public:
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,is_non_const_lvalue_iterator,(T))
};

} // namespace iterators

using iterators::is_lvalue_iterator;
using iterators::is_non_const_lvalue_iterator;

} // namespace autoboost

#endif

#include <autoboost/iterator/detail/config_undef.hpp>

#endif // IS_LVALUE_ITERATOR_DWA2003112_HPP

// Copyright David Abrahams 2003. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef IS_READABLE_ITERATOR_DWA2003112_HPP
# define IS_READABLE_ITERATOR_DWA2003112_HPP

#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>
#include <autoboost/detail/iterator.hpp>
#include <autoboost/type_traits/add_lvalue_reference.hpp>

#include <autoboost/iterator/detail/any_conversion_eater.hpp>

// should be the last #include
#include <autoboost/type_traits/integral_constant.hpp>
#include <autoboost/iterator/detail/config_def.hpp>

#ifndef AUTOBOOST_NO_IS_CONVERTIBLE

namespace autoboost {

namespace iterators {

namespace detail
{
  // Guts of is_readable_iterator.  Value is the iterator's value_type
  // and the result is computed in the nested rebind template.
  template <class Value>
  struct is_readable_iterator_impl
  {
      static char tester(typename add_lvalue_reference<Value>::type, int);
      static char (& tester(any_conversion_eater, ...) )[2];

      template <class It>
      struct rebind
      {
          static It& x;

          AUTOBOOST_STATIC_CONSTANT(
              bool
            , value = (
                sizeof(
                    is_readable_iterator_impl<Value>::tester(*x, 1)
                ) == 1
            )
          );
      };
  };

#undef AUTOBOOST_READABLE_PRESERVER

  //
  // void specializations to handle std input and output iterators
  //
  template <>
  struct is_readable_iterator_impl<void>
  {
      template <class It>
      struct rebind : autoboost::mpl::false_
      {};
  };

#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
  template <>
  struct is_readable_iterator_impl<const void>
  {
      template <class It>
      struct rebind : autoboost::mpl::false_
      {};
  };

  template <>
  struct is_readable_iterator_impl<volatile void>
  {
      template <class It>
      struct rebind : autoboost::mpl::false_
      {};
  };

  template <>
  struct is_readable_iterator_impl<const volatile void>
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
  struct is_readable_iterator_impl2
    : is_readable_iterator_impl<
          AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::iterator_traits<It>::value_type const
      >::template rebind<It>
  {};
} // namespace detail

template< typename T > struct is_readable_iterator
: public ::autoboost::integral_constant<bool,::autoboost::iterators::detail::is_readable_iterator_impl2<T>::value>
{
public:
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,is_readable_iterator,(T))
};

} // namespace iterators

using iterators::is_readable_iterator;

} // namespace autoboost

#endif

#include <autoboost/iterator/detail/config_undef.hpp>

#endif // IS_READABLE_ITERATOR_DWA2003112_HPP

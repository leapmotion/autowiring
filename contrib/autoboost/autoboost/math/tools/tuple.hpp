//  (C) Copyright John Maddock 2010.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_TUPLE_HPP_INCLUDED
#  define AUTOBOOST_MATH_TUPLE_HPP_INCLUDED
#  include <autoboost/config.hpp>
#  include <autoboost/detail/workaround.hpp>

#if !defined(AUTOBOOST_NO_CXX11_HDR_TUPLE) && !AUTOBOOST_WORKAROUND(AUTOBOOST_GCC_VERSION, < 40500)

#include <tuple>

namespace autoboost{ namespace math{

using ::std::tuple;

// [6.1.3.2] Tuple creation functions
using ::std::ignore;
using ::std::make_tuple;
using ::std::tie;
using ::std::get;

// [6.1.3.3] Tuple helper classes
using ::std::tuple_size;
using ::std::tuple_element;

}}

#elif (defined(__BORLANDC__) && (__BORLANDC__ <= 0x600)) || defined(__IBMCPP__)

#include <autoboost/tuple/tuple.hpp>
#include <autoboost/tuple/tuple_comparison.hpp>
#include <autoboost/type_traits/integral_constant.hpp>

namespace autoboost{ namespace math{

using ::autoboost::tuple;

// [6.1.3.2] Tuple creation functions
using ::autoboost::tuples::ignore;
using ::autoboost::make_tuple;
using ::autoboost::tie;

// [6.1.3.3] Tuple helper classes
template <class T>
struct tuple_size
   : public ::autoboost::integral_constant
   < ::std::size_t, ::autoboost::tuples::length<T>::value>
{};

template < int I, class T>
struct tuple_element
{
   typedef typename autoboost::tuples::element<I,T>::type type;
};

#if !AUTOBOOST_WORKAROUND(__BORLANDC__, < 0x0582)
// [6.1.3.4] Element access
using ::autoboost::get;
#endif

} } // namespaces

#else

#include <autoboost/fusion/include/tuple.hpp>
#include <autoboost/fusion/include/std_pair.hpp>

namespace autoboost{ namespace math{

using ::autoboost::fusion::tuple;

// [6.1.3.2] Tuple creation functions
using ::autoboost::fusion::ignore;
using ::autoboost::fusion::make_tuple;
using ::autoboost::fusion::tie;
using ::autoboost::fusion::get;

// [6.1.3.3] Tuple helper classes
using ::autoboost::fusion::tuple_size;
using ::autoboost::fusion::tuple_element;

}}

#endif

#endif



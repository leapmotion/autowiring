#ifndef _BOOST_TYPE_TRAITS_H
#define _BOOST_TYPE_TRAITS_H
#include <boost/type_traits/conditional.hpp>
#include <boost/type_traits/decay.hpp>
#include <boost/type_traits/has_trivial_constructor.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/is_abstract.hpp>
#include <boost/type_traits/is_array.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_polymorphic.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/type_traits/remove_extent.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/utility/enable_if.hpp>

namespace std {
  using boost::conditional;
  using boost::decay;
  using boost::false_type;
  using boost::has_trivial_constructor;
  using boost::integral_constant;
  using boost::is_abstract;
  using boost::is_array;
  using boost::is_base_of;
  using boost::is_const;
  using boost::is_floating_point;
  using boost::is_integral;
  using boost::is_polymorphic;
  using boost::is_same;
  using boost::is_scalar;
  using boost::remove_extent;
  using boost::remove_reference;
  using boost::true_type;
  using boost::is_void;
  using boost::is_convertible;

  template<bool _Test, class _Type = void>
	struct enable_if {};

  template<class _Type>
	struct enable_if<true, _Type>
	{
	  typedef _Type type;
	};
}
#endif

/* autoboost random/uniform_01.hpp header file
 *
 * Copyright Jens Maurer 2000-2001
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id$
 *
 * Revision history
 *  2001-02-18  moved to individual header files
 */

#ifndef AUTOBOOST_RANDOM_UNIFORM_01_HPP
#define AUTOBOOST_RANDOM_UNIFORM_01_HPP

#include <iostream>
#include <autoboost/config.hpp>
#include <autoboost/limits.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/random/detail/config.hpp>
#include <autoboost/random/detail/ptr_helper.hpp>

#include <autoboost/random/detail/disable_warnings.hpp>

namespace autoboost {
namespace random {

#ifdef AUTOBOOST_RANDOM_DOXYGEN

/**
 * The distribution function uniform_01 models a \random_distribution.
 * On each invocation, it returns a random floating-point value
 * uniformly distributed in the range [0..1).
 *
 * The template parameter RealType shall denote a float-like value type
 * with support for binary operators +, -, and /.
 *
 * Note: The current implementation is buggy, because it may not fill
 * all of the mantissa with random bits. I'm unsure how to fill a
 * (to-be-invented) @c autoboost::bigfloat class with random bits efficiently.
 * It's probably time for a traits class.
 */
template<class RealType = double>
class uniform_01
{
public:
  typedef RealType input_type;
  typedef RealType result_type;
  result_type min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION () const;
  result_type max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION () const;
  void reset();

  template<class Engine>
  result_type operator()(Engine& eng);

#ifndef AUTOBOOST_RANDOM_NO_STREAM_OPERATORS
  template<class CharT, class Traits>
  friend std::basic_ostream<CharT,Traits>&
  operator<<(std::basic_ostream<CharT,Traits>& os, const new_uniform_01&)
  {
    return os;
  }

  template<class CharT, class Traits>
  friend std::basic_istream<CharT,Traits>&
  operator>>(std::basic_istream<CharT,Traits>& is, new_uniform_01&)
  {
    return is;
  }
#endif
};

#else

namespace detail {

template<class RealType>
class new_uniform_01
{
public:
  typedef RealType input_type;
  typedef RealType result_type;
  // compiler-generated copy ctor and copy assignment are fine
  result_type min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION () const { return result_type(0); }
  result_type max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION () const { return result_type(1); }
  void reset() { }

  template<class Engine>
  result_type operator()(Engine& eng) {
    for (;;) {
      typedef typename Engine::result_type base_result;
      result_type factor = result_type(1) /
              (result_type(base_result((eng.max)()-(eng.min)())) +
               result_type(std::numeric_limits<base_result>::is_integer ? 1 : 0));
      result_type result = result_type(base_result(eng() - (eng.min)())) * factor;
      if (result < result_type(1))
        return result;
    }
  }

#ifndef AUTOBOOST_RANDOM_NO_STREAM_OPERATORS
  template<class CharT, class Traits>
  friend std::basic_ostream<CharT,Traits>&
  operator<<(std::basic_ostream<CharT,Traits>& os, const new_uniform_01&)
  {
    return os;
  }

  template<class CharT, class Traits>
  friend std::basic_istream<CharT,Traits>&
  operator>>(std::basic_istream<CharT,Traits>& is, new_uniform_01&)
  {
    return is;
  }
#endif
};

template<class UniformRandomNumberGenerator, class RealType>
class backward_compatible_uniform_01
{
  typedef autoboost::random::detail::ptr_helper<UniformRandomNumberGenerator> traits;
public:
  typedef UniformRandomNumberGenerator base_type;
  typedef RealType result_type;

  AUTOBOOST_STATIC_CONSTANT(bool, has_fixed_range = false);

#if !defined(AUTOBOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS)
  AUTOBOOST_STATIC_ASSERT(!std::numeric_limits<RealType>::is_integer);
#endif

  explicit backward_compatible_uniform_01(typename traits::rvalue_type rng)
    : _rng(rng),
      _factor(result_type(1) /
              (result_type((base().max)()-(base().min)()) +
               result_type(std::numeric_limits<base_result>::is_integer ? 1 : 0)))
  {
  }
  // compiler-generated copy ctor and copy assignment are fine

  result_type min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION () const { return result_type(0); }
  result_type max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION () const { return result_type(1); }
  typename traits::value_type& base() { return traits::ref(_rng); }
  const typename traits::value_type& base() const { return traits::ref(_rng); }
  void reset() { }

  result_type operator()() {
    for (;;) {
      result_type result = result_type(base()() - (base().min)()) * _factor;
      if (result < result_type(1))
        return result;
    }
  }

#if !defined(AUTOBOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS)
  template<class CharT, class Traits>
  friend std::basic_ostream<CharT,Traits>&
  operator<<(std::basic_ostream<CharT,Traits>& os, const backward_compatible_uniform_01& u)
  {
    os << u._rng;
    return os;
  }

  template<class CharT, class Traits>
  friend std::basic_istream<CharT,Traits>&
  operator>>(std::basic_istream<CharT,Traits>& is, backward_compatible_uniform_01& u)
  {
    is >> u._rng;
    return is;
  }
#endif

private:
  typedef typename traits::value_type::result_type base_result;
  UniformRandomNumberGenerator _rng;
  result_type _factor;
};

#ifndef AUTOBOOST_NO_INCLASS_MEMBER_INITIALIZATION
//  A definition is required even for integral static constants
template<class UniformRandomNumberGenerator, class RealType>
const bool backward_compatible_uniform_01<UniformRandomNumberGenerator, RealType>::has_fixed_range;
#endif

template<class UniformRandomNumberGenerator, bool is_number = std::numeric_limits<UniformRandomNumberGenerator>::is_specialized>
struct select_uniform_01
{
  template<class RealType>
  struct apply
  {
    typedef backward_compatible_uniform_01<UniformRandomNumberGenerator, RealType> type;
  };
};

template<class Num>
struct select_uniform_01<Num, true>
{
  template<class RealType>
  struct apply
  {
    typedef new_uniform_01<Num> type;
  };
};

}

// Because it is so commonly used: uniform distribution on the real [0..1)
// range.  This allows for specializations to avoid a costly int -> float
// conversion plus float multiplication
template<class UniformRandomNumberGenerator = double, class RealType = double>
class uniform_01
  : public detail::select_uniform_01<UniformRandomNumberGenerator>::AUTOBOOST_NESTED_TEMPLATE apply<RealType>::type
{
  typedef typename detail::select_uniform_01<UniformRandomNumberGenerator>::AUTOBOOST_NESTED_TEMPLATE apply<RealType>::type impl_type;
  typedef autoboost::random::detail::ptr_helper<UniformRandomNumberGenerator> traits;
public:

  uniform_01() {}

  explicit uniform_01(typename traits::rvalue_type rng)
    : impl_type(rng)
  {
  }

#if !defined(AUTOBOOST_NO_OPERATORS_IN_NAMESPACE) && !defined(AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS)
  template<class CharT, class Traits>
  friend std::basic_ostream<CharT,Traits>&
  operator<<(std::basic_ostream<CharT,Traits>& os, const uniform_01& u)
  {
    os << static_cast<const impl_type&>(u);
    return os;
  }

  template<class CharT, class Traits>
  friend std::basic_istream<CharT,Traits>&
  operator>>(std::basic_istream<CharT,Traits>& is, uniform_01& u)
  {
    is >> static_cast<impl_type&>(u);
    return is;
  }
#endif
};

#endif

} // namespace random

using random::uniform_01;

} // namespace autoboost

#include <autoboost/random/detail/enable_warnings.hpp>

#endif // AUTOBOOST_RANDOM_UNIFORM_01_HPP

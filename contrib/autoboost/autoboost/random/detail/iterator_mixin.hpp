/* autoboost random/detail/iterator_mixin.hpp header file
 *
 * Copyright Jens Maurer 2000-2001
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * Revision history
 */

#ifndef AUTOBOOST_ITERATOR_MIXIN_HPP
#define AUTOBOOST_ITERATOR_MIXIN_HPP

#include <autoboost/operators.hpp>

namespace autoboost {

// must be in autoboost namespace, otherwise the inline friend trick fails
template<class Generator, class ResultType>
class generator_iterator_mixin_adapter
  : incrementable<Generator>, equality_comparable<Generator>
{
public:
  typedef std::input_iterator_tag iterator_category;
  typedef ResultType value_type;
  typedef std::ptrdiff_t difference_type;
  typedef const value_type * pointer;
  typedef const value_type & reference;
  Generator& operator++() { v = cast()(); return cast(); }
  const value_type& operator*() const { return v; }

protected:
  // instantiate from derived classes only
  generator_iterator_mixin_adapter() { }
  void iterator_init() { operator++(); }
private:
  Generator & cast() { return static_cast<Generator&>(*this); }
  value_type v;
};

} // namespace autoboost

#endif // AUTOBOOST_ITERATOR_MIXIN_HPP

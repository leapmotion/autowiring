#ifndef INDIRECT_REFERENCE_DWA200415_HPP
# define INDIRECT_REFERENCE_DWA200415_HPP

//
// Copyright David Abrahams 2004. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// typename indirect_reference<P>::type provides the type of *p.
//
// http://www.boost.org/libs/iterator/doc/pointee.html
//

# include <autoboost/detail/is_incrementable.hpp>
# include <autoboost/iterator/iterator_traits.hpp>
# include <autoboost/type_traits/remove_cv.hpp>
# include <autoboost/mpl/eval_if.hpp>
# include <autoboost/pointee.hpp>

namespace autoboost {

namespace detail
{
  template <class P>
  struct smart_ptr_reference
  {
      typedef typename autoboost::pointee<P>::type& type;
  };
}

template <class P>
struct indirect_reference
  : mpl::eval_if<
        detail::is_incrementable<P>
      , iterator_reference<P>
      , detail::smart_ptr_reference<P>
    >
{
};

} // namespace autoboost

#endif // INDIRECT_REFERENCE_DWA200415_HPP

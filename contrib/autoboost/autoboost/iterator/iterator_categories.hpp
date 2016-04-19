// (C) Copyright Jeremy Siek 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ITERATOR_CATEGORIES_HPP
# define AUTOBOOST_ITERATOR_CATEGORIES_HPP

# include <autoboost/config.hpp>
# include <autoboost/detail/iterator.hpp>
# include <autoboost/iterator/detail/config_def.hpp>

# include <autoboost/detail/workaround.hpp>

# include <autoboost/mpl/eval_if.hpp>
# include <autoboost/mpl/identity.hpp>
# include <autoboost/mpl/placeholders.hpp>
# include <autoboost/mpl/aux_/lambda_support.hpp>

# include <autoboost/type_traits/is_convertible.hpp>

# include <autoboost/static_assert.hpp>

namespace autoboost {
namespace iterators {

//
// Traversal Categories
//

struct no_traversal_tag {};

struct incrementable_traversal_tag
  : no_traversal_tag
{
//     incrementable_traversal_tag() {}
//     incrementable_traversal_tag(std::output_iterator_tag const&) {};
};

struct single_pass_traversal_tag
  : incrementable_traversal_tag
{
//     single_pass_traversal_tag() {}
//     single_pass_traversal_tag(std::input_iterator_tag const&) {};
};

struct forward_traversal_tag
  : single_pass_traversal_tag
{
//     forward_traversal_tag() {}
//     forward_traversal_tag(std::forward_iterator_tag const&) {};
};

struct bidirectional_traversal_tag
  : forward_traversal_tag
{
//     bidirectional_traversal_tag() {};
//     bidirectional_traversal_tag(std::bidirectional_iterator_tag const&) {};
};

struct random_access_traversal_tag
  : bidirectional_traversal_tag
{
//     random_access_traversal_tag() {};
//     random_access_traversal_tag(std::random_access_iterator_tag const&) {};
};

namespace detail
{
  //
  // Convert a "strictly old-style" iterator category to a traversal
  // tag.  This is broken out into a separate metafunction to reduce
  // the cost of instantiating iterator_category_to_traversal, below,
  // for new-style types.
  //
  template <class Cat>
  struct old_category_to_traversal
    : mpl::eval_if<
          is_convertible<Cat,std::random_access_iterator_tag>
        , mpl::identity<random_access_traversal_tag>
        , mpl::eval_if<
              is_convertible<Cat,std::bidirectional_iterator_tag>
            , mpl::identity<bidirectional_traversal_tag>
            , mpl::eval_if<
                  is_convertible<Cat,std::forward_iterator_tag>
                , mpl::identity<forward_traversal_tag>
                , mpl::eval_if<
                      is_convertible<Cat,std::input_iterator_tag>
                    , mpl::identity<single_pass_traversal_tag>
                    , mpl::eval_if<
                          is_convertible<Cat,std::output_iterator_tag>
                        , mpl::identity<incrementable_traversal_tag>
                        , void
                      >
                  >
              >
          >
      >
  {};

} // namespace detail

//
// Convert an iterator category into a traversal tag
//
template <class Cat>
struct iterator_category_to_traversal
  : mpl::eval_if< // if already convertible to a traversal tag, we're done.
        is_convertible<Cat,incrementable_traversal_tag>
      , mpl::identity<Cat>
      , autoboost::iterators::detail::old_category_to_traversal<Cat>
    >
{};

// Trait to get an iterator's traversal category
template <class Iterator = mpl::_1>
struct iterator_traversal
  : iterator_category_to_traversal<
        typename autoboost::detail::iterator_traits<Iterator>::iterator_category
    >
{};

# ifdef AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT
// Hack because AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT doesn't seem to work
// out well.  Instantiating the nested apply template also
// requires instantiating iterator_traits on the
// placeholder. Instead we just specialize it as a metafunction
// class.
template <>
struct iterator_traversal<mpl::_1>
{
    template <class T>
    struct apply : iterator_traversal<T>
    {};
};
template <>
struct iterator_traversal<mpl::_>
  : iterator_traversal<mpl::_1>
{};
# endif

//
// Convert an iterator traversal to one of the traversal tags.
//
template <class Traversal>
struct pure_traversal_tag
  : mpl::eval_if<
        is_convertible<Traversal,random_access_traversal_tag>
      , mpl::identity<random_access_traversal_tag>
      , mpl::eval_if<
            is_convertible<Traversal,bidirectional_traversal_tag>
          , mpl::identity<bidirectional_traversal_tag>
          , mpl::eval_if<
                is_convertible<Traversal,forward_traversal_tag>
              , mpl::identity<forward_traversal_tag>
              , mpl::eval_if<
                    is_convertible<Traversal,single_pass_traversal_tag>
                  , mpl::identity<single_pass_traversal_tag>
                  , mpl::eval_if<
                        is_convertible<Traversal,incrementable_traversal_tag>
                      , mpl::identity<incrementable_traversal_tag>
                      , void
                    >
                >
            >
        >
    >
{
};

//
// Trait to retrieve one of the iterator traversal tags from the iterator category or traversal.
//
template <class Iterator = mpl::_1>
struct pure_iterator_traversal
  : pure_traversal_tag<typename iterator_traversal<Iterator>::type>
{};

# ifdef AUTOBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT
template <>
struct pure_iterator_traversal<mpl::_1>
{
    template <class T>
    struct apply : pure_iterator_traversal<T>
    {};
};
template <>
struct pure_iterator_traversal<mpl::_>
  : pure_iterator_traversal<mpl::_1>
{};
# endif

} // namespace iterators

using iterators::no_traversal_tag;
using iterators::incrementable_traversal_tag;
using iterators::single_pass_traversal_tag;
using iterators::forward_traversal_tag;
using iterators::bidirectional_traversal_tag;
using iterators::random_access_traversal_tag;
using iterators::iterator_category_to_traversal;
using iterators::iterator_traversal;

// This import is needed for backward compatibility with Boost.Range:
// autoboost/range/detail/demote_iterator_traversal_tag.hpp
// It should be removed when that header is fixed.
namespace detail {
using iterators::pure_traversal_tag;
} // namespace detail

} // namespace autoboost

#include <autoboost/iterator/detail/config_undef.hpp>

#endif // AUTOBOOST_ITERATOR_CATEGORIES_HPP

// (C) Copyright Jeremy Siek 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ITERATOR_CONCEPTS_HPP
#define AUTOBOOST_ITERATOR_CONCEPTS_HPP

#include <autoboost/concept_check.hpp>
#include <autoboost/iterator/iterator_categories.hpp>

// Use autoboost::detail::iterator_traits to work around some MSVC/Dinkumware problems.
#include <autoboost/detail/iterator.hpp>

#include <autoboost/type_traits/is_same.hpp>
#include <autoboost/type_traits/is_integral.hpp>

#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/and.hpp>
#include <autoboost/mpl/or.hpp>

#include <autoboost/static_assert.hpp>

// Use autoboost/limits to work around missing limits headers on some compilers
#include <autoboost/limits.hpp>
#include <autoboost/config.hpp>

#include <algorithm>

#include <autoboost/concept/detail/concept_def.hpp>

namespace autoboost_concepts
{
  // Used a different namespace here (instead of "autoboost") so that the
  // concept descriptions do not take for granted the names in
  // namespace autoboost.

  //===========================================================================
  // Iterator Access Concepts

  AUTOBOOST_concept(ReadableIterator,(Iterator))
    : autoboost::Assignable<Iterator>
    , autoboost::CopyConstructible<Iterator>

  {
      typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::iterator_traits<Iterator>::value_type value_type;
      typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::iterator_traits<Iterator>::reference reference;

      AUTOBOOST_CONCEPT_USAGE(ReadableIterator)
      {

          value_type v = *i;
          autoboost::ignore_unused_variable_warning(v);
      }
  private:
      Iterator i;
  };

  template <
      typename Iterator
    , typename ValueType = AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::iterator_traits<Iterator>::value_type
  >
  struct WritableIterator
    : autoboost::CopyConstructible<Iterator>
  {
      AUTOBOOST_CONCEPT_USAGE(WritableIterator)
      {
          *i = v;
      }
  private:
      ValueType v;
      Iterator i;
  };

  template <
      typename Iterator
    , typename ValueType = AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::iterator_traits<Iterator>::value_type
  >
  struct WritableIteratorConcept : WritableIterator<Iterator,ValueType> {};

  AUTOBOOST_concept(SwappableIterator,(Iterator))
  {
      AUTOBOOST_CONCEPT_USAGE(SwappableIterator)
      {
          std::iter_swap(i1, i2);
      }
  private:
      Iterator i1;
      Iterator i2;
  };

  AUTOBOOST_concept(LvalueIterator,(Iterator))
  {
      typedef typename autoboost::detail::iterator_traits<Iterator>::value_type value_type;

      AUTOBOOST_CONCEPT_USAGE(LvalueIterator)
      {
        value_type& r = const_cast<value_type&>(*i);
        autoboost::ignore_unused_variable_warning(r);
      }
  private:
      Iterator i;
  };


  //===========================================================================
  // Iterator Traversal Concepts

  AUTOBOOST_concept(IncrementableIterator,(Iterator))
    : autoboost::Assignable<Iterator>
    , autoboost::CopyConstructible<Iterator>
  {
      typedef typename autoboost::iterator_traversal<Iterator>::type traversal_category;

      AUTOBOOST_CONCEPT_ASSERT((
        autoboost::Convertible<
            traversal_category
          , autoboost::incrementable_traversal_tag
        >));

      AUTOBOOST_CONCEPT_USAGE(IncrementableIterator)
      {
          ++i;
          (void)i++;
      }
  private:
      Iterator i;
  };

  AUTOBOOST_concept(SinglePassIterator,(Iterator))
    : IncrementableIterator<Iterator>
    , autoboost::EqualityComparable<Iterator>

  {
      AUTOBOOST_CONCEPT_ASSERT((
          autoboost::Convertible<
             AUTOBOOST_DEDUCED_TYPENAME SinglePassIterator::traversal_category
           , autoboost::single_pass_traversal_tag
          > ));
  };

  AUTOBOOST_concept(ForwardTraversal,(Iterator))
    : SinglePassIterator<Iterator>
    , autoboost::DefaultConstructible<Iterator>
  {
      typedef typename autoboost::detail::iterator_traits<Iterator>::difference_type difference_type;

      AUTOBOOST_MPL_ASSERT((autoboost::is_integral<difference_type>));
      AUTOBOOST_MPL_ASSERT_RELATION(std::numeric_limits<difference_type>::is_signed, ==, true);

      AUTOBOOST_CONCEPT_ASSERT((
          autoboost::Convertible<
             AUTOBOOST_DEDUCED_TYPENAME ForwardTraversal::traversal_category
           , autoboost::forward_traversal_tag
          > ));
  };

  AUTOBOOST_concept(BidirectionalTraversal,(Iterator))
    : ForwardTraversal<Iterator>
  {
      AUTOBOOST_CONCEPT_ASSERT((
          autoboost::Convertible<
             AUTOBOOST_DEDUCED_TYPENAME BidirectionalTraversal::traversal_category
           , autoboost::bidirectional_traversal_tag
          > ));

      AUTOBOOST_CONCEPT_USAGE(BidirectionalTraversal)
      {
          --i;
          (void)i--;
      }
   private:
      Iterator i;
  };

  AUTOBOOST_concept(RandomAccessTraversal,(Iterator))
    : BidirectionalTraversal<Iterator>
  {
      AUTOBOOST_CONCEPT_ASSERT((
          autoboost::Convertible<
             AUTOBOOST_DEDUCED_TYPENAME RandomAccessTraversal::traversal_category
           , autoboost::random_access_traversal_tag
          > ));

      AUTOBOOST_CONCEPT_USAGE(RandomAccessTraversal)
      {
          i += n;
          i = i + n;
          i = n + i;
          i -= n;
          i = i - n;
          n = i - j;
      }

   private:
      typename BidirectionalTraversal<Iterator>::difference_type n;
      Iterator i, j;
  };

  //===========================================================================
  // Iterator Interoperability

  namespace detail
  {
    template <typename Iterator1, typename Iterator2>
    void interop_single_pass_constraints(Iterator1 const& i1, Iterator2 const& i2)
    {
        bool b;
        b = i1 == i2;
        b = i1 != i2;

        b = i2 == i1;
        b = i2 != i1;
        autoboost::ignore_unused_variable_warning(b);
    }

    template <typename Iterator1, typename Iterator2>
    void interop_rand_access_constraints(
        Iterator1 const& i1, Iterator2 const& i2,
        autoboost::random_access_traversal_tag, autoboost::random_access_traversal_tag)
    {
        bool b;
        typename autoboost::detail::iterator_traits<Iterator2>::difference_type n;
        b = i1 <  i2;
        b = i1 <= i2;
        b = i1 >  i2;
        b = i1 >= i2;
        n = i1 -  i2;

        b = i2 <  i1;
        b = i2 <= i1;
        b = i2 >  i1;
        b = i2 >= i1;
        n = i2 -  i1;
        autoboost::ignore_unused_variable_warning(b);
        autoboost::ignore_unused_variable_warning(n);
    }

    template <typename Iterator1, typename Iterator2>
    void interop_rand_access_constraints(
        Iterator1 const&, Iterator2 const&,
        autoboost::single_pass_traversal_tag, autoboost::single_pass_traversal_tag)
    { }

  } // namespace detail

  AUTOBOOST_concept(InteroperableIterator,(Iterator)(ConstIterator))
  {
   private:
      typedef typename autoboost::iterators::pure_iterator_traversal<Iterator>::type traversal_category;
      typedef typename autoboost::iterators::pure_iterator_traversal<ConstIterator>::type const_traversal_category;

   public:
      AUTOBOOST_CONCEPT_ASSERT((SinglePassIterator<Iterator>));
      AUTOBOOST_CONCEPT_ASSERT((SinglePassIterator<ConstIterator>));

      AUTOBOOST_CONCEPT_USAGE(InteroperableIterator)
      {
          detail::interop_single_pass_constraints(i, ci);
          detail::interop_rand_access_constraints(i, ci, traversal_category(), const_traversal_category());

          ci = i;
      }

   private:
      Iterator      i;
      ConstIterator ci;
  };

} // namespace autoboost_concepts

#include <autoboost/concept/detail/concept_undef.hpp>

#endif // AUTOBOOST_ITERATOR_CONCEPTS_HPP

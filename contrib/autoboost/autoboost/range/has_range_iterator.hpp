// Boost.Range library
//
//  Copyright Neil Groves 2010. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
// Acknowledgments:
// Ticket #8341: Arno Schoedl - improved handling of has_range_iterator upon
// use-cases where T was const.
#ifndef AUTOBOOST_RANGE_HAS_ITERATOR_HPP_INCLUDED
#define AUTOBOOST_RANGE_HAS_ITERATOR_HPP_INCLUDED

#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/eval_if.hpp>
#include <autoboost/mpl/has_xxx.hpp>
#include <autoboost/range/iterator.hpp>
#include <autoboost/type_traits/remove_reference.hpp>
#include <autoboost/utility/enable_if.hpp>

namespace autoboost
{
    namespace range_detail
    {
        AUTOBOOST_MPL_HAS_XXX_TRAIT_DEF(type)

        template<class T, class Enabler = void>
        struct has_range_iterator_impl
            : autoboost::mpl::false_
        {
        };

        template<class T>
        struct has_range_iterator_impl<
            T,
            AUTOBOOST_DEDUCED_TYPENAME ::autoboost::enable_if<
                AUTOBOOST_DEDUCED_TYPENAME mpl::eval_if<is_const<T>,
                    has_type<range_const_iterator<
                                AUTOBOOST_DEDUCED_TYPENAME remove_const<T>::type> >,
                    has_type<range_mutable_iterator<T> >
                >::type
            >::type
        >
            : autoboost::mpl::true_
        {
        };

        template<class T, class Enabler = void>
        struct has_range_const_iterator_impl
            : autoboost::mpl::false_
        {
        };

        template<class T>
        struct has_range_const_iterator_impl<
            T,
            AUTOBOOST_DEDUCED_TYPENAME ::autoboost::enable_if<
                has_type<range_const_iterator<T> >
            >::type
        >
            : autoboost::mpl::true_
        {
        };

    } // namespace range_detail

    template<class T>
    struct has_range_iterator
        : range_detail::has_range_iterator_impl<
            AUTOBOOST_DEDUCED_TYPENAME remove_reference<T>::type>
    {};

    template<class T>
    struct has_range_const_iterator
        : range_detail::has_range_const_iterator_impl<
            AUTOBOOST_DEDUCED_TYPENAME remove_reference<T>::type>
    {};
} // namespace autoboost

#endif // include guard


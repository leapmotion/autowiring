/*=============================================================================
    Copyright (c) 2005-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(AUTOBOOST_FUSION_MAP_IMPL_02032013_2233)
#define AUTOBOOST_FUSION_MAP_IMPL_02032013_2233

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/fusion/support/detail/access.hpp>
#include <autoboost/fusion/iterator/deref.hpp>
#include <autoboost/fusion/iterator/next.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/identity.hpp>

namespace autoboost { namespace fusion
{
    struct fusion_sequence_tag;
}}

namespace autoboost { namespace fusion { namespace detail
{
    struct map_impl_from_iterator {};

    template <int index, typename ...T>
    struct map_impl;

    template <int index_>
    struct map_impl<index_>
    {
        typedef fusion_sequence_tag tag;
        static int const index = index_;
        static int const size = 0;

        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl() {}

        template <typename Iterator>
        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl(Iterator const&, map_impl_from_iterator)
        {}

        template <typename Iterator>
        AUTOBOOST_FUSION_GPU_ENABLED
        void assign(Iterator const&, map_impl_from_iterator)
        {}

        AUTOBOOST_FUSION_GPU_ENABLED
        void get();
        AUTOBOOST_FUSION_GPU_ENABLED
        void get_val();
        AUTOBOOST_FUSION_GPU_ENABLED
        void get_key();
    };

    template <int index_, typename Pair, typename ...T>
    struct map_impl<index_, Pair, T...> : map_impl<index_ + 1, T...>
    {
        typedef fusion_sequence_tag tag;
        typedef map_impl<index_+1, T...> rest_type;

        using rest_type::get;
        using rest_type::get_val;
        using rest_type::get_key;

        static int const index = index_;
        static int const size = rest_type::size + 1;

        typedef Pair pair_type;
        typedef typename Pair::first_type key_type;
        typedef typename Pair::second_type value_type;

        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl()
          : rest_type(), element()
        {}

        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl(map_impl const& rhs)
          : rest_type(rhs.get_base()), element(rhs.element)
        {}

        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl(map_impl&& rhs)
          : rest_type(std::forward<rest_type>(*static_cast<rest_type*>(&rhs)))
          , element(std::forward<Pair>(rhs.element))
        {}

        template <typename ...U>
        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl(map_impl<index, U...> const& rhs)
          : rest_type(rhs.get_base()), element(rhs.element)
        {}

        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl(typename detail::call_param<Pair>::type element_
          , typename detail::call_param<T>::type... rest)
          : rest_type(rest...), element(element_)
        {}

        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl(Pair&& element_, T&&... rest)
          : rest_type(std::forward<T>(rest)...)
          , element(std::forward<Pair>(element_))
        {}

        template <typename Iterator>
        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl(Iterator const& iter, map_impl_from_iterator fi)
          : rest_type(fusion::next(iter), fi)
          , element(*iter)
        {}

        AUTOBOOST_FUSION_GPU_ENABLED
        rest_type& get_base()
        {
            return *this;
        }

        AUTOBOOST_FUSION_GPU_ENABLED
        rest_type const& get_base() const
        {
            return *this;
        }

        AUTOBOOST_FUSION_GPU_ENABLED
        value_type get_val(mpl::identity<key_type>);
        AUTOBOOST_FUSION_GPU_ENABLED
        pair_type get_val(mpl::int_<index>);
        AUTOBOOST_FUSION_GPU_ENABLED
        value_type get_val(mpl::identity<key_type>) const;
        AUTOBOOST_FUSION_GPU_ENABLED
        pair_type get_val(mpl::int_<index>) const;

        AUTOBOOST_FUSION_GPU_ENABLED
        mpl::identity<key_type> get_key(mpl::int_<index>);
        AUTOBOOST_FUSION_GPU_ENABLED
        mpl::identity<key_type> get_key(mpl::int_<index>) const;

        AUTOBOOST_FUSION_GPU_ENABLED
        typename cref_result<value_type>::type
        get(mpl::identity<key_type>) const
        {
            return element.second;
        }

        AUTOBOOST_FUSION_GPU_ENABLED
        typename ref_result<value_type>::type
        get(mpl::identity<key_type>)
        {
            return element.second;
        }

        AUTOBOOST_FUSION_GPU_ENABLED
        typename cref_result<pair_type>::type
        get(mpl::int_<index>) const
        {
            return element;
        }

        AUTOBOOST_FUSION_GPU_ENABLED
        typename ref_result<pair_type>::type
        get(mpl::int_<index>)
        {
            return element;
        }

        template <typename ...U>
        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl& operator=(map_impl<index, U...> const& rhs)
        {
            rest_type::operator=(rhs);
            element = rhs.element;
            return *this;
        }

        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl& operator=(map_impl const& rhs)
        {
            rest_type::operator=(rhs);
            element = rhs.element;
            return *this;
        }

        AUTOBOOST_FUSION_GPU_ENABLED
        map_impl& operator=(map_impl&& rhs)
        {
            rest_type::operator=(std::forward<map_impl>(rhs));
            element = std::forward<Pair>(rhs.element);
            return *this;
        }

        template <typename Iterator>
        AUTOBOOST_FUSION_GPU_ENABLED
        void assign(Iterator const& iter, map_impl_from_iterator fi)
        {
            rest_type::assign(fusion::next(iter), fi);
            element = *iter;
        }

        Pair element;
    };
}}}

#endif

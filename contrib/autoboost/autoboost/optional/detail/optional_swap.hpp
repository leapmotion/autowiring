// Copyright (C) 2003, 2008 Fernando Luis Cacciola Carballal.
// Copyright (C) 2015 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/optional for documentation.
//
// You are welcome to contact the author at:
//  akrzemi1@gmail.com

#ifndef AUTOBOOST_OPTIONAL_DETAIL_OPTIONAL_SWAP_AJK_28JAN2015_HPP
#define AUTOBOOST_OPTIONAL_DETAIL_OPTIONAL_SWAP_AJK_28JAN2015_HPP

#include <autoboost/core/swap.hpp>
#include <autoboost/optional/optional_fwd.hpp>

namespace autoboost {

namespace optional_detail {

template <bool use_default_constructor> struct swap_selector;

template <>
struct swap_selector<true>
{
    template <class T>
    static void optional_swap ( optional<T>& x, optional<T>& y )
    {
        const bool hasX = !!x;
        const bool hasY = !!y;

        if ( !hasX && !hasY )
            return;

        if( !hasX )
            x.emplace();
        else if ( !hasY )
            y.emplace();

        // Boost.Utility.Swap will take care of ADL and workarounds for broken compilers
        autoboost::swap(x.get(), y.get());

        if( !hasX )
            y = autoboost::none ;
        else if( !hasY )
            x = autoboost::none ;
    }
};

#ifdef AUTOBOOST_OPTIONAL_DETAIL_MOVE
# undef AUTOBOOST_OPTIONAL_DETAIL_MOVE
#endif

#ifndef AUTOBOOST_OPTIONAL_DETAIL_NO_RVALUE_REFERENCES
# define AUTOBOOST_OPTIONAL_DETAIL_MOVE(EXPR_) autoboost::move(EXPR_)
#else
# define AUTOBOOST_OPTIONAL_DETAIL_MOVE(EXPR_) EXPR_
#endif

template <>
struct swap_selector<false>
{
    template <class T>
    static void optional_swap ( optional<T>& x, optional<T>& y )
    //AUTOBOOST_NOEXCEPT_IF(::autoboost::is_nothrow_move_constructible<T>::value && AUTOBOOST_NOEXCEPT_EXPR(autoboost::swap(*x, *y)))
    {
        if (x)
        {
            if (y)
            {
                autoboost::swap(*x, *y);
            }
            else
            {
                y = AUTOBOOST_OPTIONAL_DETAIL_MOVE(*x);
                x = autoboost::none;
            }
        }
        else
        {
            if (y)
            {
                x = AUTOBOOST_OPTIONAL_DETAIL_MOVE(*y);
                y = autoboost::none;
            }
        }
    }
};

} // namespace optional_detail

#if (!defined AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && (!defined AUTOBOOST_CONFIG_RESTORE_OBSOLETE_SWAP_IMPLEMENTATION)

template<class T>
struct optional_swap_should_use_default_constructor : autoboost::false_type {} ;

#else

template<class T>
struct optional_swap_should_use_default_constructor : has_nothrow_default_constructor<T> {} ;

#endif

template <class T>
inline void swap ( optional<T>& x, optional<T>& y )
//AUTOBOOST_NOEXCEPT_IF(::autoboost::is_nothrow_move_constructible<T>::value && AUTOBOOST_NOEXCEPT_EXPR(autoboost::swap(*x, *y)))
{
    optional_detail::swap_selector<optional_swap_should_use_default_constructor<T>::value>::optional_swap(x, y);
}

} // namespace autoboost

#undef AUTOBOOST_OPTIONAL_DETAIL_MOVE

#endif // header guard

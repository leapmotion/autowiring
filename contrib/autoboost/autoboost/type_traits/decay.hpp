//  (C) Copyright John Maddock & Thorsten Ottosen 2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_DECAY_HPP_INCLUDED
#define AUTOBOOST_TT_DECAY_HPP_INCLUDED

#include <autoboost/type_traits/config.hpp>
#include <autoboost/type_traits/is_array.hpp>
#include <autoboost/type_traits/is_function.hpp>
#include <autoboost/type_traits/remove_bounds.hpp>
#include <autoboost/type_traits/add_pointer.hpp>
#include <autoboost/type_traits/remove_reference.hpp>
#include <autoboost/mpl/eval_if.hpp>
#include <autoboost/mpl/identity.hpp>

namespace autoboost
{

    template< class T >
    struct decay
    {
    private:
        typedef AUTOBOOST_DEDUCED_TYPENAME remove_reference<T>::type Ty;
    public:
        typedef AUTOBOOST_DEDUCED_TYPENAME mpl::eval_if<
            is_array<Ty>,
            mpl::identity<AUTOBOOST_DEDUCED_TYPENAME remove_bounds<Ty>::type*>,
            AUTOBOOST_DEDUCED_TYPENAME mpl::eval_if<
                is_function<Ty>,
                add_pointer<Ty>,
                mpl::identity<Ty>
            >
        >::type type;
    };

} // namespace autoboost


#endif // AUTOBOOST_TT_DECAY_HPP_INCLUDED

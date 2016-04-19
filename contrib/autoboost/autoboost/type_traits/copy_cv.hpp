#ifndef AUTOBOOST_TYPE_TRAITS_COPY_CV_HPP_INCLUDED
#define AUTOBOOST_TYPE_TRAITS_COPY_CV_HPP_INCLUDED

//
//  Copyright 2015 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//

#include <autoboost/type_traits/is_const.hpp>
#include <autoboost/type_traits/is_volatile.hpp>
#include <autoboost/type_traits/add_const.hpp>
#include <autoboost/type_traits/add_volatile.hpp>
#include <autoboost/type_traits/conditional.hpp>

namespace autoboost
{

template<class T, class U> struct copy_cv
{
private:

    typedef typename autoboost::conditional<autoboost::is_const<U>::value, typename autoboost::add_const<T>::type, T>::type CT;

public:

    typedef typename autoboost::conditional<autoboost::is_volatile<U>::value, typename autoboost::add_volatile<CT>::type, CT>::type type;
};

} // namespace autoboost

#endif // #ifndef AUTOBOOST_TYPE_TRAITS_COPY_CV_HPP_INCLUDED

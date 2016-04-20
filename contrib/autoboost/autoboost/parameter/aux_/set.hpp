// Copyright Daniel Wallin 2006. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_PARAMETER_SET_060912_HPP
# define AUTOBOOST_PARAMETER_SET_060912_HPP

# include <autoboost/detail/workaround.hpp>

# if !AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564)) \
  && !AUTOBOOST_WORKAROUND(__GNUC__, < 3)
#  include <autoboost/mpl/insert.hpp>
#  include <autoboost/mpl/set/set0.hpp>
#  include <autoboost/mpl/has_key.hpp>

namespace autoboost { namespace parameter { namespace aux {

typedef mpl::set0<> set0;

template <class Set, class K>
struct insert_
{
    typedef typename mpl::insert<Set, K>::type type;
};

template <class Set, class K>
struct has_key_
{
    typedef typename mpl::has_key<Set, K>::type type;
};

}}} // namespace autoboost::parameter::aux

# else

#  include <autoboost/mpl/list.hpp>
#  include <autoboost/mpl/end.hpp>
#  include <autoboost/mpl/find.hpp>
#  include <autoboost/mpl/not.hpp>
#  include <autoboost/mpl/push_front.hpp>

namespace autoboost { namespace parameter { namespace aux {

typedef mpl::list0<> set0;

template <class Set, class K>
struct insert_
{
    typedef typename mpl::push_front<Set, K>::type type;
};

template <class Set, class K>
struct has_key_
{
    typedef typename mpl::find<Set, K>::type iter;
    typedef mpl::not_<
        is_same<iter, typename mpl::end<Set>::type>
    > type;
};

}}} // namespace autoboost::parameter::aux

# endif


#endif // AUTOBOOST_PARAMETER_SET_060912_HPP


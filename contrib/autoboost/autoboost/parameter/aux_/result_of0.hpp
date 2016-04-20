// Copyright David Abrahams 2005. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_PARAMETER_AUX_RESULT_OF0_DWA2005511_HPP
# define AUTOBOOST_PARAMETER_AUX_RESULT_OF0_DWA2005511_HPP

# include <autoboost/utility/result_of.hpp>

// A metafunction returning the result of invoking a nullary function
// object of the given type.

#ifndef AUTOBOOST_NO_RESULT_OF

# include <autoboost/utility/result_of.hpp>
namespace autoboost { namespace parameter { namespace aux {
template <class F>
struct result_of0 : result_of<F()>
{};

}}} // namespace autoboost::parameter::aux_

#else

namespace autoboost { namespace parameter { namespace aux {
template <class F>
struct result_of0
{
    typedef typename F::result_type type;
};

}}} // namespace autoboost::parameter::aux_

#endif


#endif // AUTOBOOST_PARAMETER_AUX_RESULT_OF0_DWA2005511_HPP

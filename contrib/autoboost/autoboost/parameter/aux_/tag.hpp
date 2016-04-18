// Copyright David Abrahams 2005. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_PARAMETER_AUX_TAG_DWA2005610_HPP
# define AUTOBOOST_PARAMETER_AUX_TAG_DWA2005610_HPP

# include <autoboost/parameter/aux_/unwrap_cv_reference.hpp>
# include <autoboost/parameter/aux_/tagged_argument.hpp>

namespace autoboost { namespace parameter { namespace aux {

template <class Keyword, class ActualArg
#if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))
        , class = typename is_cv_reference_wrapper<ActualArg>::type
#endif
          >
struct tag
{
    typedef tagged_argument<
        Keyword
      , typename unwrap_cv_reference<ActualArg>::type
    > type;
};

#if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))
template <class Keyword, class ActualArg>
struct tag<Keyword,ActualArg,mpl::false_>
{
    typedef tagged_argument<
        Keyword
      , ActualArg
    > type;
};
#endif

}}} // namespace autoboost::parameter::aux_

#endif // AUTOBOOST_PARAMETER_AUX_TAG_DWA2005610_HPP

// Copyright David Abrahams 2005. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_PARAMETER_BINDING_DWA200558_HPP
# define AUTOBOOST_PARAMETER_BINDING_DWA200558_HPP

# include <autoboost/mpl/apply.hpp>
# include <autoboost/mpl/assert.hpp>
# include <autoboost/mpl/and.hpp>
# include <autoboost/parameter/aux_/result_of0.hpp>
# include <autoboost/parameter/aux_/void.hpp>
# include <autoboost/type_traits/is_same.hpp>

# if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
#  include <autoboost/mpl/eval_if.hpp>
# endif

namespace autoboost { namespace parameter {

// A metafunction that, given an argument pack, returns the type of
// the parameter identified by the given keyword.  If no such
// parameter has been specified, returns Default

# if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300) \
  || AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))
template <class Parameters, class Keyword, class Default>
struct binding0
{
    typedef typename mpl::apply_wrap3<
        typename Parameters::binding,Keyword,Default,mpl::true_
    >::type type;

    AUTOBOOST_MPL_ASSERT_NOT((
        mpl::and_<
            is_same<Default, void_>
          , is_same<type, void_>
        >
    ));
};
# endif

template <class Parameters, class Keyword, class Default = void_>
# if !AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
struct binding
# else
struct binding_eti
# endif
{
# if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300) \
  || AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))
    typedef typename mpl::eval_if<
        mpl::is_placeholder<Parameters>
      , mpl::identity<int>
      , binding0<Parameters,Keyword,Default>
    >::type type;
# else
    typedef typename mpl::apply_wrap3<
        typename Parameters::binding,Keyword,Default,mpl::true_
    >::type type;

    AUTOBOOST_MPL_ASSERT_NOT((
        mpl::and_<
            is_same<Default, void_>
          , is_same<type, void_>
        >
    ));
# endif

# if !AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(3,binding,(Parameters,Keyword,Default))
# endif
};

# if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
template <class Parameters, class Keyword, class Default = void_>
struct binding
{
    typedef typename mpl::eval_if<
        is_same<Parameters, int>
      , mpl::identity<int>
      , binding_eti<Parameters, Keyword, Default>
    >::type type;

    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(3,binding,(Parameters,Keyword,Default))
};
# endif

// A metafunction that, given an argument pack, returns the type of
// the parameter identified by the given keyword.  If no such
// parameter has been specified, returns the type returned by invoking
// DefaultFn
template <class Parameters, class Keyword, class DefaultFn>
struct lazy_binding
{
  typedef typename mpl::apply_wrap3<
      typename Parameters::binding
    , Keyword
    , typename aux::result_of0<DefaultFn>::type
    , mpl::true_
  >::type type;
};


}} // namespace autoboost::parameter

#endif // AUTOBOOST_PARAMETER_BINDING_DWA200558_HPP

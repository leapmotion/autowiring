// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_CONCEPT_CHECK_MSVC_DWA2006429_HPP
# define AUTOBOOST_CONCEPT_CHECK_MSVC_DWA2006429_HPP

# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/concept/detail/backward_compatibility.hpp>
# include <autoboost/config.hpp>

# ifdef AUTOBOOST_OLD_CONCEPT_SUPPORT
#  include <autoboost/concept/detail/has_constraints.hpp>
#  include <autoboost/mpl/if.hpp>
# endif

# ifdef AUTOBOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable:4100)
# endif

namespace autoboost { namespace concepts {


template <class Model>
struct check
{
    virtual void failed(Model* x)
    {
        x->~Model();
    }
};

# ifndef AUTOBOOST_NO_PARTIAL_SPECIALIZATION
struct failed {};
template <class Model>
struct check<failed ************ Model::************>
{
    virtual void failed(Model* x)
    {
        x->~Model();
    }
};
# endif

# ifdef AUTOBOOST_OLD_CONCEPT_SUPPORT

namespace detail
{
  // No need for a virtual function here, since evaluating
  // not_satisfied below will have already instantiated the
  // constraints() member.
  struct constraint {};
}

template <class Model>
struct require
  : mpl::if_c<
        not_satisfied<Model>::value
      , detail::constraint
# ifndef AUTOBOOST_NO_PARTIAL_SPECIALIZATION
      , check<Model>
# else
      , check<failed ************ Model::************>
# endif
        >::type
{};

# else

template <class Model>
struct require
# ifndef AUTOBOOST_NO_PARTIAL_SPECIALIZATION
    : check<Model>
# else
    : check<failed ************ Model::************>
# endif
{};

# endif

# if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, == 1310)

//
// The iterator library sees some really strange errors unless we
// do things this way.
//
template <class Model>
struct require<void(*)(Model)>
{
    virtual void failed(Model*)
    {
        require<Model>();
    }
};

# define AUTOBOOST_CONCEPT_ASSERT_FN( ModelFnPtr )      \
enum                                                \
{                                                   \
    AUTOBOOST_PP_CAT(autoboost_concept_check,__LINE__) =    \
    sizeof(::autoboost::concepts::require<ModelFnPtr>)    \
}

# else // Not vc-7.1

template <class Model>
require<Model>
require_(void(*)(Model));

# define AUTOBOOST_CONCEPT_ASSERT_FN( ModelFnPtr )          \
enum                                                    \
{                                                       \
    AUTOBOOST_PP_CAT(autoboost_concept_check,__LINE__) =        \
      sizeof(::autoboost::concepts::require_((ModelFnPtr)0)) \
}

# endif
}}

# ifdef AUTOBOOST_MSVC
#  pragma warning(pop)
# endif

#endif // AUTOBOOST_CONCEPT_CHECK_MSVC_DWA2006429_HPP

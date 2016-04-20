
#if !defined(AUTOBOOST_PP_IS_ITERATING)

///// header body

#ifndef AUTOBOOST_MPL_BIND_HPP_INCLUDED
#define AUTOBOOST_MPL_BIND_HPP_INCLUDED

// Copyright Peter Dimov 2001
// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#if !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)
#   include <autoboost/mpl/bind_fwd.hpp>
#   include <autoboost/mpl/placeholders.hpp>
#   include <autoboost/mpl/next.hpp>
#   include <autoboost/mpl/protect.hpp>
#   include <autoboost/mpl/apply_wrap.hpp>
#   include <autoboost/mpl/limits/arity.hpp>
#   include <autoboost/mpl/aux_/na.hpp>
#   include <autoboost/mpl/aux_/arity_spec.hpp>
#   include <autoboost/mpl/aux_/type_wrapper.hpp>
#   include <autoboost/mpl/aux_/yes_no.hpp>
#   if defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
#       include <autoboost/type_traits/is_reference.hpp>
#   endif
#endif

#include <autoboost/mpl/aux_/config/bind.hpp>
#include <autoboost/mpl/aux_/config/static_constant.hpp>
#include <autoboost/mpl/aux_/config/use_preprocessed.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS) \
    && !defined(AUTOBOOST_MPL_PREPROCESSING_MODE)

#   if defined(AUTOBOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT)
#       define AUTOBOOST_MPL_PREPROCESSED_HEADER basic_bind.hpp
#   else
#       define AUTOBOOST_MPL_PREPROCESSED_HEADER bind.hpp
#   endif
#   include <autoboost/mpl/aux_/include_preprocessed.hpp>

#else

#   include <autoboost/mpl/aux_/preprocessor/params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/default_params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/def_params_tail.hpp>
#   include <autoboost/mpl/aux_/preprocessor/partial_spec_params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/ext_params.hpp>
#   include <autoboost/mpl/aux_/preprocessor/repeat.hpp>
#   include <autoboost/mpl/aux_/preprocessor/enum.hpp>
#   include <autoboost/mpl/aux_/preprocessor/add.hpp>
#   include <autoboost/mpl/aux_/config/dmc_ambiguous_ctps.hpp>
#   include <autoboost/mpl/aux_/config/ctps.hpp>
#   include <autoboost/mpl/aux_/config/ttp.hpp>
#   include <autoboost/mpl/aux_/config/dtp.hpp>
#   include <autoboost/mpl/aux_/nttp_decl.hpp>

#   include <autoboost/preprocessor/iterate.hpp>
#   include <autoboost/preprocessor/comma_if.hpp>
#   include <autoboost/preprocessor/cat.hpp>
#   include <autoboost/preprocessor/inc.hpp>

namespace autoboost { namespace mpl {

// local macros, #undef-ined at the end of the header
#   define AUX778076_APPLY \
    AUTOBOOST_PP_CAT(apply_wrap,AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY) \
    /**/

#   if defined(AUTOBOOST_MPL_CFG_DMC_AMBIGUOUS_CTPS)
#       define AUX778076_DMC_PARAM() , int dummy_
#   else
#       define AUX778076_DMC_PARAM()
#   endif

#   define AUX778076_BIND_PARAMS(param) \
    AUTOBOOST_MPL_PP_PARAMS( \
          AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY \
        , param \
        ) \
    /**/

#   define AUX778076_BIND_DEFAULT_PARAMS(param, value) \
    AUTOBOOST_MPL_PP_DEFAULT_PARAMS( \
          AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY \
        , param \
        , value \
        ) \
    /**/

#   define AUX778076_BIND_N_PARAMS(n, param) \
    AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_MPL_PP_PARAMS(n, param) \
    /**/

#   define AUX778076_BIND_N_SPEC_PARAMS(n, param, def) \
    AUTOBOOST_PP_COMMA_IF(n) \
    AUTOBOOST_MPL_PP_PARTIAL_SPEC_PARAMS(n, param, def) \
    /**/

#if !defined(AUTOBOOST_MPL_CFG_NO_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
#   define AUX778076_BIND_NESTED_DEFAULT_PARAMS(param, value) \
    AUX778076_BIND_DEFAULT_PARAMS(param, value) \
    /**/
#else
#   define AUX778076_BIND_NESTED_DEFAULT_PARAMS(param, value) \
    AUX778076_BIND_PARAMS(param) \
    /**/
#endif

namespace aux {

#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template<
      typename T, AUX778076_BIND_PARAMS(typename U)
    >
struct resolve_bind_arg
{
    typedef T type;
};

#   if !defined(AUTOBOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT)

template<
      typename T
    , typename Arg
    >
struct replace_unnamed_arg
{
    typedef Arg next;
    typedef T type;
};

template<
      typename Arg
    >
struct replace_unnamed_arg< arg<-1>,Arg >
{
    typedef typename Arg::next next;
    typedef Arg type;
};

#   endif // AUTOBOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT

template<
      AUTOBOOST_MPL_AUX_NTTP_DECL(int, N), AUX778076_BIND_PARAMS(typename U)
    >
struct resolve_bind_arg< arg<N>,AUX778076_BIND_PARAMS(U) >
{
    typedef typename AUX778076_APPLY<mpl::arg<N>, AUX778076_BIND_PARAMS(U)>::type type;
};

#if !defined(AUTOBOOST_MPL_CFG_NO_BIND_TEMPLATE)
template<
      typename F, AUX778076_BIND_PARAMS(typename T), AUX778076_BIND_PARAMS(typename U)
    >
struct resolve_bind_arg< bind<F,AUX778076_BIND_PARAMS(T)>,AUX778076_BIND_PARAMS(U) >
{
    typedef bind<F,AUX778076_BIND_PARAMS(T)> f_;
    typedef typename AUX778076_APPLY<f_, AUX778076_BIND_PARAMS(U)>::type type;
};
#endif

#else // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

// agurt, 15/jan/02: it's not a intended to be used as a function class, and
// MSVC6.5 has problems with 'apply' name here (the code compiles, but doesn't
// work), so I went with the 'result_' here, and in all other similar cases
template< bool >
struct resolve_arg_impl
{
    template< typename T, AUX778076_BIND_PARAMS(typename U) > struct result_
    {
        typedef T type;
    };
};

template<>
struct resolve_arg_impl<true>
{
    template< typename T, AUX778076_BIND_PARAMS(typename U) > struct result_
    {
        typedef typename AUX778076_APPLY<
              T
            , AUX778076_BIND_PARAMS(U)
            >::type type;
    };
};

// for 'resolve_bind_arg'
template< typename T > struct is_bind_template;

template<
      typename T, AUX778076_BIND_PARAMS(typename U)
    >
struct resolve_bind_arg
    : resolve_arg_impl< is_bind_template<T>::value >
            ::template result_< T,AUX778076_BIND_PARAMS(U) >
{
};

#   if !defined(AUTOBOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT)

template< typename T >
struct replace_unnamed_arg_impl
{
    template< typename Arg > struct result_
    {
        typedef Arg next;
        typedef T type;
    };
};

template<>
struct replace_unnamed_arg_impl< arg<-1> >
{
    template< typename Arg > struct result_
    {
        typedef typename next<Arg>::type next;
        typedef Arg type;
    };
};

template< typename T, typename Arg >
struct replace_unnamed_arg
    : replace_unnamed_arg_impl<T>::template result_<Arg>
{
};

#   endif // AUTOBOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT

// agurt, 10/mar/02: the forward declaration has to appear before any of
// 'is_bind_helper' overloads, otherwise MSVC6.5 issues an ICE on it
template< AUTOBOOST_MPL_AUX_NTTP_DECL(int, arity_) > struct bind_chooser;

aux::no_tag is_bind_helper(...);
template< typename T > aux::no_tag is_bind_helper(protect<T>*);

// overload for "main" form
// agurt, 15/mar/02: MSVC 6.5 fails to properly resolve the overload
// in case if we use 'aux::type_wrapper< bind<...> >' here, and all
// 'bind' instantiations form a complete type anyway
#if !defined(AUTOBOOST_MPL_CFG_NO_BIND_TEMPLATE)
template<
      typename F, AUX778076_BIND_PARAMS(typename T)
    >
aux::yes_tag is_bind_helper(bind<F,AUX778076_BIND_PARAMS(T)>*);
#endif

template< AUTOBOOST_MPL_AUX_NTTP_DECL(int, N) >
aux::yes_tag is_bind_helper(arg<N>*);

template< bool is_ref_ = true >
struct is_bind_template_impl
{
    template< typename T > struct result_
    {
        AUTOBOOST_STATIC_CONSTANT(bool, value = false);
    };
};

template<>
struct is_bind_template_impl<false>
{
    template< typename T > struct result_
    {
        AUTOBOOST_STATIC_CONSTANT(bool, value =
              sizeof(aux::is_bind_helper(static_cast<T*>(0)))
                == sizeof(aux::yes_tag)
            );
    };
};

template< typename T > struct is_bind_template
    : is_bind_template_impl< ::autoboost::detail::is_reference_impl<T>::value >
        ::template result_<T>
{
};

#endif // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace aux


#define AUTOBOOST_PP_ITERATION_PARAMS_1 \
    (3,(0, AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY, <autoboost/mpl/bind.hpp>))
#include AUTOBOOST_PP_ITERATE()

#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
    && !defined(AUTOBOOST_MPL_CFG_NO_TEMPLATE_TEMPLATE_PARAMETERS)
/// if_/eval_if specializations
#   define AUX778076_SPEC_NAME if_
#   define AUTOBOOST_PP_ITERATION_PARAMS_1 (3,(3, 3, <autoboost/mpl/bind.hpp>))
#   include AUTOBOOST_PP_ITERATE()

#if !defined(AUTOBOOST_MPL_CFG_DMC_AMBIGUOUS_CTPS)
#   define AUX778076_SPEC_NAME eval_if
#   define AUTOBOOST_PP_ITERATION_PARAMS_1 (3,(3, 3, <autoboost/mpl/bind.hpp>))
#   include AUTOBOOST_PP_ITERATE()
#endif
#endif

// real C++ version is already taken care of
#if defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
    && !defined(AUTOBOOST_MPL_CFG_NO_BIND_TEMPLATE)

namespace aux {
// apply_count_args
#define AUX778076_COUNT_ARGS_PREFIX bind
#define AUX778076_COUNT_ARGS_DEFAULT na
#define AUX778076_COUNT_ARGS_ARITY AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY
#include <autoboost/mpl/aux_/count_args.hpp>
}

// bind
template<
      typename F, AUX778076_BIND_PARAMS(typename T) AUX778076_DMC_PARAM()
    >
struct bind
    : aux::bind_chooser<
          aux::bind_count_args<AUX778076_BIND_PARAMS(T)>::value
        >::template result_< F,AUX778076_BIND_PARAMS(T) >::type
{
};

AUTOBOOST_MPL_AUX_ARITY_SPEC(
      AUTOBOOST_PP_INC(AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY)
    , bind
    )

AUTOBOOST_MPL_AUX_TEMPLATE_ARITY_SPEC(
      AUTOBOOST_PP_INC(AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY)
    , bind
    )


#endif // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

#   undef AUX778076_BIND_NESTED_DEFAULT_PARAMS
#   undef AUX778076_BIND_N_SPEC_PARAMS
#   undef AUX778076_BIND_N_PARAMS
#   undef AUX778076_BIND_DEFAULT_PARAMS
#   undef AUX778076_BIND_PARAMS
#   undef AUX778076_DMC_PARAM
#   undef AUX778076_APPLY

}}

#endif // AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif // AUTOBOOST_MPL_BIND_HPP_INCLUDED

///// iteration, depth == 1

// For gcc 4.4 compatability, we must include the
// AUTOBOOST_PP_ITERATION_DEPTH test inside an #else clause.
#else // AUTOBOOST_PP_IS_ITERATING
#if AUTOBOOST_PP_ITERATION_DEPTH() == 1

#   define i_ AUTOBOOST_PP_FRAME_ITERATION(1)

#if defined(AUX778076_SPEC_NAME)

// lazy metafunction specialization
template< template< AUTOBOOST_MPL_PP_PARAMS(i_, typename T) > class F, typename Tag >
struct AUTOBOOST_PP_CAT(quote,i_);

template< AUTOBOOST_MPL_PP_PARAMS(i_, typename T) > struct AUX778076_SPEC_NAME;

template<
      typename Tag AUX778076_BIND_N_PARAMS(i_, typename T)
    >
struct AUTOBOOST_PP_CAT(bind,i_)<
      AUTOBOOST_PP_CAT(quote,i_)<AUX778076_SPEC_NAME,Tag>
    AUX778076_BIND_N_PARAMS(i_,T)
    >
{
    template<
          AUX778076_BIND_NESTED_DEFAULT_PARAMS(typename U, na)
        >
    struct apply
    {
     private:
        typedef mpl::arg<1> n1;
#       define AUTOBOOST_PP_ITERATION_PARAMS_2 (3,(1, i_, <autoboost/mpl/bind.hpp>))
#       include AUTOBOOST_PP_ITERATE()

        typedef typename AUX778076_SPEC_NAME<
              typename t1::type
            , AUTOBOOST_MPL_PP_EXT_PARAMS(2, AUTOBOOST_PP_INC(i_), t)
            >::type f_;

     public:
        typedef typename f_::type type;
    };
};

#undef AUX778076_SPEC_NAME

#else // AUX778076_SPEC_NAME

template<
      typename F AUX778076_BIND_N_PARAMS(i_, typename T) AUX778076_DMC_PARAM()
    >
struct AUTOBOOST_PP_CAT(bind,i_)
{
    template<
          AUX778076_BIND_NESTED_DEFAULT_PARAMS(typename U, na)
        >
    struct apply
    {
     private:
#   if !defined(AUTOBOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT)

        typedef aux::replace_unnamed_arg< F,mpl::arg<1> > r0;
        typedef typename r0::type a0;
        typedef typename r0::next n1;
        typedef typename aux::resolve_bind_arg<a0,AUX778076_BIND_PARAMS(U)>::type f_;
        ///
#   else
        typedef typename aux::resolve_bind_arg<F,AUX778076_BIND_PARAMS(U)>::type f_;

#   endif // AUTOBOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT

#   if i_ > 0
#       define AUTOBOOST_PP_ITERATION_PARAMS_2 (3,(1, i_, <autoboost/mpl/bind.hpp>))
#       include AUTOBOOST_PP_ITERATE()
#   endif

     public:

#   define AUX778076_ARG(unused, i_, t) \
    AUTOBOOST_PP_COMMA_IF(i_) \
    typename AUTOBOOST_PP_CAT(t,AUTOBOOST_PP_INC(i_))::type \
/**/

        typedef typename AUTOBOOST_PP_CAT(apply_wrap,i_)<
              f_
            AUTOBOOST_PP_COMMA_IF(i_) AUTOBOOST_MPL_PP_REPEAT(i_, AUX778076_ARG, t)
            >::type type;

#   undef AUX778076_ARG
    };
};

namespace aux {

#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template<
      typename F AUX778076_BIND_N_PARAMS(i_, typename T), AUX778076_BIND_PARAMS(typename U)
    >
struct resolve_bind_arg<
      AUTOBOOST_PP_CAT(bind,i_)<F AUX778076_BIND_N_PARAMS(i_,T)>,AUX778076_BIND_PARAMS(U)
    >
{
    typedef AUTOBOOST_PP_CAT(bind,i_)<F AUX778076_BIND_N_PARAMS(i_,T)> f_;
    typedef typename AUX778076_APPLY<f_, AUX778076_BIND_PARAMS(U)>::type type;
};

#else

template<
      typename F AUX778076_BIND_N_PARAMS(i_, typename T)
    >
aux::yes_tag
is_bind_helper(AUTOBOOST_PP_CAT(bind,i_)<F AUX778076_BIND_N_PARAMS(i_,T)>*);

#endif // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace aux

AUTOBOOST_MPL_AUX_ARITY_SPEC(AUTOBOOST_PP_INC(i_), AUTOBOOST_PP_CAT(bind,i_))
AUTOBOOST_MPL_AUX_TEMPLATE_ARITY_SPEC(AUTOBOOST_PP_INC(i_), AUTOBOOST_PP_CAT(bind,i_))

#   if !defined(AUTOBOOST_MPL_CFG_NO_BIND_TEMPLATE)
#   if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

#if i_ == AUTOBOOST_MPL_LIMIT_METAFUNCTION_ARITY
/// primary template (not a specialization!)
template<
      typename F AUX778076_BIND_N_PARAMS(i_, typename T) AUX778076_DMC_PARAM()
    >
struct bind
    : AUTOBOOST_PP_CAT(bind,i_)<F AUX778076_BIND_N_PARAMS(i_,T) >
{
};
#else
template<
      typename F AUX778076_BIND_N_PARAMS(i_, typename T) AUX778076_DMC_PARAM()
    >
struct bind< F AUX778076_BIND_N_SPEC_PARAMS(i_, T, na) >
    : AUTOBOOST_PP_CAT(bind,i_)<F AUX778076_BIND_N_PARAMS(i_,T) >
{
};
#endif

#   else // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

namespace aux {

template<>
struct bind_chooser<i_>
{
    template<
          typename F, AUX778076_BIND_PARAMS(typename T)
        >
    struct result_
    {
        typedef AUTOBOOST_PP_CAT(bind,i_)< F AUX778076_BIND_N_PARAMS(i_,T) > type;
    };
};

} // namespace aux

#   endif // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#   endif // AUTOBOOST_MPL_CFG_NO_BIND_TEMPLATE

#endif // AUX778076_SPEC_NAME

#   undef i_

///// iteration, depth == 2

#elif AUTOBOOST_PP_ITERATION_DEPTH() == 2

#   define j_ AUTOBOOST_PP_FRAME_ITERATION(2)
#   if !defined(AUTOBOOST_MPL_CFG_NO_UNNAMED_PLACEHOLDER_SUPPORT)

        typedef aux::replace_unnamed_arg< AUTOBOOST_PP_CAT(T,j_),AUTOBOOST_PP_CAT(n,j_) > AUTOBOOST_PP_CAT(r,j_);
        typedef typename AUTOBOOST_PP_CAT(r,j_)::type AUTOBOOST_PP_CAT(a,j_);
        typedef typename AUTOBOOST_PP_CAT(r,j_)::next AUTOBOOST_PP_CAT(n,AUTOBOOST_PP_INC(j_));
        typedef aux::resolve_bind_arg<AUTOBOOST_PP_CAT(a,j_), AUX778076_BIND_PARAMS(U)> AUTOBOOST_PP_CAT(t,j_);
        ///
#   else
        typedef aux::resolve_bind_arg< AUTOBOOST_PP_CAT(T,j_),AUX778076_BIND_PARAMS(U)> AUTOBOOST_PP_CAT(t,j_);

#   endif
#   undef j_

#endif // AUTOBOOST_PP_ITERATION_DEPTH()
#endif // AUTOBOOST_PP_IS_ITERATING

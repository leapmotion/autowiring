/*=============================================================================
    Copyright (c) 2007-2008 Tobias Schwinger

    Use modification and distribution are subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
==============================================================================*/

#ifndef AUTOBOOST_FUNCTIONAL_FORWARD_ADAPTER_HPP_INCLUDED
#   ifndef AUTOBOOST_PP_IS_ITERATING

#   include <autoboost/config.hpp>
#   include <autoboost/detail/workaround.hpp>

#   include <autoboost/preprocessor/iteration/iterate.hpp>
#   include <autoboost/preprocessor/repetition/enum_params.hpp>
#   include <autoboost/preprocessor/repetition/enum_binary_params.hpp>
#   include <autoboost/preprocessor/facilities/intercept.hpp>
#   include <autoboost/preprocessor/arithmetic/dec.hpp>

#   include <autoboost/utility/result_of.hpp>

#   ifndef AUTOBOOST_FUNCTIONAL_FORWARD_ADAPTER_MAX_ARITY
#     define AUTOBOOST_FUNCTIONAL_FORWARD_ADAPTER_MAX_ARITY 6
#   elif AUTOBOOST_FUNCTIONAL_FORWARD_ADAPTER_MAX_ARITY < 3
#     undef  AUTOBOOST_FUNCTIONAL_FORWARD_ADAPTER_MAX_ARITY
#     define AUTOBOOST_FUNCTIONAL_FORWARD_ADAPTER_MAX_ARITY 3
#   endif


namespace autoboost
{
    template< typename Function, int Arity_Or_MinArity = -1, int MaxArity = -1 >
    class forward_adapter;

    //----- ---- --- -- - -  -   -

    namespace detail
    {
        template< class MostDerived, typename Function, typename FunctionConst,
            int Arity, int MinArity >
        struct forward_adapter_impl;

        struct forward_adapter_result
        {
            template< typename Sig > struct apply;

            // Utility metafunction for qualification adjustment on arguments
            template< typename T > struct q          { typedef T const t; };
            template< typename T > struct q<T const> { typedef T const t; };
            template< typename T > struct q<T &>     { typedef T       t; };

            // Utility metafunction to choose target function qualification
            template< typename T > struct c
            { typedef typename T::target_function_t t; };
            template< typename T > struct c<T&      >
            { typedef typename T::target_function_t t; };
            template< typename T > struct c<T const >
            { typedef typename T::target_function_const_t t; };
            template< typename T > struct c<T const&>
            { typedef typename T::target_function_const_t t; };
        };
    }

#   define AUTOBOOST_TMP_MACRO(f,fn,fc) \
        autoboost::detail::forward_adapter_impl< \
            forward_adapter<f,Arity_Or_MinArity,MaxArity>, fn, fc, \
            (MaxArity!=-1? MaxArity :Arity_Or_MinArity!=-1? Arity_Or_MinArity \
                :AUTOBOOST_FUNCTIONAL_FORWARD_ADAPTER_MAX_ARITY), \
            (Arity_Or_MinArity!=-1? Arity_Or_MinArity : 0) >

    template< typename Function, int Arity_Or_MinArity, int MaxArity >
    class forward_adapter
        : public AUTOBOOST_TMP_MACRO(Function,Function,Function const)
        , private Function
    {
      public:
        forward_adapter(Function const& f = Function())
          : Function(f)
        { }

        typedef Function        target_function_t;
        typedef Function const  target_function_const_t;

        Function       & target_function()       { return *this; }
        Function const & target_function() const { return *this; }

        template< typename Sig > struct result
            : detail::forward_adapter_result::template apply<Sig>
        { };

        using AUTOBOOST_TMP_MACRO(Function,Function, Function const)::operator();
    };
    template< typename Function, int Arity_Or_MinArity, int MaxArity >
    class forward_adapter< Function const, Arity_Or_MinArity, MaxArity >
        : public AUTOBOOST_TMP_MACRO(Function const, Function const, Function const)
        , private Function
    {
      public:
        forward_adapter(Function const& f = Function())
          : Function(f)
        { }

        typedef Function const target_function_t;
        typedef Function const target_function_const_t;

        Function const & target_function() const { return *this; }

        template< typename Sig > struct result
            : detail::forward_adapter_result::template apply<Sig>
        { };

        using AUTOBOOST_TMP_MACRO(Function const,Function const, Function const)
            ::operator();
    };
    template< typename Function, int Arity_Or_MinArity, int MaxArity >
    class forward_adapter< Function &, Arity_Or_MinArity, MaxArity >
        : public AUTOBOOST_TMP_MACRO(Function&, Function, Function)
    {
        Function& ref_function;
      public:
        forward_adapter(Function& f)
          : ref_function(f)
        { }

        typedef Function target_function_t;
        typedef Function target_function_const_t;

        Function & target_function() const { return this->ref_function; }

        template< typename Sig > struct result
            : detail::forward_adapter_result::template apply<Sig>
        { };

        using AUTOBOOST_TMP_MACRO(Function&, Function, Function)::operator();
    };

    #undef AUTOBOOST_TMP_MACRO

    namespace detail
    {
        template< class Self >
        struct forward_adapter_result::apply< Self() >
            : autoboost::result_of< AUTOBOOST_DEDUCED_TYPENAME c<Self>::t() >
        { };

        template< class MD, class F, class FC >
        struct forward_adapter_impl<MD,F,FC,0,0>
        {
            inline typename autoboost::result_of< FC() >::type
            operator()() const
            {
                return static_cast<MD const*>(this)->target_function()();
            }

            inline typename autoboost::result_of< F() >::type
            operator()()
            {
                return static_cast<MD*>(this)->target_function()();
            }

        // closing brace gets generated by preprocessing code, below

#       define AUTOBOOST_TMP_MACRO(tpl_params,arg_types,params,args)              \
            template< tpl_params >                                             \
            inline typename autoboost::result_of< FC(arg_types) >::type            \
            operator()(params) const                                           \
            {                                                                  \
                return static_cast<MD const*>(this)->target_function()(args);  \
            }                                                                  \
            template< tpl_params >                                             \
            inline typename autoboost::result_of< F(arg_types)>::type              \
            operator()(params)                                                 \
            {                                                                  \
                return static_cast<MD*>(this)->target_function()(args);        \
            }

#       // This is the total number of iterations we need
#       define count ((1 << AUTOBOOST_FUNCTIONAL_FORWARD_ADAPTER_MAX_ARITY+1)-2)

#       // Chain file iteration to virtually one loop
#       if AUTOBOOST_FUNCTIONAL_FORWARD_ADAPTER_MAX_ARITY <= 7
#         define limit1 count
#         define limit2 0
#         define limit3 0
#       else
#         if AUTOBOOST_FUNCTIONAL_FORWARD_ADAPTER_MAX_ARITY <= 15
#           define limit1 (count >> 8)
#           define limit2 255
#           define limit3 0
#         else
#           define limit1 (count >> 16)
#           define limit2 255
#           define limit3 255
#         endif
#       endif

#       define N 0

#       define  AUTOBOOST_PP_FILENAME_1 <autoboost/functional/forward_adapter.hpp>
#       define  AUTOBOOST_PP_ITERATION_LIMITS (0,limit1)
#       include AUTOBOOST_PP_ITERATE()

#       undef N
#       undef limit3
#       undef limit2
#       undef limit1
#       undef count
#       undef AUTOBOOST_TMP_MACRO

        };

    } // namespace detail

    template<class F, int A0, int A1>
    struct result_of<autoboost::forward_adapter<F,A0,A1> const ()>
        : autoboost::detail::forward_adapter_result::template apply<
            autoboost::forward_adapter<F,A0,A1> const () >
    { };
    template<class F, int A0, int A1>
    struct result_of<autoboost::forward_adapter<F,A0,A1>()>
        : autoboost::detail::forward_adapter_result::template apply<
            autoboost::forward_adapter<F,A0,A1>() >
    { };
    template<class F, int A0, int A1>
    struct result_of<autoboost::forward_adapter<F,A0,A1> const& ()>
        : autoboost::detail::forward_adapter_result::template apply<
            autoboost::forward_adapter<F,A0,A1> const () >
    { };
    template<class F, int A0, int A1>
    struct result_of<autoboost::forward_adapter<F,A0,A1>& ()>
        : autoboost::detail::forward_adapter_result::template apply<
            autoboost::forward_adapter<F,A0,A1>() >
    { };
}

#       define AUTOBOOST_FUNCTIONAL_FORWARD_ADAPTER_HPP_INCLUDED

#   elif AUTOBOOST_PP_ITERATION_DEPTH() == 1 && limit2
#     define  AUTOBOOST_PP_FILENAME_2 <autoboost/functional/forward_adapter.hpp>
#     define  AUTOBOOST_PP_ITERATION_LIMITS (0,limit2)
#     include AUTOBOOST_PP_ITERATE()
#   elif AUTOBOOST_PP_ITERATION_DEPTH() == 2 && limit3
#     define  AUTOBOOST_PP_FILENAME_3 <autoboost/functional/forward_adapter.hpp>
#     define  AUTOBOOST_PP_ITERATION_LIMITS (0,limit3)
#     include AUTOBOOST_PP_ITERATE()

#   else

#     // I is the loop counter
#     if limit2 && limit3
#       define I (AUTOBOOST_PP_ITERATION_1 << 16 | AUTOBOOST_PP_ITERATION_2 << 8 | \
            AUTOBOOST_PP_ITERATION_3)
#     elif limit2
#       define I (AUTOBOOST_PP_ITERATION_1 << 8 | AUTOBOOST_PP_ITERATION_2)
#     else
#       define I AUTOBOOST_PP_ITERATION_1
#     endif

#     if I < count

#       // Done for this arity? Increment N
#       if (I+2 >> N+1)
#         if N == 0
#           undef N
#           define N 1
#         elif N == 1
#           undef N
#           define N 2
#         elif N == 2
#           undef N
#           define N 3
#         elif N == 3
#           undef N
#           define N 4
#         elif N == 4
#           undef N
#           define N 5
#         elif N == 5
#           undef N
#           define N 6
#         elif N == 6
#           undef N
#           define N 7
#         elif N == 7
#           undef N
#           define N 8
#         elif N == 8
#           undef N
#           define N 9
#         elif N == 9
#           undef N
#           define N 10
#         elif N == 10
#           undef N
#           define N 11
#         elif N == 11
#           undef N
#           define N 12
#         elif N == 12
#           undef N
#           define N 13
#         elif N == 13
#           undef N
#           define N 14
#         elif N == 14
#           undef N
#           define N 15
#         elif N == 15
#           undef N
#           define N 16
#         endif

        };

        template< class Self, AUTOBOOST_PP_ENUM_PARAMS(N,typename T) >
        struct forward_adapter_result::apply< Self(AUTOBOOST_PP_ENUM_PARAMS(N,T)) >
            : autoboost::result_of<
                AUTOBOOST_DEDUCED_TYPENAME c<Self>::t(AUTOBOOST_PP_ENUM_BINARY_PARAMS(N,
                      typename q<T,>::t& AUTOBOOST_PP_INTERCEPT)) >
        { };

        template< class MD, class F, class FC >
        struct forward_adapter_impl<MD,F,FC,AUTOBOOST_PP_DEC(N),N>
        {
            template< AUTOBOOST_PP_ENUM_PARAMS(N,typename T) >
            inline typename autoboost::result_of< F(
                AUTOBOOST_PP_ENUM_BINARY_PARAMS(N,T,& AUTOBOOST_PP_INTERCEPT)) >::type
            operator()(AUTOBOOST_PP_ENUM_BINARY_PARAMS(N,T,& AUTOBOOST_PP_INTERCEPT));
        };

        template< class MD, class F, class FC, int MinArity >
        struct forward_adapter_impl<MD,F,FC,N,MinArity>
            : forward_adapter_impl<MD,F,FC,AUTOBOOST_PP_DEC(N),MinArity>
        {
            using forward_adapter_impl<MD,F,FC,AUTOBOOST_PP_DEC(N),MinArity>::operator();

#       endif

#       // Zero based count for each arity would be I-(1<<N)+2, but we don't
#       // need it, unless we need a nicer order.

#       // Macros for the parameter's type modifiers.
#       if I & 0x000001
#         define PT0 T0 &
#       else
#         define PT0 T0 const &
#       endif
#       if I & 0x000002
#         define PT1 T1 &
#       else
#         define PT1 T1 const &
#       endif
#       if I & 0x000004
#         define PT2 T2 &
#       else
#         define PT2 T2 const &
#       endif
#       if I & 0x000008
#         define PT3 T3 &
#       else
#         define PT3 T3 const &
#       endif
#       if I & 0x000010
#         define PT4 T4 &
#       else
#         define PT4 T4 const &
#       endif
#       if I & 0x000020
#         define PT5 T5 &
#       else
#         define PT5 T5 const &
#       endif
#       if I & 0x000040
#         define PT6 T6 &
#       else
#         define PT6 T6 const &
#       endif
#       if I & 0x000080
#         define PT7 T7 &
#       else
#         define PT7 T7 const &
#       endif
#       if I & 0x000100
#         define PT8 T8 &
#       else
#         define PT8 T8 const &
#       endif
#       if I & 0x000200
#         define PT9 T9 &
#       else
#         define PT9 T9 const &
#       endif
#       if I & 0x000400
#         define PT10 T10 &
#       else
#         define PT10 T10 const &
#       endif
#       if I & 0x000800
#         define PT11 T11 &
#       else
#         define PT11 T11 const &
#       endif
#       if I & 0x001000
#         define PT12 T12 &
#       else
#         define PT12 T12 const &
#       endif
#       if I & 0x002000
#         define PT13 T13 &
#       else
#         define PT13 T13 const &
#       endif
#       if I & 0x004000
#         define PT14 T14 &
#       else
#         define PT14 T14 const &
#       endif
#       if I & 0x008000
#         define PT15 T15 &
#       else
#         define PT15 T15 const &
#       endif

#       if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC,AUTOBOOST_TESTED_AT(1400))
            template< AUTOBOOST_PP_ENUM_PARAMS(N,typename T) >
            inline typename autoboost::result_of<  FC(AUTOBOOST_PP_ENUM_PARAMS(N,PT))
                >::type
            operator()(AUTOBOOST_PP_ENUM_BINARY_PARAMS(N,PT,a)) const
            {
                return static_cast<MD const* const>(this)
                    ->target_function()(AUTOBOOST_PP_ENUM_PARAMS(N,a));
            }
            template< AUTOBOOST_PP_ENUM_PARAMS(N,typename T) >
            inline typename autoboost::result_of<  F(AUTOBOOST_PP_ENUM_PARAMS(N,PT))
                >::type
            operator()(AUTOBOOST_PP_ENUM_BINARY_PARAMS(N,PT,a))
            {
                return static_cast<MD* const>(this)
                    ->target_function()(AUTOBOOST_PP_ENUM_PARAMS(N,a));
            }
#       else
        AUTOBOOST_TMP_MACRO(AUTOBOOST_PP_ENUM_PARAMS(N,typename T),
            AUTOBOOST_PP_ENUM_PARAMS(N,PT), AUTOBOOST_PP_ENUM_BINARY_PARAMS(N,PT,a),
            AUTOBOOST_PP_ENUM_PARAMS(N,a) )
        // ...generates uglier code but is faster - it caches ENUM_*
#       endif

#       undef PT0
#       undef PT1
#       undef PT2
#       undef PT3
#       undef PT4
#       undef PT5
#       undef PT6
#       undef PT7
#       undef PT8
#       undef PT9
#       undef PT10
#       undef PT11
#       undef PT12
#       undef PT13
#       undef PT14
#       undef PT15

#     endif // I < count

#     undef I
#   endif // defined(AUTOBOOST_PP_IS_ITERATING)

#endif // include guard


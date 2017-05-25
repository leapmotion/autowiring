/*=============================================================================
    Copyright (c) 2007 Tobias Schwinger

    Use modification and distribution are subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
==============================================================================*/

#ifndef AUTOBOOST_FUNCTIONAL_VALUE_FACTORY_HPP_INCLUDED
#   ifndef AUTOBOOST_PP_IS_ITERATING

#     include <autoboost/preprocessor/iteration/iterate.hpp>
#     include <autoboost/preprocessor/repetition/enum_params.hpp>
#     include <autoboost/preprocessor/repetition/enum_binary_params.hpp>

#     include <new>
#     include <autoboost/pointee.hpp>
#     include <autoboost/get_pointer.hpp>
#     include <autoboost/non_type.hpp>
#     include <autoboost/type_traits/remove_cv.hpp>

#     ifndef AUTOBOOST_FUNCTIONAL_VALUE_FACTORY_MAX_ARITY
#       define AUTOBOOST_FUNCTIONAL_VALUE_FACTORY_MAX_ARITY 10
#     elif AUTOBOOST_FUNCTIONAL_VALUE_FACTORY_MAX_ARITY < 3
#       undef  AUTOBOOST_FUNCTIONAL_VALUE_FACTORY_MAX_ARITY
#       define AUTOBOOST_FUNCTIONAL_VALUE_FACTORY_MAX_ARITY 3
#     endif

namespace autoboost
{
    template< typename T >
    class value_factory;

    //----- ---- --- -- - -  -   -

    template< typename T >
    class value_factory
    {
      public:
        typedef T result_type;

        value_factory()
        { }

#     define AUTOBOOST_PP_FILENAME_1 <autoboost/functional/value_factory.hpp>
#     define AUTOBOOST_PP_ITERATION_LIMITS (0,AUTOBOOST_FUNCTIONAL_VALUE_FACTORY_MAX_ARITY)
#     include AUTOBOOST_PP_ITERATE()
    };

    template< typename T > class value_factory<T&>;
    // forbidden, would create a dangling reference
}
#     define AUTOBOOST_FUNCTIONAL_VALUE_FACTORY_HPP_INCLUDED
#   else // defined(AUTOBOOST_PP_IS_ITERATING)

#     define N AUTOBOOST_PP_ITERATION()
#     if N > 0
    template< AUTOBOOST_PP_ENUM_PARAMS(N, typename T) >
#     endif
    inline result_type operator()(AUTOBOOST_PP_ENUM_BINARY_PARAMS(N,T,& a)) const
    {
        return result_type(AUTOBOOST_PP_ENUM_PARAMS(N,a));
    }
#     undef N

#   endif // defined(AUTOBOOST_PP_IS_ITERATING)

#endif // include guard


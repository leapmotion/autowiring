// Copyright David Abrahams 2005. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef AUTOBOOST_DETAIL_IS_XXX_DWA20051011_HPP
# define AUTOBOOST_DETAIL_IS_XXX_DWA20051011_HPP

# include <autoboost/config.hpp>
# include <autoboost/mpl/bool.hpp>
# include <autoboost/preprocessor/enum_params.hpp>


#  define AUTOBOOST_DETAIL_IS_XXX_DEF(name, qualified_name, nargs)  \
template <class T>                                              \
struct is_##name : mpl::false_                                  \
{                                                               \
};                                                              \
                                                                \
template < AUTOBOOST_PP_ENUM_PARAMS_Z(1, nargs, class T) >          \
struct is_##name<                                               \
   qualified_name< AUTOBOOST_PP_ENUM_PARAMS_Z(1, nargs, T) >        \
>                                                               \
   : mpl::true_                                                 \
{                                                               \
};


#endif // AUTOBOOST_DETAIL_IS_XXX_DWA20051011_HPP

# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2003.
#  *     (C) Copyright Edward Diener 2014.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_FACILITIES_IS_EMPTY_HPP
# define AUTOBOOST_PREPROCESSOR_FACILITIES_IS_EMPTY_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# if AUTOBOOST_PP_VARIADICS
#
# include <autoboost/preprocessor/facilities/is_empty_variadic.hpp>
#
# else
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC() && ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/preprocessor/facilities/identity.hpp>
# else
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/detail/split.hpp>
# endif
#
# /* AUTOBOOST_PP_IS_EMPTY */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC() && ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_IS_EMPTY(x) AUTOBOOST_PP_IS_EMPTY_I(x AUTOBOOST_PP_IS_EMPTY_HELPER)
#    define AUTOBOOST_PP_IS_EMPTY_I(contents) AUTOBOOST_PP_TUPLE_ELEM(2, 1, (AUTOBOOST_PP_IS_EMPTY_DEF_ ## contents()))
#    define AUTOBOOST_PP_IS_EMPTY_DEF_AUTOBOOST_PP_IS_EMPTY_HELPER 1, AUTOBOOST_PP_IDENTITY(1)
#    define AUTOBOOST_PP_IS_EMPTY_HELPER() , 0
# else
#    if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#        define AUTOBOOST_PP_IS_EMPTY(x) AUTOBOOST_PP_IS_EMPTY_I(AUTOBOOST_PP_IS_EMPTY_HELPER x ())
#        define AUTOBOOST_PP_IS_EMPTY_I(test) AUTOBOOST_PP_IS_EMPTY_II(AUTOBOOST_PP_SPLIT(0, AUTOBOOST_PP_CAT(AUTOBOOST_PP_IS_EMPTY_DEF_, test)))
#        define AUTOBOOST_PP_IS_EMPTY_II(id) id
#    else
#        define AUTOBOOST_PP_IS_EMPTY(x) AUTOBOOST_PP_IS_EMPTY_I((AUTOBOOST_PP_IS_EMPTY_HELPER x ()))
#        define AUTOBOOST_PP_IS_EMPTY_I(par) AUTOBOOST_PP_IS_EMPTY_II ## par
#        define AUTOBOOST_PP_IS_EMPTY_II(test) AUTOBOOST_PP_SPLIT(0, AUTOBOOST_PP_CAT(AUTOBOOST_PP_IS_EMPTY_DEF_, test))
#    endif
#    define AUTOBOOST_PP_IS_EMPTY_HELPER() 1
#    define AUTOBOOST_PP_IS_EMPTY_DEF_1 1, AUTOBOOST_PP_NIL
#    define AUTOBOOST_PP_IS_EMPTY_DEF_AUTOBOOST_PP_IS_EMPTY_HELPER 0, AUTOBOOST_PP_NIL
# endif
#
# endif /* AUTOBOOST_PP_VARIADICS */
#
# endif /* AUTOBOOST_PREPROCESSOR_FACILITIES_IS_EMPTY_HPP */

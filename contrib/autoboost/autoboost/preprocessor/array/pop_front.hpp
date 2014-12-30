# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_ARRAY_POP_FRONT_HPP
# define AUTOBOOST_PREPROCESSOR_ARRAY_POP_FRONT_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/array/elem.hpp>
# include <autoboost/preprocessor/array/size.hpp>
# include <autoboost/preprocessor/repetition/enum.hpp>
# include <autoboost/preprocessor/repetition/deduce_z.hpp>
#
# /* AUTOBOOST_PP_ARRAY_POP_FRONT */
#
# define AUTOBOOST_PP_ARRAY_POP_FRONT(array) AUTOBOOST_PP_ARRAY_POP_FRONT_Z(AUTOBOOST_PP_DEDUCE_Z(), array)
#
# /* AUTOBOOST_PP_ARRAY_POP_FRONT_Z */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ARRAY_POP_FRONT_Z(z, array) AUTOBOOST_PP_ARRAY_POP_FRONT_I(z, AUTOBOOST_PP_ARRAY_SIZE(array), array)
# else
#    define AUTOBOOST_PP_ARRAY_POP_FRONT_Z(z, array) AUTOBOOST_PP_ARRAY_POP_FRONT_Z_D(z, array)
#    define AUTOBOOST_PP_ARRAY_POP_FRONT_Z_D(z, array) AUTOBOOST_PP_ARRAY_POP_FRONT_I(z, AUTOBOOST_PP_ARRAY_SIZE(array), array)
# endif
#
# define AUTOBOOST_PP_ARRAY_POP_FRONT_I(z, size, array) (AUTOBOOST_PP_DEC(size), (AUTOBOOST_PP_ENUM_ ## z(AUTOBOOST_PP_DEC(size), AUTOBOOST_PP_ARRAY_POP_FRONT_M, array)))
# define AUTOBOOST_PP_ARRAY_POP_FRONT_M(z, n, data) AUTOBOOST_PP_ARRAY_ELEM(AUTOBOOST_PP_INC(n), data)
#
# endif

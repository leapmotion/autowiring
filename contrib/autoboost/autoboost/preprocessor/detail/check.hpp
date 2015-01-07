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
# ifndef AUTOBOOST_PREPROCESSOR_DETAIL_CHECK_HPP
# define AUTOBOOST_PREPROCESSOR_DETAIL_CHECK_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/config/config.hpp>
#
# /* AUTOBOOST_PP_CHECK */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_CHECK(x, type) AUTOBOOST_PP_CHECK_D(x, type)
# else
#    define AUTOBOOST_PP_CHECK(x, type) AUTOBOOST_PP_CHECK_OO((x, type))
#    define AUTOBOOST_PP_CHECK_OO(par) AUTOBOOST_PP_CHECK_D ## par
# endif
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC() && ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_DMC()
#    define AUTOBOOST_PP_CHECK_D(x, type) AUTOBOOST_PP_CHECK_1(AUTOBOOST_PP_CAT(AUTOBOOST_PP_CHECK_RESULT_, type x))
#    define AUTOBOOST_PP_CHECK_1(chk) AUTOBOOST_PP_CHECK_2(chk)
#    define AUTOBOOST_PP_CHECK_2(res, _) res
# elif AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#    define AUTOBOOST_PP_CHECK_D(x, type) AUTOBOOST_PP_CHECK_1(type x)
#    define AUTOBOOST_PP_CHECK_1(chk) AUTOBOOST_PP_CHECK_2(chk)
#    define AUTOBOOST_PP_CHECK_2(chk) AUTOBOOST_PP_CHECK_3((AUTOBOOST_PP_CHECK_RESULT_ ## chk))
#    define AUTOBOOST_PP_CHECK_3(im) AUTOBOOST_PP_CHECK_5(AUTOBOOST_PP_CHECK_4 im)
#    define AUTOBOOST_PP_CHECK_4(res, _) res
#    define AUTOBOOST_PP_CHECK_5(res) res
# else /* DMC */
#    define AUTOBOOST_PP_CHECK_D(x, type) AUTOBOOST_PP_CHECK_OO((type x))
#    define AUTOBOOST_PP_CHECK_OO(par) AUTOBOOST_PP_CHECK_0 ## par
#    define AUTOBOOST_PP_CHECK_0(chk) AUTOBOOST_PP_CHECK_1(AUTOBOOST_PP_CAT(AUTOBOOST_PP_CHECK_RESULT_, chk))
#    define AUTOBOOST_PP_CHECK_1(chk) AUTOBOOST_PP_CHECK_2(chk)
#    define AUTOBOOST_PP_CHECK_2(res, _) res
# endif
#
# define AUTOBOOST_PP_CHECK_RESULT_1 1, AUTOBOOST_PP_NIL
#
# endif

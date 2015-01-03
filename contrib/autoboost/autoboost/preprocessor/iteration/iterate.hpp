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
# ifndef AUTOBOOST_PREPROCESSOR_ITERATION_ITERATE_HPP
# define AUTOBOOST_PREPROCESSOR_ITERATION_ITERATE_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/array/elem.hpp>
# include <autoboost/preprocessor/array/size.hpp>
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/slot/slot.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_ITERATION_DEPTH */
#
# define AUTOBOOST_PP_ITERATION_DEPTH() 0
#
# /* AUTOBOOST_PP_ITERATION */
#
# define AUTOBOOST_PP_ITERATION() AUTOBOOST_PP_CAT(AUTOBOOST_PP_ITERATION_, AUTOBOOST_PP_ITERATION_DEPTH())
#
# /* AUTOBOOST_PP_ITERATION_START && AUTOBOOST_PP_ITERATION_FINISH */
#
# define AUTOBOOST_PP_ITERATION_START() AUTOBOOST_PP_CAT(AUTOBOOST_PP_ITERATION_START_, AUTOBOOST_PP_ITERATION_DEPTH())
# define AUTOBOOST_PP_ITERATION_FINISH() AUTOBOOST_PP_CAT(AUTOBOOST_PP_ITERATION_FINISH_, AUTOBOOST_PP_ITERATION_DEPTH())
#
# /* AUTOBOOST_PP_ITERATION_FLAGS */
#
# define AUTOBOOST_PP_ITERATION_FLAGS() (AUTOBOOST_PP_CAT(AUTOBOOST_PP_ITERATION_FLAGS_, AUTOBOOST_PP_ITERATION_DEPTH())())
#
# /* AUTOBOOST_PP_FRAME_ITERATION */
#
# define AUTOBOOST_PP_FRAME_ITERATION(i) AUTOBOOST_PP_CAT(AUTOBOOST_PP_ITERATION_, i)
#
# /* AUTOBOOST_PP_FRAME_START && AUTOBOOST_PP_FRAME_FINISH */
#
# define AUTOBOOST_PP_FRAME_START(i) AUTOBOOST_PP_CAT(AUTOBOOST_PP_ITERATION_START_, i)
# define AUTOBOOST_PP_FRAME_FINISH(i) AUTOBOOST_PP_CAT(AUTOBOOST_PP_ITERATION_FINISH_, i)
#
# /* AUTOBOOST_PP_FRAME_FLAGS */
#
# define AUTOBOOST_PP_FRAME_FLAGS(i) (AUTOBOOST_PP_CAT(AUTOBOOST_PP_ITERATION_FLAGS_, i)())
#
# /* AUTOBOOST_PP_RELATIVE_ITERATION */
#
# define AUTOBOOST_PP_RELATIVE_ITERATION(i) AUTOBOOST_PP_CAT(AUTOBOOST_PP_RELATIVE_, i)(AUTOBOOST_PP_ITERATION_)
#
# define AUTOBOOST_PP_RELATIVE_0(m) AUTOBOOST_PP_CAT(m, AUTOBOOST_PP_ITERATION_DEPTH())
# define AUTOBOOST_PP_RELATIVE_1(m) AUTOBOOST_PP_CAT(m, AUTOBOOST_PP_DEC(AUTOBOOST_PP_ITERATION_DEPTH()))
# define AUTOBOOST_PP_RELATIVE_2(m) AUTOBOOST_PP_CAT(m, AUTOBOOST_PP_DEC(AUTOBOOST_PP_DEC(AUTOBOOST_PP_ITERATION_DEPTH())))
# define AUTOBOOST_PP_RELATIVE_3(m) AUTOBOOST_PP_CAT(m, AUTOBOOST_PP_DEC(AUTOBOOST_PP_DEC(AUTOBOOST_PP_DEC(AUTOBOOST_PP_ITERATION_DEPTH()))))
# define AUTOBOOST_PP_RELATIVE_4(m) AUTOBOOST_PP_CAT(m, AUTOBOOST_PP_DEC(AUTOBOOST_PP_DEC(AUTOBOOST_PP_DEC(AUTOBOOST_PP_DEC(AUTOBOOST_PP_ITERATION_DEPTH())))))
#
# /* AUTOBOOST_PP_RELATIVE_START && AUTOBOOST_PP_RELATIVE_FINISH */
#
# define AUTOBOOST_PP_RELATIVE_START(i) AUTOBOOST_PP_CAT(AUTOBOOST_PP_RELATIVE_, i)(AUTOBOOST_PP_ITERATION_START_)
# define AUTOBOOST_PP_RELATIVE_FINISH(i) AUTOBOOST_PP_CAT(AUTOBOOST_PP_RELATIVE_, i)(AUTOBOOST_PP_ITERATION_FINISH_)
#
# /* AUTOBOOST_PP_RELATIVE_FLAGS */
#
# define AUTOBOOST_PP_RELATIVE_FLAGS(i) (AUTOBOOST_PP_CAT(AUTOBOOST_PP_RELATIVE_, i)(AUTOBOOST_PP_ITERATION_FLAGS_)())
#
# /* AUTOBOOST_PP_ITERATE */
#
# define AUTOBOOST_PP_ITERATE() AUTOBOOST_PP_CAT(AUTOBOOST_PP_ITERATE_, AUTOBOOST_PP_INC(AUTOBOOST_PP_ITERATION_DEPTH()))
#
# define AUTOBOOST_PP_ITERATE_1 <autoboost/preprocessor/iteration/detail/iter/forward1.hpp>
# define AUTOBOOST_PP_ITERATE_2 <autoboost/preprocessor/iteration/detail/iter/forward2.hpp>
# define AUTOBOOST_PP_ITERATE_3 <autoboost/preprocessor/iteration/detail/iter/forward3.hpp>
# define AUTOBOOST_PP_ITERATE_4 <autoboost/preprocessor/iteration/detail/iter/forward4.hpp>
# define AUTOBOOST_PP_ITERATE_5 <autoboost/preprocessor/iteration/detail/iter/forward5.hpp>
#
# endif

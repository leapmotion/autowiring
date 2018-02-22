# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2016.                                    *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_SEQ_DETAIL_TO_LIST_MSVC_HPP
# define AUTOBOOST_PREPROCESSOR_SEQ_DETAIL_TO_LIST_MSVC_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
#
# if AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# define AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_RESULT(state) \
    AUTOBOOST_PP_TUPLE_ELEM(2, 0, state) \
/**/
# define AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_SIZE(state) \
    AUTOBOOST_PP_TUPLE_ELEM(2, 1, state) \
/**/
# define AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC_PRED(d,state) \
    AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_SIZE(state) \
/**/
# define AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC_OP(d,state) \
    ( \
    AUTOBOOST_PP_CAT(AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_RESULT(state),), \
    AUTOBOOST_PP_DEC(AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_SIZE(state)) \
    ) \
/**/
#
# /* AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC */
#
# define AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC(result,seqsize) \
    AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_RESULT \
        ( \
        AUTOBOOST_PP_WHILE \
            ( \
            AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC_PRED, \
            AUTOBOOST_PP_SEQ_DETAIL_TO_LIST_MSVC_OP, \
            (result,seqsize) \
            ) \
        ) \
/**/
# endif // AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MSVC()
#
# endif // AUTOBOOST_PREPROCESSOR_SEQ_DETAIL_TO_LIST_MSVC_HPP

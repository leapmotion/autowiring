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
# ifndef AUTOBOOST_PREPROCESSOR_SLOT_SLOT_HPP
# define AUTOBOOST_PREPROCESSOR_SLOT_SLOT_HPP
#
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/slot/detail/def.hpp>
#
# /* AUTOBOOST_PP_ASSIGN_SLOT */
#
# define AUTOBOOST_PP_ASSIGN_SLOT(i) AUTOBOOST_PP_CAT(AUTOBOOST_PP_ASSIGN_SLOT_, i)
#
# define AUTOBOOST_PP_ASSIGN_SLOT_1 <autoboost/preprocessor/slot/detail/slot1.hpp>
# define AUTOBOOST_PP_ASSIGN_SLOT_2 <autoboost/preprocessor/slot/detail/slot2.hpp>
# define AUTOBOOST_PP_ASSIGN_SLOT_3 <autoboost/preprocessor/slot/detail/slot3.hpp>
# define AUTOBOOST_PP_ASSIGN_SLOT_4 <autoboost/preprocessor/slot/detail/slot4.hpp>
# define AUTOBOOST_PP_ASSIGN_SLOT_5 <autoboost/preprocessor/slot/detail/slot5.hpp>
#
# /* AUTOBOOST_PP_SLOT */
#
# define AUTOBOOST_PP_SLOT(i) AUTOBOOST_PP_CAT(AUTOBOOST_PP_SLOT_, i)()
#
# endif

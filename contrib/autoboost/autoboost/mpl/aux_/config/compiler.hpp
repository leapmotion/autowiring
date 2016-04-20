
#ifndef AUTOBOOST_MPL_AUX_CONFIG_COMPILER_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_COMPILER_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2008
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#if !defined(AUTOBOOST_MPL_CFG_COMPILER_DIR)

#   include <autoboost/mpl/aux_/config/dtp.hpp>
#   include <autoboost/mpl/aux_/config/ttp.hpp>
#   include <autoboost/mpl/aux_/config/ctps.hpp>
#   include <autoboost/mpl/aux_/config/msvc.hpp>
#   include <autoboost/mpl/aux_/config/gcc.hpp>
#   include <autoboost/mpl/aux_/config/workaround.hpp>

#   if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
#       define AUTOBOOST_MPL_CFG_COMPILER_DIR msvc60

#   elif AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, == 1300)
#       define AUTOBOOST_MPL_CFG_COMPILER_DIR msvc70

#   elif AUTOBOOST_WORKAROUND(AUTOBOOST_MPL_CFG_GCC, AUTOBOOST_TESTED_AT(0x0304))
#       define AUTOBOOST_MPL_CFG_COMPILER_DIR gcc

#   elif AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x610))
#       if !defined(AUTOBOOST_MPL_CFG_NO_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
#           define AUTOBOOST_MPL_CFG_COMPILER_DIR bcc551
#       elif AUTOBOOST_WORKAROUND(__BORLANDC__, >= 0x590)
#           define AUTOBOOST_MPL_CFG_COMPILER_DIR bcc
#       else
#           define AUTOBOOST_MPL_CFG_COMPILER_DIR bcc_pre590
#       endif

#   elif AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840))
#       define AUTOBOOST_MPL_CFG_COMPILER_DIR dmc

#   elif defined(__MWERKS__)
#       if defined(AUTOBOOST_MPL_CFG_BROKEN_DEFAULT_PARAMETERS_IN_NESTED_TEMPLATES)
#           define AUTOBOOST_MPL_CFG_COMPILER_DIR mwcw
#       else
#           define AUTOBOOST_MPL_CFG_COMPILER_DIR plain
#       endif

#   elif defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
#       define AUTOBOOST_MPL_CFG_COMPILER_DIR no_ctps

#   elif defined(AUTOBOOST_MPL_CFG_NO_TEMPLATE_TEMPLATE_PARAMETERS)
#       define AUTOBOOST_MPL_CFG_COMPILER_DIR no_ttp

#   else
#       define AUTOBOOST_MPL_CFG_COMPILER_DIR plain
#   endif

#endif // AUTOBOOST_MPL_CFG_COMPILER_DIR

#endif // AUTOBOOST_MPL_AUX_CONFIG_COMPILER_HPP_INCLUDED

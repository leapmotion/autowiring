/*
 *
 * Copyright (c) 1998-2000
 * Dr John Maddock
 *
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org/libs/regex for documentation.
  *   FILE         regex.h
  *   VERSION      3.12
  *   DESCRIPTION: Declares POSIX API functions
  */

#ifndef AUTOBOOST_RE_REGEX_H
#define AUTOBOOST_RE_REGEX_H

#include <autoboost/cregex.hpp>

/*
*  add using declarations to bring POSIX API functions into
* global scope, only if this is C++ (and not C).
*/
#ifdef __cplusplus

using autoboost::regoff_t;
using autoboost::regex_tA;
using autoboost::regmatch_t;
using autoboost::REG_BASIC;
using autoboost::REG_EXTENDED;
using autoboost::REG_ICASE;
using autoboost::REG_NOSUB;
using autoboost::REG_NEWLINE;
using autoboost::REG_NOSPEC;
using autoboost::REG_PEND;
using autoboost::REG_DUMP;
using autoboost::REG_NOCOLLATE;
using autoboost::REG_ESCAPE_IN_LISTS;
using autoboost::REG_NEWLINE_ALT;
using autoboost::REG_PERL;
using autoboost::REG_AWK;
using autoboost::REG_GREP;
using autoboost::REG_EGREP;
using autoboost::REG_ASSERT;
using autoboost::REG_INVARG;
using autoboost::REG_ATOI;
using autoboost::REG_ITOA;

using autoboost::REG_NOTBOL;
using autoboost::REG_NOTEOL;
using autoboost::REG_STARTEND;

using autoboost::reg_comp_flags;
using autoboost::reg_exec_flags;
using autoboost::regcompA;
using autoboost::regerrorA;
using autoboost::regexecA;
using autoboost::regfreeA;

#ifndef AUTOBOOST_NO_WREGEX
using autoboost::regcompW;
using autoboost::regerrorW;
using autoboost::regexecW;
using autoboost::regfreeW;
using autoboost::regex_tW;
#endif

using autoboost::REG_NOERROR;
using autoboost::REG_NOMATCH;
using autoboost::REG_BADPAT;
using autoboost::REG_ECOLLATE;
using autoboost::REG_ECTYPE;
using autoboost::REG_EESCAPE;
using autoboost::REG_ESUBREG;
using autoboost::REG_EBRACK;
using autoboost::REG_EPAREN;
using autoboost::REG_EBRACE;
using autoboost::REG_BADBR;
using autoboost::REG_ERANGE;
using autoboost::REG_ESPACE;
using autoboost::REG_BADRPT;
using autoboost::REG_EEND;
using autoboost::REG_ESIZE;
using autoboost::REG_ERPAREN;
using autoboost::REG_EMPTY;
using autoboost::REG_E_MEMORY;
using autoboost::REG_E_UNKNOWN;
using autoboost::reg_errcode_t;

#endif /* __cplusplus */

#endif /* AUTOBOOST_RE_REGEX_H */





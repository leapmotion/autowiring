/*
 *
 * Copyright (c) 1998-2004
 * John Maddock
 *
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE:        regex.cpp
  *   VERSION:     see <autoboost/version.hpp>
  *   DESCRIPTION: Misc autoboost::regbase member funnctions.
  */


#define AUTOBOOST_REGEX_SOURCE

#include <autoboost/config.hpp>
#include <new>
#include <autoboost/regex.hpp>
#include <autoboost/throw_exception.hpp>

#if defined(AUTOBOOST_REGEX_HAS_MS_STACK_GUARD) && defined(_MSC_VER) && (_MSC_VER >= 1300)
#  include <malloc.h>
#endif
#ifdef AUTOBOOST_REGEX_HAS_MS_STACK_GUARD
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#define NOGDI
#define NOUSER
#include <windows.h>
#endif

#if defined(AUTOBOOST_REGEX_NON_RECURSIVE) && !defined(AUTOBOOST_REGEX_V3)
#if AUTOBOOST_REGEX_MAX_CACHE_BLOCKS == 0
#include <new>
#else
#include <autoboost/regex/v4/mem_block_cache.hpp>
#endif
#endif

#ifdef AUTOBOOST_INTEL
#pragma warning(disable:383)
#endif

namespace autoboost{

//
// fix: these are declared out of line here to ensure
// that dll builds contain the Virtual table for these
// types - this ensures that exceptions can be thrown
// from the dll and caught in an exe.
regex_error::regex_error(const std::string& s, regex_constants::error_type err, std::ptrdiff_t pos)
   : std::runtime_error(s)
   , m_error_code(err)
   , m_position(pos)
{
}

regex_error::regex_error(regex_constants::error_type err)
   : std::runtime_error(::autoboost::re_detail::get_default_error_string(err))
   , m_error_code(err)
   , m_position(0)
{
}

regex_error::~regex_error() throw()
{
}

void regex_error::raise()const
{
#ifndef AUTOBOOST_NO_EXCEPTIONS
   ::autoboost::throw_exception(*this);
#endif
}



namespace re_detail{

AUTOBOOST_REGEX_DECL void AUTOBOOST_REGEX_CALL raise_runtime_error(const std::runtime_error& ex)
{
   ::autoboost::throw_exception(ex);
}
//
// error checking API:
//
AUTOBOOST_REGEX_DECL void AUTOBOOST_REGEX_CALL verify_options(autoboost::regex::flag_type /*ef*/, match_flag_type mf)
{
#ifndef AUTOBOOST_REGEX_V3
   //
   // can't mix match_extra with POSIX matching rules:
   //
   if((mf & match_extra) && (mf & match_posix))
   {
      std::logic_error msg("Usage Error: Can't mix regular expression captures with POSIX matching rules");
      throw_exception(msg);
   }
#endif
}

#ifdef AUTOBOOST_REGEX_HAS_MS_STACK_GUARD

static void execute_eror()
{
   // we only get here after a stack overflow,
   // this has to be a separate proceedure because we
   // can't mix __try{}__except block with local objects
   // that have destructors:
   reset_stack_guard_page();
   std::runtime_error err("Out of stack space, while attempting to match a regular expression.");
   raise_runtime_error(err);
}

bool AUTOBOOST_REGEX_CALL abstract_protected_call::execute()const
{
   __try{
      return this->call();
   }__except(EXCEPTION_STACK_OVERFLOW == GetExceptionCode())
   {
      execute_eror();
   }
   // We never really get here at all:
   return false;
}

AUTOBOOST_REGEX_DECL void AUTOBOOST_REGEX_CALL reset_stack_guard_page()
{
#if defined(AUTOBOOST_REGEX_HAS_MS_STACK_GUARD) && defined(_MSC_VER) && (_MSC_VER >= 1300)
   _resetstkoflw();
#else
   //
   // We need to locate the current page being used by the stack,
   // move to the page below it and then deallocate and protect
   // that page.  Note that ideally we would protect only the lowest
   // stack page that has been allocated: in practice there
   // seems to be no easy way to locate this page, in any case as
   // long as the next page is protected, then Windows will figure
   // the rest out for us...
   //
   SYSTEM_INFO si;
   GetSystemInfo(&si);
   MEMORY_BASIC_INFORMATION mi;
   DWORD previous_protection_status;
   //
   // this is an address in our stack space:
   //
   LPBYTE page = (LPBYTE)&page;
   //
   // Get the current memory page in use:
   //
   VirtualQuery(page, &mi, sizeof(mi));
   //
   // Go to the page one below this:
   //
   page = (LPBYTE)(mi.BaseAddress)-si.dwPageSize;
   //
   // Free and protect everything from the start of the
   // allocation range, to the end of the page below the
   // one in use:
   //
   if (!VirtualFree(mi.AllocationBase, (LPBYTE)page - (LPBYTE)mi.AllocationBase, MEM_DECOMMIT)
      || !VirtualProtect(page, si.dwPageSize, PAGE_GUARD | PAGE_READWRITE, &previous_protection_status))
   {
      throw std::bad_exception();
   }
#endif
}
#endif

#if defined(AUTOBOOST_REGEX_NON_RECURSIVE) && !defined(AUTOBOOST_REGEX_V3)

#if AUTOBOOST_REGEX_MAX_CACHE_BLOCKS == 0

AUTOBOOST_REGEX_DECL void* AUTOBOOST_REGEX_CALL get_mem_block()
{
   return ::operator new(AUTOBOOST_REGEX_BLOCKSIZE);
}

AUTOBOOST_REGEX_DECL void AUTOBOOST_REGEX_CALL put_mem_block(void* p)
{
   ::operator delete(p);
}

#else

#ifdef AUTOBOOST_HAS_THREADS
mem_block_cache block_cache = { 0, 0, AUTOBOOST_STATIC_MUTEX_INIT, };
#else
mem_block_cache block_cache = { 0, 0, };
#endif

AUTOBOOST_REGEX_DECL void* AUTOBOOST_REGEX_CALL get_mem_block()
{
   return block_cache.get();
}

AUTOBOOST_REGEX_DECL void AUTOBOOST_REGEX_CALL put_mem_block(void* p)
{
   block_cache.put(p);
}

#endif

#endif

} // namespace re_detail



} // namespace autoboost

#if defined(AUTOBOOST_RE_USE_VCL) && defined(AUTOBOOST_REGEX_DYN_LINK)

int WINAPI DllEntryPoint(HINSTANCE , unsigned long , void*)
{
   return 1;
}
#endif


/*
 *
 * Copyright (c) 2004
 * John Maddock
 *
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         regex_raw_buffer.cpp
  *   VERSION      see <autoboost/version.hpp>
  *   DESCRIPTION: Member functions for class raw_storage.
  */


#define AUTOBOOST_REGEX_SOURCE
#include <autoboost/config.hpp>
#include <memory>
#include <cstring>
#include <autoboost/assert.hpp>
#include <autoboost/regex/v4/regex_raw_buffer.hpp>

#if defined(AUTOBOOST_NO_STDC_NAMESPACE)
namespace std{
   using ::memcpy;
   using ::memmove;
}
#endif


namespace autoboost{ namespace re_detail{

void AUTOBOOST_REGEX_CALL raw_storage::resize(size_type n)
{
   register size_type newsize = start ? last - start : 1024;
   while(newsize < n)
      newsize *= 2;
   register size_type datasize = end - start;
   // extend newsize to WORD/DWORD boundary:
   newsize = (newsize + padding_mask) & ~(padding_mask);

   // allocate and copy data:
   register pointer ptr = static_cast<pointer>(::operator new(newsize));
   AUTOBOOST_REGEX_NOEH_ASSERT(ptr)
   if(start)
      std::memcpy(ptr, start, datasize);

   // get rid of old buffer:
   ::operator delete(start);

   // and set up pointers:
   start = ptr;
   end = ptr + datasize;
   last = ptr + newsize;
}

void* AUTOBOOST_REGEX_CALL raw_storage::insert(size_type pos, size_type n)
{
   AUTOBOOST_ASSERT(pos <= size_type(end - start));
   if(size_type(last - end) < n)
      resize(n + (end - start));
   register void* result = start + pos;
   std::memmove(start + pos + n, start + pos, (end - start) - pos);
   end += n;
   return result;
}

}} // namespaces

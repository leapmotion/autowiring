//  filesystem path_traits.cpp  --------------------------------------------------------//

//  Copyright Beman Dawes 2008, 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

// define AUTOBOOST_FILESYSTEM_SOURCE so that <autoboost/system/config.hpp> knows
// the library is being built (possibly exporting rather than importing code)
#define AUTOBOOST_FILESYSTEM_SOURCE

#ifndef AUTOBOOST_SYSTEM_NO_DEPRECATED
# define AUTOBOOST_SYSTEM_NO_DEPRECATED
#endif

#include <autoboost/filesystem/config.hpp>
#include <autoboost/filesystem/path_traits.hpp>
#include <autoboost/system/system_error.hpp>
#include <autoboost/scoped_array.hpp>
#include <locale>   // for codecvt_base::result
#include <cstring>  // for strlen
#include <cwchar>   // for wcslen

namespace pt = autoboost::filesystem::path_traits;
namespace fs = autoboost::filesystem;
namespace bs = autoboost::system;

//--------------------------------------------------------------------------------------//
//                                  configuration                                       //
//--------------------------------------------------------------------------------------//

#ifndef AUTOBOOST_FILESYSTEM_CODECVT_BUF_SIZE
# define AUTOBOOST_FILESYSTEM_CODECVT_BUF_SIZE 256
#endif

namespace {

  const std::size_t default_codecvt_buf_size = AUTOBOOST_FILESYSTEM_CODECVT_BUF_SIZE;


//--------------------------------------------------------------------------------------//
//                                                                                      //
//  The public convert() functions do buffer management, and then forward to the        //
//  convert_aux() functions for the actual call to the codecvt facet.                   //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------//
//                      convert_aux const char* to wstring                             //
//--------------------------------------------------------------------------------------//

  void convert_aux(
                   const char* from,
                   const char* from_end,
                   wchar_t* to, wchar_t* to_end,
                   std::wstring & target,
                   const pt::codecvt_type & cvt)
  {
    //std::cout << std::hex
    //          << " from=" << std::size_t(from)
    //          << " from_end=" << std::size_t(from_end)
    //          << " to=" << std::size_t(to)
    //          << " to_end=" << std::size_t(to_end)
    //          << std::endl;

    std::mbstate_t state  = std::mbstate_t();  // perhaps unneeded, but cuts bug reports
    const char* from_next;
    wchar_t* to_next;

    std::codecvt_base::result res;

    if ((res=cvt.in(state, from, from_end, from_next,
           to, to_end, to_next)) != std::codecvt_base::ok)
    {
      //std::cout << " result is " << static_cast<int>(res) << std::endl;
      AUTOBOOST_FILESYSTEM_THROW(bs::system_error(res, fs::codecvt_error_category(),
        "autoboost::filesystem::path codecvt to wstring"));
    }
    target.append(to, to_next);
  }

//--------------------------------------------------------------------------------------//
//                      convert_aux const wchar_t* to string                           //
//--------------------------------------------------------------------------------------//

  void convert_aux(
                   const wchar_t* from,
                   const wchar_t* from_end,
                   char* to, char* to_end,
                   std::string & target,
                   const pt::codecvt_type & cvt)
  {
    //std::cout << std::hex
    //          << " from=" << std::size_t(from)
    //          << " from_end=" << std::size_t(from_end)
    //          << " to=" << std::size_t(to)
    //          << " to_end=" << std::size_t(to_end)
    //          << std::endl;

    std::mbstate_t state  = std::mbstate_t();  // perhaps unneeded, but cuts bug reports
    const wchar_t* from_next;
    char* to_next;

    std::codecvt_base::result res;

    if ((res=cvt.out(state, from, from_end, from_next,
           to, to_end, to_next)) != std::codecvt_base::ok)
    {
      //std::cout << " result is " << static_cast<int>(res) << std::endl;
      AUTOBOOST_FILESYSTEM_THROW(bs::system_error(res, fs::codecvt_error_category(),
        "autoboost::filesystem::path codecvt to string"));
    }
    target.append(to, to_next);
  }

}  // unnamed namespace

//--------------------------------------------------------------------------------------//
//                                   path_traits                                        //
//--------------------------------------------------------------------------------------//

namespace autoboost { namespace filesystem { namespace path_traits {

//--------------------------------------------------------------------------------------//
//                          convert const char* to wstring                              //
//--------------------------------------------------------------------------------------//

  AUTOBOOST_FILESYSTEM_DECL
  void convert(const char* from,
                const char* from_end,    // 0 for null terminated MBCS
                std::wstring & to,
                const codecvt_type & cvt)
  {
    AUTOBOOST_ASSERT(from);

    if (!from_end)  // null terminated
    {
      from_end = from + std::strlen(from);
    }

    if (from == from_end) return;

    std::size_t buf_size = (from_end - from) * 3;  // perhaps too large, but that's OK

    //  dynamically allocate a buffer only if source is unusually large
    if (buf_size > default_codecvt_buf_size)
    {
      autoboost::scoped_array< wchar_t > buf(new wchar_t [buf_size]);
      convert_aux(from, from_end, buf.get(), buf.get()+buf_size, to, cvt);
    }
    else
    {
      wchar_t buf[default_codecvt_buf_size];
      convert_aux(from, from_end, buf, buf+default_codecvt_buf_size, to, cvt);
    }
  }

//--------------------------------------------------------------------------------------//
//                         convert const wchar_t* to string                            //
//--------------------------------------------------------------------------------------//

  AUTOBOOST_FILESYSTEM_DECL
  void convert(const wchar_t* from,
                const wchar_t* from_end,  // 0 for null terminated MBCS
                std::string & to,
                const codecvt_type & cvt)
  {
    AUTOBOOST_ASSERT(from);

    if (!from_end)  // null terminated
    {
      from_end = from + std::wcslen(from);
    }

    if (from == from_end) return;

    //  The codecvt length functions may not be implemented, and I don't really
    //  understand them either. Thus this code is just a guess; if it turns
    //  out the buffer is too small then an error will be reported and the code
    //  will have to be fixed.
    std::size_t buf_size = (from_end - from) * 4;  // perhaps too large, but that's OK
    buf_size += 4;  // encodings like shift-JIS need some prefix space

    //  dynamically allocate a buffer only if source is unusually large
    if (buf_size > default_codecvt_buf_size)
    {
      autoboost::scoped_array< char > buf(new char [buf_size]);
      convert_aux(from, from_end, buf.get(), buf.get()+buf_size, to, cvt);
    }
    else
    {
      char buf[default_codecvt_buf_size];
      convert_aux(from, from_end, buf, buf+default_codecvt_buf_size, to, cvt);
    }
  }
}}} // namespace autoboost::filesystem::path_traits

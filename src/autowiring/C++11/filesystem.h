// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

#ifdef _MSC_VER
#include <filesystem>
namespace awfsnamespace = std::tr2::sys;
#else
// Experimental filesystem library is rare everywhere else, we have to resort to autoboost
#include <autoboost/filesystem/config.hpp>
#include <autoboost/filesystem/convenience.hpp>
#include <autoboost/filesystem/operations.hpp>
#include <autoboost/filesystem/path.hpp>
namespace awfsnamespace = autoboost::filesystem;
#endif

namespace std {
  namespace filesystem {
#ifdef _MSC_VER
#if _MSC_VER >= 1900
    using awfsnamespace::path;
#else
    class path :
      public awfsnamespace::wpath
    {
    public:
      path(void) = default;
      path(const string_type& _Str) :
        awfsnamespace::wpath(_Str)
      {}
      path(const awfsnamespace::wpath& _Right) :
        awfsnamespace::wpath(_Right)
      {}
      path(const path& _Right) :
        awfsnamespace::wpath(_Right)
      {}
      path(const char* _Ptr) :
        awfsnamespace::wpath(_Ptr, _Ptr + strlen(_Ptr))
      {}
      path(const wchar_t* _Ptr) :
        awfsnamespace::wpath(_Ptr)
      {}

      basic_path& operator=(basic_path&& _Right) { return *(awfsnamespace::wpath*)this = std::move(_Right); }
      basic_path& operator=(const string_type& _Str) { return *(awfsnamespace::wpath*)this = _Str; }
      basic_path& operator=(const wchar_t* _Ptr) { return *(awfsnamespace::wpath*)this = _Ptr; }

      path extension(void) const {
        return{ awfsnamespace::wpath::extension() };
      }
      path filename(void) const {
        return{ awfsnamespace::wpath::filename() };
      }
    };
#endif
#else
    using path = awfsnamespace::wpath;
#endif

    using awfsnamespace::create_directory;
    using awfsnamespace::current_path;
    using awfsnamespace::exists;
    using awfsnamespace::is_directory;
    using awfsnamespace::is_empty;
    using awfsnamespace::remove;
    using awfsnamespace::remove_all;
    using awfsnamespace::rename;

#ifdef _MSC_VER
#if _MSC_VER >= 1900
    using std::experimental::filesystem::canonical;
#else
    template<class _Path>
    _Path canonical(const _Path& _Pval, const _Path& _Pbase = current_path<_Path>())
    {
      return awfsnamespace::complete(_Pval, _Pbase);
    }
#endif
#else
    using awfsnamespace::canonical;
#endif
  }
}

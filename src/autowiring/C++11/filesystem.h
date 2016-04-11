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
    using awfsnamespace::path;
    using awfsnamespace::wpath;
    using awfsnamespace::basename;
    using awfsnamespace::create_directory;
    using awfsnamespace::current_path;
    using awfsnamespace::exists;
    using awfsnamespace::initial_path;
    using awfsnamespace::is_directory;
    using awfsnamespace::is_empty;
    using awfsnamespace::remove;
    using awfsnamespace::remove_all;
    using awfsnamespace::rename;

#ifdef _MSC_VER
    template<class _Path>
    _Path canonical(const _Path& _Pval, const _Path& _Pbase = current_path<_Path>())
    {
      return awfsnamespace::complete(_Pval, _Pbase);
    }
#else
    using awfsnamespace::canonical;
#endif
  }
}

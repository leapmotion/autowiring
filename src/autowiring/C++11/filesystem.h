// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

#if _MSC_VER >= 1900
#include <filesystem>
namespace awfsnamespace = std::tr2::sys;
#else
// Experimental filesystem library is rare everywhere else, we have to resort to autoboost
#define AUTOBOOST_FILESYSTEM_NO_DEPRECATED 1
#include <autoboost/filesystem/config.hpp>
#include <autoboost/filesystem/convenience.hpp>
#include <autoboost/filesystem/operations.hpp>
#include <autoboost/filesystem/path.hpp>
namespace awfsnamespace = autoboost::filesystem;
#endif

namespace std {
  namespace filesystem {
    using awfsnamespace::path;
    using awfsnamespace::create_directory;
    using awfsnamespace::current_path;
    using awfsnamespace::exists;
    using awfsnamespace::is_directory;
    using awfsnamespace::is_empty;
    using awfsnamespace::remove;
    using awfsnamespace::remove_all;
    using awfsnamespace::rename;
    using awfsnamespace::canonical;
  }
}

// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once

//C++17 Filesystem standard
#if defined(_MSC_VER) && _MSC_VER >= 1900
  #include <filesystem>
  #if _MSC_VER >= 1914 && _HAS_CXX17
    // VS 2017 15.7 Preview 2 or newer with /std:c++17
    namespace awfsnamespace = std::filesystem;
  #elif _MSC_VER >= 1910
    namespace awfsnamespace = std::experimental::filesystem;
  #else
    namespace awfsnamespace = std::tr2::sys;
  #endif
#else
// Experimental filesystem TS library is rare everywhere else, we have to resort to autoboost
#include <autoboost/filesystem.hpp>
namespace awfsnamespace = autoboost::filesystem;
#endif

#if !defined(_MSC_VER) || _MSC_VER < 1914 || !_HAS_CXX17
namespace std {
  namespace filesystem {
    using awfsnamespace::path;
    using awfsnamespace::canonical;
    using awfsnamespace::create_directory;
    using awfsnamespace::current_path;
    using awfsnamespace::directory_iterator;
    using awfsnamespace::exists;
    using awfsnamespace::filesystem_error;
    using awfsnamespace::is_directory;
    using awfsnamespace::is_empty;
    using awfsnamespace::remove;
    using awfsnamespace::remove_all;
    using awfsnamespace::rename;
    using awfsnamespace::file_size;
    using awfsnamespace::temp_directory_path;
  }
}
#endif

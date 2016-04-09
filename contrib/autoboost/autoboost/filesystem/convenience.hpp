//  autoboost/filesystem/convenience.hpp  ----------------------------------------//

//  Copyright Beman Dawes, 2002-2005
//  Copyright Vladimir Prus, 2002
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

//----------------------------------------------------------------------------// 

#ifndef AUTOBOOST_FILESYSTEM3_CONVENIENCE_HPP
#define AUTOBOOST_FILESYSTEM3_CONVENIENCE_HPP

#include <autoboost/config.hpp>

# if defined( AUTOBOOST_NO_STD_WSTRING )
#   error Configuration not supported: Boost.Filesystem V3 and later requires std::wstring support
# endif

#include <autoboost/filesystem/operations.hpp>
#include <autoboost/system/error_code.hpp>

#include <autoboost/config/abi_prefix.hpp> // must be the last #include

namespace autoboost
{
  namespace filesystem
  {

# ifndef AUTOBOOST_FILESYSTEM_NO_DEPRECATED

    inline std::string extension(const path & p)
    {
      return p.extension().string();
    }

    inline std::string basename(const path & p)
    {
      return p.stem().string();
    }

    inline path change_extension( const path & p, const path & new_extension )
    { 
      path new_p( p );
      new_p.replace_extension( new_extension );
      return new_p;
    }

# endif


  } // namespace filesystem
} // namespace autoboost

#include <autoboost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas
#endif // AUTOBOOST_FILESYSTEM3_CONVENIENCE_HPP

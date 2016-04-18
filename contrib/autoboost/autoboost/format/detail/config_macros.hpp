// -*- C++ -*-
// ----------------------------------------------------------------------------
// config_macros.hpp : configuration macros for the format library
// only AUTOBOOST_IO_STD is absolutely needed (it should be 'std::' in general)
// others are compiler-specific workaround macros used in #ifdef switches
// ----------------------------------------------------------------------------

//  Copyright Samuel Krempp 2003. Use, modification, and distribution are
//  subject to the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// see http://www.boost.org/libs/format for library home page


// ----------------------------------------------------------------------------

#ifndef AUTOBOOST_FORMAT_CONFIG_MACROS_HPP
#define AUTOBOOST_FORMAT_CONFIG_MACROS_HPP

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

// make sure our local macros wont override something :
#if defined(AUTOBOOST_NO_LOCALE_ISDIGIT) || defined(AUTOBOOST_OVERLOAD_FOR_NON_CONST) \
  || defined(AUTOBOOST_IO_STD) || defined( AUTOBOOST_IO_NEEDS_USING_DECLARATION ) \
    || defined(AUTOBOOST_NO_TEMPLATE_STD_STREAM) \
    || defined(AUTOBOOST_FORMAT_STREAMBUF_DEFINED) || defined(AUTOBOOST_FORMAT_OSTREAM_DEFINED)
#error "autoboost::format uses a local macro that is already defined."
#endif

// specific workarounds. each header can define BOOS_IO_STD if it
// needs. (e.g. because of IO_NEEDS_USING_DECLARATION)
#include <autoboost/format/detail/workarounds_gcc-2_95.hpp>
#include <autoboost/format/detail/workarounds_stlport.hpp>

#ifndef AUTOBOOST_IO_STD
#  define AUTOBOOST_IO_STD ::std::
#endif

#if defined(AUTOBOOST_NO_STD_LOCALE) || \
 ( AUTOBOOST_WORKAROUND(__BORLANDC__, <= 0x564) \
   || AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT( 0x570 ) )  )
// some future __BORLANDC__ >0x564  versions might not need this
// 0x570 is Borland's kylix branch
#define AUTOBOOST_NO_LOCALE_ISDIGIT
#endif

#if  AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x570) ) || AUTOBOOST_WORKAROUND( AUTOBOOST_MSVC, AUTOBOOST_TESTED_AT(1300))
#define AUTOBOOST_NO_OVERLOAD_FOR_NON_CONST
#endif

// **** Workaround for io streams, stlport and msvc.
#ifdef AUTOBOOST_IO_NEEDS_USING_DECLARATION
namespace autoboost {
  using std::char_traits;
  using std::basic_ostream;
  namespace io {
    using std::basic_ostream;
    namespace detail {
      using std::basic_ios;
      using std::basic_ostream;
    }
  }
#if ! defined(AUTOBOOST_NO_STD_LOCALE)
    using std::locale;
    namespace io {
        using std::locale;
        namespace detail {
            using std::locale;
        }
    }
#endif // locale
}
  // -end N.S. autoboost
#endif // needs_using_declaration

#if ! defined(AUTOBOOST_NO_STD_LOCALE)
#include <locale>
#endif


// ***  hide std::locale if it doesnt exist.
// this typedef is either std::locale or int, avoids placing ifdefs everywhere
namespace autoboost { namespace io { namespace detail {
#if ! defined(AUTOBOOST_NO_STD_LOCALE)
    typedef AUTOBOOST_IO_STD locale locale_t;
#else
    typedef int          locale_t;
#endif
} } }


// ----------------------------------------------------------------------------

#endif // AUTOBOOST_FORMAT_MACROS_DEFAULT_HPP

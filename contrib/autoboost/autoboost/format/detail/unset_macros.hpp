// ----------------------------------------------------------------------------
// unset_macros.hpp
// ----------------------------------------------------------------------------

//  Copyright Samuel Krempp 2003. Use, modification, and distribution are
//  subject to the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/format for library home page

// ----------------------------------------------------------------------------

// *** Undefine 'local' macros :
#ifdef AUTOBOOST_NO_OVERLOAD_FOR_NON_CONST
#undef AUTOBOOST_NO_OVERLOAD_FOR_NON_CONST
#endif
#ifdef AUTOBOOST_NO_LOCALE_ISDIGIT
#undef AUTOBOOST_NO_LOCALE_ISDIGIT
#endif
#ifdef AUTOBOOST_IO_STD
#undef AUTOBOOST_IO_STD
#endif
#ifdef AUTOBOOST_IO_NEEDS_USING_DECLARATION
#undef AUTOBOOST_IO_NEEDS_USING_DECLARATION
#endif
#ifdef AUTOBOOST_NO_TEMPLATE_STD_STREAM
#undef AUTOBOOST_NO_TEMPLATE_STD_STREAM
#endif
#ifdef AUTOBOOST_FORMAT_STREAMBUF_DEFINED
#undef AUTOBOOST_FORMAT_STREAMBUF_DEFINED
#endif
#ifdef AUTOBOOST_FORMAT_OSTREAM_DEFINED
#undef AUTOBOOST_FORMAT_OSTREAM_DEFINED
#endif

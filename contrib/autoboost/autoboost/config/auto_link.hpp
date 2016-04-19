//  (C) Copyright John Maddock 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         auto_link.hpp
  *   VERSION      see <autoboost/version.hpp>
  *   DESCRIPTION: Automatic library inclusion for Borland/Microsoft compilers.
  */

/*************************************************************************

USAGE:
~~~~~~

Before including this header you must define one or more of define the following macros:

AUTOBOOST_LIB_NAME:           Required: A string containing the basename of the library,
                          for example autoboost_regex.
AUTOBOOST_LIB_TOOLSET:        Optional: the base name of the toolset.
AUTOBOOST_DYN_LINK:           Optional: when set link to dll rather than static library.
AUTOBOOST_LIB_DIAGNOSTIC:     Optional: when set the header will print out the name
                          of the library selected (useful for debugging).
AUTOBOOST_AUTO_LINK_NOMANGLE: Specifies that we should link to AUTOBOOST_LIB_NAME.lib,
                          rather than a mangled-name version.
AUTOBOOST_AUTO_LINK_TAGGED:   Specifies that we link to libraries built with the --layout=tagged option.
                          This is essentially the same as the default name-mangled version, but without
                          the compiler name and version, or the Boost version.  Just the build options.

These macros will be undef'ed at the end of the header, further this header
has no include guards - so be sure to include it only once from your library!

Algorithm:
~~~~~~~~~~

Libraries for Borland and Microsoft compilers are automatically
selected here, the name of the lib is selected according to the following
formula:

AUTOBOOST_LIB_PREFIX
   + AUTOBOOST_LIB_NAME
   + "_"
   + AUTOBOOST_LIB_TOOLSET
   + AUTOBOOST_LIB_THREAD_OPT
   + AUTOBOOST_LIB_RT_OPT
   "-"
   + AUTOBOOST_LIB_VERSION

These are defined as:

AUTOBOOST_LIB_PREFIX:     "lib" for static libraries otherwise "".

AUTOBOOST_LIB_NAME:       The base name of the lib ( for example autoboost_regex).

AUTOBOOST_LIB_TOOLSET:    The compiler toolset name (vc6, vc7, bcb5 etc).

AUTOBOOST_LIB_THREAD_OPT: "-mt" for multithread builds, otherwise nothing.

AUTOBOOST_LIB_RT_OPT:     A suffix that indicates the runtime library used,
                      contains one or more of the following letters after
                      a hyphen:

                      s      static runtime (dynamic if not present).
                      g      debug/diagnostic runtime (release if not present).
                      y      Python debug/diagnostic runtime (release if not present).
                      d      debug build (release if not present).
                      p      STLport build.
                      n      STLport build without its IOStreams.

AUTOBOOST_LIB_VERSION:    The Boost version, in the form x_y, for Boost version x.y.


***************************************************************************/

#ifdef __cplusplus
#  ifndef AUTOBOOST_CONFIG_HPP
#     include <autoboost/config.hpp>
#  endif
#elif defined(_MSC_VER) && !defined(__MWERKS__) && !defined(__EDG_VERSION__)
//
// C language compatability (no, honestly)
//
#  define AUTOBOOST_MSVC _MSC_VER
#  define AUTOBOOST_STRINGIZE(X) AUTOBOOST_DO_STRINGIZE(X)
#  define AUTOBOOST_DO_STRINGIZE(X) #X
#endif
//
// Only include what follows for known and supported compilers:
//
#if defined(AUTOBOOST_MSVC) \
    || defined(__BORLANDC__) \
    || (defined(__MWERKS__) && defined(_WIN32) && (__MWERKS__ >= 0x3000)) \
    || (defined(__ICL) && defined(_MSC_EXTENSIONS) && (_MSC_VER >= 1200))

#ifndef AUTOBOOST_VERSION_HPP
#  include <autoboost/version.hpp>
#endif

#ifndef AUTOBOOST_LIB_NAME
#  error "Macro AUTOBOOST_LIB_NAME not set (internal error)"
#endif

//
// error check:
//
#if defined(__MSVC_RUNTIME_CHECKS) && !defined(_DEBUG)
#  pragma message("Using the /RTC option without specifying a debug runtime will lead to linker errors")
#  pragma message("Hint: go to the code generation options and switch to one of the debugging runtimes")
#  error "Incompatible build options"
#endif
//
// select toolset if not defined already:
//
#ifndef AUTOBOOST_LIB_TOOLSET
#  if defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC < 1200)
    // Note: no compilers before 1200 are supported
#  elif defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC < 1300)

#    ifdef UNDER_CE
       // eVC4:
#      define AUTOBOOST_LIB_TOOLSET "evc4"
#    else
       // vc6:
#      define AUTOBOOST_LIB_TOOLSET "vc6"
#    endif

#  elif defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC < 1310)

     // vc7:
#    define AUTOBOOST_LIB_TOOLSET "vc7"

#  elif defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC < 1400)

     // vc71:
#    define AUTOBOOST_LIB_TOOLSET "vc71"

#  elif defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC < 1500)

     // vc80:
#    define AUTOBOOST_LIB_TOOLSET "vc80"

#  elif defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC < 1600)

     // vc90:
#    define AUTOBOOST_LIB_TOOLSET "vc90"

#  elif defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC < 1700)

     // vc10:
#    define AUTOBOOST_LIB_TOOLSET "vc100"

#  elif defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC < 1800)

     // vc11:
#    define AUTOBOOST_LIB_TOOLSET "vc110"

#  elif defined(AUTOBOOST_MSVC) && (AUTOBOOST_MSVC < 1900)

     // vc12:
#    define AUTOBOOST_LIB_TOOLSET "vc120"

# elif defined(AUTOBOOST_MSVC)

   // vc14:
#  define AUTOBOOST_LIB_TOOLSET "vc140"

#  elif defined(__BORLANDC__)

     // CBuilder 6:
#    define AUTOBOOST_LIB_TOOLSET "bcb"

#  elif defined(__ICL)

     // Intel C++, no version number:
#    define AUTOBOOST_LIB_TOOLSET "iw"

#  elif defined(__MWERKS__) && (__MWERKS__ <= 0x31FF )

     // Metrowerks CodeWarrior 8.x
#    define AUTOBOOST_LIB_TOOLSET "cw8"

#  elif defined(__MWERKS__) && (__MWERKS__ <= 0x32FF )

     // Metrowerks CodeWarrior 9.x
#    define AUTOBOOST_LIB_TOOLSET "cw9"

#  endif
#endif // AUTOBOOST_LIB_TOOLSET

//
// select thread opt:
//
#if defined(_MT) || defined(__MT__)
#  define AUTOBOOST_LIB_THREAD_OPT "-mt"
#else
#  define AUTOBOOST_LIB_THREAD_OPT
#endif

#if defined(_MSC_VER) || defined(__MWERKS__)

#  ifdef _DLL

#     if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && (defined(_STLP_OWN_IOSTREAMS) || defined(__STL_OWN_IOSTREAMS))

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#            define AUTOBOOST_LIB_RT_OPT "-gydp"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define AUTOBOOST_LIB_RT_OPT "-gdp"
#        elif defined(_DEBUG)\
               && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#            define AUTOBOOST_LIB_RT_OPT "-gydp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#            define AUTOBOOST_LIB_RT_OPT "-gdp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define AUTOBOOST_LIB_RT_OPT "-p"
#        endif

#     elif defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#            define AUTOBOOST_LIB_RT_OPT "-gydpn"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define AUTOBOOST_LIB_RT_OPT "-gdpn"
#        elif defined(_DEBUG)\
               && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#            define AUTOBOOST_LIB_RT_OPT "-gydpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#            define AUTOBOOST_LIB_RT_OPT "-gdpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define AUTOBOOST_LIB_RT_OPT "-pn"
#        endif

#     else

#        if defined(_DEBUG) && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#            define AUTOBOOST_LIB_RT_OPT "-gyd"
#        elif defined(_DEBUG)
#            define AUTOBOOST_LIB_RT_OPT "-gd"
#        else
#            define AUTOBOOST_LIB_RT_OPT
#        endif

#     endif

#  else

#     if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && (defined(_STLP_OWN_IOSTREAMS) || defined(__STL_OWN_IOSTREAMS))

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#            define AUTOBOOST_LIB_RT_OPT "-sgydp"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define AUTOBOOST_LIB_RT_OPT "-sgdp"
#        elif defined(_DEBUG)\
               && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#             define AUTOBOOST_LIB_RT_OPT "-sgydp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#             define AUTOBOOST_LIB_RT_OPT "-sgdp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define AUTOBOOST_LIB_RT_OPT "-sp"
#        endif

#     elif defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#            define AUTOBOOST_LIB_RT_OPT "-sgydpn"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define AUTOBOOST_LIB_RT_OPT "-sgdpn"
#        elif defined(_DEBUG)\
               && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#             define AUTOBOOST_LIB_RT_OPT "-sgydpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#             define AUTOBOOST_LIB_RT_OPT "-sgdpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define AUTOBOOST_LIB_RT_OPT "-spn"
#        endif

#     else

#        if defined(_DEBUG)\
               && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#             define AUTOBOOST_LIB_RT_OPT "-sgyd"
#        elif defined(_DEBUG)
#             define AUTOBOOST_LIB_RT_OPT "-sgd"
#        else
#            define AUTOBOOST_LIB_RT_OPT "-s"
#        endif

#     endif

#  endif

#elif defined(__BORLANDC__)

//
// figure out whether we want the debug builds or not:
//
#if __BORLANDC__ > 0x561
#pragma defineonoption AUTOBOOST_BORLAND_DEBUG -v
#endif
//
// sanity check:
//
#if defined(__STL_DEBUG) || defined(_STLP_DEBUG)
#error "Pre-built versions of the Boost libraries are not provided in STLport-debug form"
#endif

#  ifdef _RTLDLL

#     if defined(AUTOBOOST_BORLAND_DEBUG)\
               && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#         define AUTOBOOST_LIB_RT_OPT "-yd"
#     elif defined(AUTOBOOST_BORLAND_DEBUG)
#         define AUTOBOOST_LIB_RT_OPT "-d"
#     elif defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#         define AUTOBOOST_LIB_RT_OPT -y
#     else
#         define AUTOBOOST_LIB_RT_OPT
#     endif

#  else

#     if defined(AUTOBOOST_BORLAND_DEBUG)\
               && defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#         define AUTOBOOST_LIB_RT_OPT "-syd"
#     elif defined(AUTOBOOST_BORLAND_DEBUG)
#         define AUTOBOOST_LIB_RT_OPT "-sd"
#     elif defined(AUTOBOOST_DEBUG_PYTHON) && defined(AUTOBOOST_LINKING_PYTHON)
#         define AUTOBOOST_LIB_RT_OPT "-sy"
#     else
#         define AUTOBOOST_LIB_RT_OPT "-s"
#     endif

#  endif

#endif

//
// select linkage opt:
//
#if (defined(_DLL) || defined(_RTLDLL)) && defined(AUTOBOOST_DYN_LINK)
#  define AUTOBOOST_LIB_PREFIX
#elif defined(AUTOBOOST_DYN_LINK)
#  error "Mixing a dll autoboost library with a static runtime is a really bad idea..."
#else
#  define AUTOBOOST_LIB_PREFIX "lib"
#endif

//
// now include the lib:
//
#if defined(AUTOBOOST_LIB_NAME) \
      && defined(AUTOBOOST_LIB_PREFIX) \
      && defined(AUTOBOOST_LIB_TOOLSET) \
      && defined(AUTOBOOST_LIB_THREAD_OPT) \
      && defined(AUTOBOOST_LIB_RT_OPT) \
      && defined(AUTOBOOST_LIB_VERSION)

#ifdef AUTOBOOST_AUTO_LINK_TAGGED
#  pragma comment(lib, AUTOBOOST_LIB_PREFIX AUTOBOOST_STRINGIZE(AUTOBOOST_LIB_NAME) AUTOBOOST_LIB_THREAD_OPT AUTOBOOST_LIB_RT_OPT ".lib")
#  ifdef AUTOBOOST_LIB_DIAGNOSTIC
#     pragma message ("Linking to lib file: " AUTOBOOST_LIB_PREFIX AUTOBOOST_STRINGIZE(AUTOBOOST_LIB_NAME) AUTOBOOST_LIB_THREAD_OPT AUTOBOOST_LIB_RT_OPT ".lib")
#  endif
#elif defined(AUTOBOOST_AUTO_LINK_NOMANGLE)
#  pragma comment(lib, AUTOBOOST_STRINGIZE(AUTOBOOST_LIB_NAME) ".lib")
#  ifdef AUTOBOOST_LIB_DIAGNOSTIC
#     pragma message ("Linking to lib file: " AUTOBOOST_STRINGIZE(AUTOBOOST_LIB_NAME) ".lib")
#  endif
#elif defined(AUTOBOOST_LIB_BUILDID)
#  pragma comment(lib, AUTOBOOST_LIB_PREFIX AUTOBOOST_STRINGIZE(AUTOBOOST_LIB_NAME) "-" AUTOBOOST_LIB_TOOLSET AUTOBOOST_LIB_THREAD_OPT AUTOBOOST_LIB_RT_OPT "-" AUTOBOOST_LIB_VERSION "-" AUTOBOOST_STRINGIZE(AUTOBOOST_LIB_BUILDID) ".lib")
#  ifdef AUTOBOOST_LIB_DIAGNOSTIC
#     pragma message ("Linking to lib file: " AUTOBOOST_LIB_PREFIX AUTOBOOST_STRINGIZE(AUTOBOOST_LIB_NAME) "-" AUTOBOOST_LIB_TOOLSET AUTOBOOST_LIB_THREAD_OPT AUTOBOOST_LIB_RT_OPT "-" AUTOBOOST_LIB_VERSION "-" AUTOBOOST_STRINGIZE(AUTOBOOST_LIB_BUILDID) ".lib")
#  endif
#else
#  pragma comment(lib, AUTOBOOST_LIB_PREFIX AUTOBOOST_STRINGIZE(AUTOBOOST_LIB_NAME) "-" AUTOBOOST_LIB_TOOLSET AUTOBOOST_LIB_THREAD_OPT AUTOBOOST_LIB_RT_OPT "-" AUTOBOOST_LIB_VERSION ".lib")
#  ifdef AUTOBOOST_LIB_DIAGNOSTIC
#     pragma message ("Linking to lib file: " AUTOBOOST_LIB_PREFIX AUTOBOOST_STRINGIZE(AUTOBOOST_LIB_NAME) "-" AUTOBOOST_LIB_TOOLSET AUTOBOOST_LIB_THREAD_OPT AUTOBOOST_LIB_RT_OPT "-" AUTOBOOST_LIB_VERSION ".lib")
#  endif
#endif

#else
#  error "some required macros where not defined (internal logic error)."
#endif


#endif // _MSC_VER || __BORLANDC__

//
// finally undef any macros we may have set:
//
#ifdef AUTOBOOST_LIB_PREFIX
#  undef AUTOBOOST_LIB_PREFIX
#endif
#if defined(AUTOBOOST_LIB_NAME)
#  undef AUTOBOOST_LIB_NAME
#endif
// Don't undef this one: it can be set by the user and should be the
// same for all libraries:
//#if defined(AUTOBOOST_LIB_TOOLSET)
//#  undef AUTOBOOST_LIB_TOOLSET
//#endif
#if defined(AUTOBOOST_LIB_THREAD_OPT)
#  undef AUTOBOOST_LIB_THREAD_OPT
#endif
#if defined(AUTOBOOST_LIB_RT_OPT)
#  undef AUTOBOOST_LIB_RT_OPT
#endif
#if defined(AUTOBOOST_LIB_LINK_OPT)
#  undef AUTOBOOST_LIB_LINK_OPT
#endif
#if defined(AUTOBOOST_LIB_DEBUG_OPT)
#  undef AUTOBOOST_LIB_DEBUG_OPT
#endif
#if defined(AUTOBOOST_DYN_LINK)
#  undef AUTOBOOST_DYN_LINK
#endif



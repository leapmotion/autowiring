//  (C) Copyright John Maddock 2001-8.
//  (C) Copyright Peter Dimov 2001.
//  (C) Copyright Jens Maurer 2001.
//  (C) Copyright David Abrahams 2002 - 2003.
//  (C) Copyright Aleksey Gurtovoy 2002 - 2003.
//  (C) Copyright Guillaume Melquiond 2002 - 2003.
//  (C) Copyright Beman Dawes 2003.
//  (C) Copyright Martin Wille 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Intel compiler setup:

#if defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 1500) && (defined(_MSC_VER) || defined(__GNUC__))

#ifdef _MSC_VER

#include <autoboost/config/compiler/visualc.hpp>

#undef AUTOBOOST_MSVC
#undef AUTOBOOST_MSVC_FULL_VER

#if (__INTEL_COMPILER >= 1500) && (_MSC_VER >= 1900)
//
// These appear to be supported, even though VC++ may not support them:
//
#define AUTOBOOST_HAS_EXPM1
#define AUTOBOOST_HAS_LOG1P
#undef AUTOBOOST_NO_CXX14_BINARY_LITERALS
// This one may be a little risky to enable??
#undef AUTOBOOST_NO_SFINAE_EXPR

#endif

#else

#include <autoboost/config/compiler/gcc.hpp>

#undef AUTOBOOST_GCC_VERSION
#undef AUTOBOOST_GCC_CXX11

#endif

#undef AUTOBOOST_COMPILER

#if defined(__INTEL_COMPILER)
#if __INTEL_COMPILER == 9999
#  define AUTOBOOST_INTEL_CXX_VERSION 1200 // Intel bug in 12.1.
#else
#  define AUTOBOOST_INTEL_CXX_VERSION __INTEL_COMPILER
#endif
#elif defined(__ICL)
#  define AUTOBOOST_INTEL_CXX_VERSION __ICL
#elif defined(__ICC)
#  define AUTOBOOST_INTEL_CXX_VERSION __ICC
#elif defined(__ECC)
#  define AUTOBOOST_INTEL_CXX_VERSION __ECC
#endif

// Flags determined by comparing output of 'icpc -dM -E' with and without '-std=c++0x'
#if (!(defined(_WIN32) || defined(_WIN64)) && defined(__STDC_HOSTED__) && (__STDC_HOSTED__ && (AUTOBOOST_INTEL_CXX_VERSION <= 1200))) || defined(__GXX_EXPERIMENTAL_CPP0X__) || defined(__GXX_EXPERIMENTAL_CXX0X__)
#  define AUTOBOOST_INTEL_STDCXX0X
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#  define AUTOBOOST_INTEL_STDCXX0X
#endif

#ifdef __GNUC__
#  define AUTOBOOST_INTEL_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

#if !defined(AUTOBOOST_COMPILER)
#  if defined(AUTOBOOST_INTEL_STDCXX0X)
#    define AUTOBOOST_COMPILER "Intel C++ C++0x mode version " AUTOBOOST_STRINGIZE(AUTOBOOST_INTEL_CXX_VERSION)
#  else
#    define AUTOBOOST_COMPILER "Intel C++ version " AUTOBOOST_STRINGIZE(AUTOBOOST_INTEL_CXX_VERSION)
#  endif
#endif

#define AUTOBOOST_INTEL AUTOBOOST_INTEL_CXX_VERSION

#if defined(_WIN32) || defined(_WIN64)
#  define AUTOBOOST_INTEL_WIN AUTOBOOST_INTEL
#else
#  define AUTOBOOST_INTEL_LINUX AUTOBOOST_INTEL
#endif

#else

#include "autoboost/config/compiler/common_edg.hpp"

#if defined(__INTEL_COMPILER)
#if __INTEL_COMPILER == 9999
#  define AUTOBOOST_INTEL_CXX_VERSION 1200 // Intel bug in 12.1.
#else
#  define AUTOBOOST_INTEL_CXX_VERSION __INTEL_COMPILER
#endif
#elif defined(__ICL)
#  define AUTOBOOST_INTEL_CXX_VERSION __ICL
#elif defined(__ICC)
#  define AUTOBOOST_INTEL_CXX_VERSION __ICC
#elif defined(__ECC)
#  define AUTOBOOST_INTEL_CXX_VERSION __ECC
#endif

// Flags determined by comparing output of 'icpc -dM -E' with and without '-std=c++0x'
#if (!(defined(_WIN32) || defined(_WIN64)) && defined(__STDC_HOSTED__) && (__STDC_HOSTED__ && (AUTOBOOST_INTEL_CXX_VERSION <= 1200))) || defined(__GXX_EXPERIMENTAL_CPP0X__) || defined(__GXX_EXPERIMENTAL_CXX0X__)
#  define AUTOBOOST_INTEL_STDCXX0X
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#  define AUTOBOOST_INTEL_STDCXX0X
#endif

#ifdef __GNUC__
#  define AUTOBOOST_INTEL_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

#if !defined(AUTOBOOST_COMPILER)
#  if defined(AUTOBOOST_INTEL_STDCXX0X)
#    define AUTOBOOST_COMPILER "Intel C++ C++0x mode version " AUTOBOOST_STRINGIZE(AUTOBOOST_INTEL_CXX_VERSION)
#  else
#    define AUTOBOOST_COMPILER "Intel C++ version " AUTOBOOST_STRINGIZE(AUTOBOOST_INTEL_CXX_VERSION)
#  endif
#endif

#define AUTOBOOST_INTEL AUTOBOOST_INTEL_CXX_VERSION

#if defined(_WIN32) || defined(_WIN64)
#  define AUTOBOOST_INTEL_WIN AUTOBOOST_INTEL
#else
#  define AUTOBOOST_INTEL_LINUX AUTOBOOST_INTEL
#endif

#if (AUTOBOOST_INTEL_CXX_VERSION <= 600)

#  if defined(_MSC_VER) && (_MSC_VER <= 1300) // added check for <= VC 7 (Peter Dimov)

// Boost libraries assume strong standard conformance unless otherwise
// indicated by a config macro. As configured by Intel, the EDG front-end
// requires certain compiler options be set to achieve that strong conformance.
// Particularly /Qoption,c,--arg_dep_lookup (reported by Kirk Klobe & Thomas Witt)
// and /Zc:wchar_t,forScope. See autoboost-root/tools/build/intel-win32-tools.jam for
// details as they apply to particular versions of the compiler. When the
// compiler does not predefine a macro indicating if an option has been set,
// this config file simply assumes the option has been set.
// Thus AUTOBOOST_NO_ARGUMENT_DEPENDENT_LOOKUP will not be defined, even if
// the compiler option is not enabled.

#     define AUTOBOOST_NO_SWPRINTF
#  endif

// Void returns, 64 bit integrals don't work when emulating VC 6 (Peter Dimov)

#  if defined(_MSC_VER) && (_MSC_VER <= 1200)
#     define AUTOBOOST_NO_VOID_RETURNS
#     define AUTOBOOST_NO_INTEGRAL_INT64_T
#  endif

#endif

#if (AUTOBOOST_INTEL_CXX_VERSION <= 710) && defined(_WIN32)
#  define AUTOBOOST_NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
#endif

// See http://aspn.activestate.com/ASPN/Mail/Message/autoboost/1614864
#if AUTOBOOST_INTEL_CXX_VERSION < 600
#  define AUTOBOOST_NO_INTRINSIC_WCHAR_T
#else
// We should test the macro _WCHAR_T_DEFINED to check if the compiler
// supports wchar_t natively. *BUT* there is a problem here: the standard
// headers define this macro if they typedef wchar_t. Anyway, we're lucky
// because they define it without a value, while Intel C++ defines it
// to 1. So we can check its value to see if the macro was defined natively
// or not.
// Under UNIX, the situation is exactly the same, but the macro _WCHAR_T
// is used instead.
#  if ((_WCHAR_T_DEFINED + 0) == 0) && ((_WCHAR_T + 0) == 0)
#    define AUTOBOOST_NO_INTRINSIC_WCHAR_T
#  endif
#endif

#if defined(__GNUC__) && !defined(AUTOBOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL)
//
// Figure out when Intel is emulating this gcc bug
// (All Intel versions prior to 9.0.26, and versions
// later than that if they are set up to emulate gcc 3.2
// or earlier):
//
#  if ((__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)) || (AUTOBOOST_INTEL < 900) || (__INTEL_COMPILER_BUILD_DATE < 20050912)
#     define AUTOBOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL
#  endif
#endif
#if (defined(__GNUC__) && (__GNUC__ < 4)) || (defined(_WIN32) && (AUTOBOOST_INTEL_CXX_VERSION <= 1200)) || (AUTOBOOST_INTEL_CXX_VERSION <= 1200)
// GCC or VC emulation:
#define AUTOBOOST_NO_TWO_PHASE_NAME_LOOKUP
#endif
//
// Verify that we have actually got AUTOBOOST_NO_INTRINSIC_WCHAR_T
// set correctly, if we don't do this now, we will get errors later
// in type_traits code among other things, getting this correct
// for the Intel compiler is actually remarkably fragile and tricky:
//
#ifdef __cplusplus
#if defined(AUTOBOOST_NO_INTRINSIC_WCHAR_T)
#include <cwchar>
template< typename T > struct assert_no_intrinsic_wchar_t;
template<> struct assert_no_intrinsic_wchar_t<wchar_t> { typedef void type; };
// if you see an error here then you need to unset AUTOBOOST_NO_INTRINSIC_WCHAR_T
// where it is defined above:
typedef assert_no_intrinsic_wchar_t<unsigned short>::type assert_no_intrinsic_wchar_t_;
#else
template< typename T > struct assert_intrinsic_wchar_t;
template<> struct assert_intrinsic_wchar_t<wchar_t> {};
// if you see an error here then define AUTOBOOST_NO_INTRINSIC_WCHAR_T on the command line:
template<> struct assert_intrinsic_wchar_t<unsigned short> {};
#endif
#endif

#if defined(_MSC_VER) && (_MSC_VER+0 >= 1000)
#  if _MSC_VER >= 1200
#     define AUTOBOOST_HAS_MS_INT64
#  endif
#  define AUTOBOOST_NO_SWPRINTF
#  define AUTOBOOST_NO_TWO_PHASE_NAME_LOOKUP
#elif defined(_WIN32)
#  define AUTOBOOST_DISABLE_WIN32
#endif

// I checked version 6.0 build 020312Z, it implements the NRVO.
// Correct this as you find out which version of the compiler
// implemented the NRVO first.  (Daniel Frey)
#if (AUTOBOOST_INTEL_CXX_VERSION >= 600)
#  define AUTOBOOST_HAS_NRVO
#endif

// Branch prediction hints
// I'm not sure 8.0 was the first version to support these builtins,
// update the condition if the version is not accurate. (Andrey Semashev)
#if defined(__GNUC__) && AUTOBOOST_INTEL_CXX_VERSION >= 800
#define AUTOBOOST_LIKELY(x) __builtin_expect(x, 1)
#define AUTOBOOST_UNLIKELY(x) __builtin_expect(x, 0)
#endif

// RTTI
// __RTTI is the EDG macro
// __INTEL_RTTI__ is the Intel macro
// __GXX_RTTI is the g++ macro
// _CPPRTTI is the MSVC++ macro
#if !defined(__RTTI) && !defined(__INTEL_RTTI__) && !defined(__GXX_RTTI) && !defined(_CPPRTTI)

#if !defined(AUTOBOOST_NO_RTTI)
# define AUTOBOOST_NO_RTTI
#endif

// in MS mode, static typeid works even when RTTI is off
#if !defined(_MSC_VER) && !defined(AUTOBOOST_NO_TYPEID)
# define AUTOBOOST_NO_TYPEID
#endif

#endif

//
// versions check:
// we don't support Intel prior to version 6.0:
#if AUTOBOOST_INTEL_CXX_VERSION < 600
#  error "Compiler not supported or configured - please reconfigure"
#endif

// Intel on MacOS requires
#if defined(__APPLE__) && defined(__INTEL_COMPILER)
#  define AUTOBOOST_NO_TWO_PHASE_NAME_LOOKUP
#endif

// Intel on Altix Itanium
#if defined(__itanium__) && defined(__INTEL_COMPILER)
#  define AUTOBOOST_NO_TWO_PHASE_NAME_LOOKUP
#endif

//
// An attempt to value-initialize a pointer-to-member may trigger an
// internal error on Intel <= 11.1 (last checked version), as was
// reported by John Maddock, Intel support issue 589832, May 2010.
// Moreover, according to test results from Huang-Vista-x86_32_intel,
// intel-vc9-win-11.1 may leave a non-POD array uninitialized, in some
// cases when it should be value-initialized.
// (Niels Dekker, LKEB, May 2010)
// Apparently Intel 12.1 (compiler version number 9999 !!) has the same issue (compiler regression).
#if defined(__INTEL_COMPILER)
#  if (__INTEL_COMPILER <= 1110) || (__INTEL_COMPILER == 9999) || (defined(_WIN32) && (__INTEL_COMPILER < 1600))
#    define AUTOBOOST_NO_COMPLETE_VALUE_INITIALIZATION
#  endif
#endif

//
// Dynamic shared object (DSO) and dynamic-link library (DLL) support
//
#if defined(__GNUC__) && (__GNUC__ >= 4)
#  define AUTOBOOST_SYMBOL_EXPORT __attribute__((visibility("default")))
#  define AUTOBOOST_SYMBOL_IMPORT
#  define AUTOBOOST_SYMBOL_VISIBLE __attribute__((visibility("default")))
#endif
//
// C++0x features
// For each feature we need to check both the Intel compiler version,
// and the version of MSVC or GCC that we are emulating.
// See http://software.intel.com/en-us/articles/c0x-features-supported-by-intel-c-compiler/
// for a list of which features were implemented in which Intel releases.
//
#if defined(AUTOBOOST_INTEL_STDCXX0X)
// AUTOBOOST_NO_CXX11_CONSTEXPR:
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1500) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40600)) && !defined(_MSC_VER)
// Available in earlier Intel versions, but fail our tests:
#  undef AUTOBOOST_NO_CXX11_CONSTEXPR
#endif
// AUTOBOOST_NO_CXX11_NULLPTR:
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1210) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40600)) && (!defined(_MSC_VER) || (_MSC_VER >= 1600))
#  undef AUTOBOOST_NO_CXX11_NULLPTR
#endif
// AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1210) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40700)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 180020827))
#  undef AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES
#endif

// AUTOBOOST_NO_CXX11_DECLTYPE
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40300)) && (!defined(_MSC_VER) || (_MSC_VER >= 1600))
#  undef AUTOBOOST_NO_CXX11_DECLTYPE
#endif

// AUTOBOOST_NO_CXX11_DECLTYPE_N3276
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1500) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40800)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 180020827))
#  undef AUTOBOOST_NO_CXX11_DECLTYPE_N3276
#endif

// AUTOBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40300)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 180020827))
#  undef AUTOBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#endif

// AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1300) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40300)) && (!defined(_MSC_VER) || (_MSC_VER >= 1600))
// This is available from earlier Intel versions, but breaks Filesystem and other libraries:
#  undef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#endif

// AUTOBOOST_NO_CXX11_STATIC_ASSERT
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1110) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40300)) && (!defined(_MSC_VER) || (_MSC_VER >= 1600))
#  undef AUTOBOOST_NO_CXX11_STATIC_ASSERT
#endif

// AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40400)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 180020827))
#  undef AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
#endif

// AUTOBOOST_NO_CXX11_VARIADIC_MACROS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40200)) && (!defined(_MSC_VER) || (_MSC_VER >= 1400))
#  undef AUTOBOOST_NO_CXX11_VARIADIC_MACROS
#endif

// AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40400)) && (!defined(_MSC_VER) || (_MSC_VER >= 1600))
#  undef AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS
#endif

// AUTOBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40400)) && (!defined(_MSC_VER) || (_MSC_VER >= 1600))
#  undef AUTOBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#endif

// AUTOBOOST_NO_CXX11_CHAR16_T
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1400) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40400)) && (!defined(_MSC_VER) || (_MSC_VER >= 9999))
#  undef AUTOBOOST_NO_CXX11_CHAR16_T
#endif

// AUTOBOOST_NO_CXX11_CHAR32_T
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1400) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40400)) && (!defined(_MSC_VER) || (_MSC_VER >= 9999))
#  undef AUTOBOOST_NO_CXX11_CHAR32_T
#endif

// AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40400)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 180020827))
#  undef AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#endif

// AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40400)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 180020827))
#  undef AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS
#endif

// AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1400) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40400)) && (!defined(_MSC_VER) || (_MSC_VER >= 1700))
#  undef AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#endif

// AUTOBOOST_NO_CXX11_SCOPED_ENUMS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1400) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40501)) && (!defined(_MSC_VER) || (_MSC_VER >= 1700))
// This is available but broken in earlier Intel releases.
#  undef AUTOBOOST_NO_CXX11_SCOPED_ENUMS
#endif

// AUTOBOOST_NO_SFINAE_EXPR
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40500)) && (!defined(_MSC_VER) || (_MSC_VER >= 9999))
#  undef AUTOBOOST_NO_SFINAE_EXPR
#endif

// AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1500) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40500)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 180020827))
// This is available in earlier Intel releases, but breaks Multiprecision:
#  undef AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#endif

// AUTOBOOST_NO_CXX11_LAMBDAS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40500)) && (!defined(_MSC_VER) || (_MSC_VER >= 1600))
#  undef AUTOBOOST_NO_CXX11_LAMBDAS
#endif

// AUTOBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40500))
#  undef AUTOBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#endif

// AUTOBOOST_NO_CXX11_RANGE_BASED_FOR
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1400) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40600)) && (!defined(_MSC_VER) || (_MSC_VER >= 1700))
#  undef AUTOBOOST_NO_CXX11_RANGE_BASED_FOR
#endif

// AUTOBOOST_NO_CXX11_RAW_LITERALS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1400) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40500)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 180020827))
#  undef AUTOBOOST_NO_CXX11_RAW_LITERALS
#endif

// AUTOBOOST_NO_CXX11_UNICODE_LITERALS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1400) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40500)) && (!defined(_MSC_VER) || (_MSC_VER >= 9999))
#  undef AUTOBOOST_NO_CXX11_UNICODE_LITERALS
#endif

// AUTOBOOST_NO_CXX11_NOEXCEPT
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1500) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40600)) && (!defined(_MSC_VER) || (_MSC_VER >= 9999))
// Available in earlier Intel release, but generates errors when used with
// conditional exception specifications, for example in multiprecision:
#  undef AUTOBOOST_NO_CXX11_NOEXCEPT
#endif

// AUTOBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1400) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40600)) && (!defined(_MSC_VER) || (_MSC_VER >= 9999))
#  undef AUTOBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#endif

// AUTOBOOST_NO_CXX11_USER_DEFINED_LITERALS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1500) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40700)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 190021730))
#  undef AUTOBOOST_NO_CXX11_USER_DEFINED_LITERALS
#endif

// AUTOBOOST_NO_CXX11_ALIGNAS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1500) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40800)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 190021730))
#  undef AUTOBOOST_NO_CXX11_ALIGNAS
#endif

// AUTOBOOST_NO_CXX11_TRAILING_RESULT_TYPES
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1200) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40400)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 180020827))
#  undef AUTOBOOST_NO_CXX11_TRAILING_RESULT_TYPES
#endif

// AUTOBOOST_NO_CXX11_INLINE_NAMESPACES
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1400) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40400)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 190021730))
#  undef AUTOBOOST_NO_CXX11_INLINE_NAMESPACES
#endif

// AUTOBOOST_NO_CXX11_REF_QUALIFIERS
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1400) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40800)) && (!defined(_MSC_VER) || (_MSC_FULL_VER >= 190021730))
#  undef AUTOBOOST_NO_CXX11_REF_QUALIFIERS
#endif

// AUTOBOOST_NO_CXX11_FINAL
#if (AUTOBOOST_INTEL_CXX_VERSION >= 1400) && (!defined(AUTOBOOST_INTEL_GCC_VERSION) || (AUTOBOOST_INTEL_GCC_VERSION >= 40700)) && (!defined(_MSC_VER) || (_MSC_VER >= 1700))
#  undef AUTOBOOST_NO_CXX11_FINAL
#endif

#endif

//
// Broken in all versions up to 15:
#define AUTOBOOST_NO_CXX11_FIXED_LENGTH_VARIADIC_TEMPLATE_EXPANSION_PACKS

#if defined(AUTOBOOST_INTEL_STDCXX0X) && (AUTOBOOST_INTEL_CXX_VERSION <= 1310)
#  define AUTOBOOST_NO_CXX11_HDR_FUTURE
#  define AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#endif

#if defined(AUTOBOOST_INTEL_STDCXX0X) && (AUTOBOOST_INTEL_CXX_VERSION == 1400)
// A regression in Intel's compiler means that <tuple> seems to be broken in this release as well as <future> :
#  define AUTOBOOST_NO_CXX11_HDR_FUTURE
#  define AUTOBOOST_NO_CXX11_HDR_TUPLE
#endif

#if (AUTOBOOST_INTEL_CXX_VERSION < 1200)
//
// fenv.h appears not to work with Intel prior to 12.0:
//
#  define AUTOBOOST_NO_FENV_H
#endif

// Intel 13.10 fails to access defaulted functions of a base class declared in private or protected sections,
// producing the following errors:
// error #453: protected function "..." (declared at ...") is not accessible through a "..." pointer or object
#if (AUTOBOOST_INTEL_CXX_VERSION <= 1310)
#  define AUTOBOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#  define AUTOBOOST_HAS_STDINT_H
#endif

#if defined(__LP64__) && defined(__GNUC__) && (AUTOBOOST_INTEL_CXX_VERSION >= 1310) && !defined(__CUDACC__)
#  define AUTOBOOST_HAS_INT128
#endif

#endif
//
// last known and checked version:
#if (AUTOBOOST_INTEL_CXX_VERSION > 1500)
#  if defined(AUTOBOOST_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  elif defined(_MSC_VER)
//
//      We don't emit this warning any more, since we have so few
//      defect macros set anyway (just the one).
//
//#     pragma message("Unknown compiler version - please run the configure tests and report the results")
#  endif
#endif


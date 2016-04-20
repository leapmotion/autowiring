// (C) Copyright Thomas Witt    2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// no include guard multiple inclusion intended

//
// This is a temporary workaround until the bulk of this is
// available in autoboost config.
// 23/02/03 thw
//

#undef AUTOBOOST_NO_IS_CONVERTIBLE
#undef AUTOBOOST_NO_IS_CONVERTIBLE_TEMPLATE
#undef AUTOBOOST_NO_STRICT_ITERATOR_INTEROPERABILITY
#undef AUTOBOOST_NO_LVALUE_RETURN_DETECTION
#undef AUTOBOOST_NO_ONE_WAY_ITERATOR_INTEROP

#ifdef AUTOBOOST_ITERATOR_CONFIG_DEF
# undef AUTOBOOST_ITERATOR_CONFIG_DEF
#else
# error missing or nested #include config_def
#endif

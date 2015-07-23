// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include <utility>

namespace autowiring {
  template<typename T, int>
  struct is_complete {
#ifdef _MSC_VER
    // MSVC requires that we try to invoke the copy ctor in order for SFINAE to
    // work properly.  The copy ctor will be missing, and the other overload selected.
    static T & getT();
    static std::true_type select(T);
    static std::false_type select(...);
    static const bool value = decltype(select(getT()))::value;
#else
    // GCC/CLANG requires that we attempt to take the size, as the SFINAE behavior
    // that works on MSVC is not portable.
    template<typename U>
    static std::integral_constant<bool, sizeof(U) == sizeof(U)> select(U*);

    template<typename U>
    static std::false_type select(...);

    static const bool value = decltype(select<T>(nullptr))::value;
#endif
  };
};

// Current target platforms all support __COUNTER__
#define AUTO_IS_COMPLETE(T) ::autowiring::is_complete<T, __COUNTER__>::value
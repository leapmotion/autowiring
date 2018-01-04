// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once

namespace autowiring {
/*
 Variadic generalizations of boolean operations.
 Single argument cases are equal to the argument.
 Zero argument cases are defined by the requirement
 that the result is idempotent with respect to the
 boolean operation.
 */

/// <summary>Variadic OR</summary>
template<bool... Args>
struct var_or {
  static const bool value = false;
};
template<bool Head, bool... Tail>
struct var_or<Head, Tail...> {
  static const bool value = Head || var_or<Tail...>::value;
};

/// <summary>Variadic AND</summary>
template<bool... Args>
struct var_and {
  static const bool value = true;
};
template<bool Head, bool... Tail>
struct var_and<Head, Tail...> {
  static const bool value = Head && var_and<Tail...>::value;
};

}

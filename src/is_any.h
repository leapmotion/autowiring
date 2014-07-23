#pragma once

// Check if any T::value is true
template<typename... T>
struct is_any{
  static const bool value = false;
};

template<typename Head, typename... Tail>
struct is_any<Head, Tail...>{
  static const bool value = Head::value || is_any<Tail...>::value;
};
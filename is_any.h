template<typename... T>
struct is_any{};

template<typename ToCheck>
struct is_any<ToCheck>{
  static const bool value = false;
};

template<typename ToCheck, typename Head, typename... Tail>
struct is_any<ToCheck, Head, Tail...>{
  static const bool value = std::is_same<ToCheck,Head>::value || is_any<ToCheck,Tail...>::value;
};

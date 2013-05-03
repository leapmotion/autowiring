#ifndef _CAST_UTILITY_H
#define _CAST_UTILITY_H

template<class To, class From, bool isPolymorphic = std::is_polymorphic<From>::value>
struct safe_dynamic_cast
{
  static To* Cast(From*) {
    return nullptr;
  }
};

template<class To, class From>
struct safe_dynamic_cast<To, From, true>
{
  static To* Cast(From* p) {
    return dynamic_cast<To*>(p);
  }
};

#endif
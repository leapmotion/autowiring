// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CAST_UTILITY_H
#define _CAST_UTILITY_H

template<class To, class From, bool isPolymorphic = std::is_polymorphic<From>::value>
struct safe_dynamic_cast
{
  static To* Cast(From*) {
    return nullptr;
  }

  static std::shared_ptr<To> Cast(const std::shared_ptr<From>& rhs) {
    return std::shared_ptr<To>();
  }
};

template<class To, class From>
struct safe_dynamic_cast<To, From, true>
{
  static To* Cast(From* p) {
    return dynamic_cast<To*>(p);
  }

  static std::shared_ptr<To> Cast(const std::shared_ptr<From>& rhs) {
    return std::dynamic_pointer_cast<To>(rhs);
  }
};

#endif

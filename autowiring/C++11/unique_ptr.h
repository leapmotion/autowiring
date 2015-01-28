// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autoboost/type_traits/remove_reference.hpp>

namespace std {

template<class Type>
struct default_delete {
  void operator()(Type* ptr) const {
    delete ptr;
  }
};

// We provide a very simplistic trivial unique_ptr with move semantics,
// but not much else.
template<class Type, class Del = default_delete<Type> >
class unique_ptr {
public:
  typedef Type element_type;
  typedef Del deleter_type;
  typedef element_type* pointer;

  unique_ptr(void):
    stored_ptr(nullptr)
  {}

  explicit unique_ptr(pointer _Ptr):
    stored_ptr(_Ptr)
  {
  }

  unique_ptr(pointer _Ptr, typename autoboost::remove_reference<Del>::type&& _Deleter) :
    stored_ptr(_Ptr),
    stored_deleter(_Deleter)
  {}

  unique_ptr(unique_ptr&& _Right):
    stored_ptr(_Right.release()),
    stored_deleter(_Right.stored_deleter)
  {
  }

  unique_ptr(const unique_ptr&) = delete;
  unique_ptr& operator=(const unique_ptr&) = delete;

  ~unique_ptr() {
    if(stored_ptr)
      stored_deleter(stored_ptr);
  }

private:
  pointer stored_ptr;    // exposition only
  Del stored_deleter;    // exposition only

public:
  unique_ptr& operator=(unique_ptr&& _Right) {
    reset(_Right.release());
    stored_deleter = _Right.stored_deleter;
    return *this;
  }

  template<class Type2, class Del2>
  unique_ptr& operator=(unique_ptr<Type2, Del2>&& _Right) {
    reset(_Right.release());
    stored_deleter = _Right.stored_deleter;
    return *this;
  }

  void swap(unique_ptr& _Right) {
    std::swap(stored_ptr, _Right->stored_ptr);
    std::swap(stored_deleter, _Right->stored_deleter);
  }

  pointer release () {
    pointer retVal = stored_ptr;
    stored_ptr = nullptr;
    return retVal;
  }
  void reset(pointer _Ptr = nullptr) {
    if(stored_ptr)
      stored_deleter(stored_ptr);
    stored_ptr = _Ptr;
  }

  pointer get () const {return stored_ptr;}
  Type& operator* () const {return *stored_ptr;}
  pointer operator-> () const {return stored_ptr;}
  explicit operator bool () const {return !!stored_ptr;}

  friend bool operator==(const unique_ptr& lhs, const unique_ptr& rhs) {return lhs.stored_ptr == rhs.stored_ptr;}
  friend bool operator!=(const unique_ptr& lhs, const unique_ptr& rhs) {return lhs.stored_ptr != rhs.stored_ptr;}
  friend bool operator< (const unique_ptr& lhs, const unique_ptr& rhs) {return lhs.stored_ptr < rhs.stored_ptr;}
  friend bool operator<=(const unique_ptr& lhs, const unique_ptr& rhs) {return lhs.stored_ptr <= rhs.stored_ptr;}
  friend bool operator> (const unique_ptr& lhs, const unique_ptr& rhs) {return lhs.stored_ptr > rhs.stored_ptr;}
  friend bool operator>=(const unique_ptr& lhs, const unique_ptr& rhs) {return lhs.stored_ptr >= rhs.stored_ptr;}
  friend bool operator==(const unique_ptr& lhs, decltype(nullptr)) noexcept {return lhs.stored_ptr == nullptr;}
  friend bool operator!=(const unique_ptr& lhs, decltype(nullptr)) noexcept {return lhs.stored_ptr != nullptr;}
  friend bool operator< (const unique_ptr& lhs, decltype(nullptr)) {return lhs.stored_ptr < nullptr;}
  friend bool operator<=(const unique_ptr& lhs, decltype(nullptr)) {return lhs.stored_ptr <= nullptr;}
  friend bool operator> (const unique_ptr& lhs, decltype(nullptr)) {return lhs.stored_ptr > nullptr;}
  friend bool operator>=(const unique_ptr& lhs, decltype(nullptr)) {return lhs.stored_ptr >= nullptr;}
  friend bool operator==(decltype(nullptr), const unique_ptr& rhs) noexcept {return nullptr == rhs.stored_ptr;}
  friend bool operator!=(decltype(nullptr), const unique_ptr& rhs) noexcept {return nullptr != rhs.stored_ptr;}
  friend bool operator> (decltype(nullptr), const unique_ptr& rhs) {return nullptr > rhs.stored_ptr;}
  friend bool operator>=(decltype(nullptr), const unique_ptr& rhs) {return nullptr >= rhs.stored_ptr;}
  friend bool operator< (decltype(nullptr), const unique_ptr& rhs) {return nullptr < rhs.stored_ptr;}
  friend bool operator<=(decltype(nullptr), const unique_ptr& rhs) {return nullptr <= rhs.stored_ptr;}
};

} //namespace std

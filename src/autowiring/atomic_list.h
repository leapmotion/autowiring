// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "callable.h"
#include "spin_lock.h"
#include <iterator>
#include <type_traits>

namespace autowiring {
  /// <summary>
  /// An atomic list of items awaiting execution
  /// </summary>
  /// <remarks>
  /// Items in this collection must inherit callable_base.  This type provides a forward link and invocation
  /// concept because the atomic list is mainly intended to serve as a low layer lightweight dispatch queue
  /// implementation.
  /// </remarks>
  struct atomic_list {
  public:
    atomic_list(void) = default;
    atomic_list(const atomic_list&) = delete;
    ~atomic_list(void);

  private:
    autowiring::spin_lock lock;

    // First entry in the atomic list
    callable_base* m_pHead = nullptr;

    // Current chain identifier:
    uint32_t m_chainID = 0;

  public:
    bool empty(void) volatile const { return m_pHead == nullptr; }

    /// <returns>
    /// The current chain identifier
    /// </returns>
    /// <remarks>
    /// No guarantee is made about how long the returned value will be valid.  Calls to
    /// `release` will change the returned value.
    /// </remarks>
    uint32_t chain_id(void) volatile const throw() { return m_chainID; }

    /// <summary>
    /// Adds the already-constructed entry to the ist
    /// </summary>
    uint32_t push_entry(callable_base* e) throw();

    template<typename T>
    uint32_t push(T&& val) {
      static_assert(std::is_base_of<callable_base, T>::value, "Can only push types which inherit from callable_base");
      return push_entry(new T{ std::forward<T&&>(val) });
    }

    template<typename T, typename... Args>
    uint32_t emplace(Args&&... args) {
      static_assert(std::is_base_of<callable_base, T>::value, "Can only push types which inherit from callable_base");
      return push_entry(new T{ std::forward<Args&&>(args)... });
    }

    /// <summary>
    /// Represents a single chain of elements added to the list
    /// </summary>
    template<typename T>
    struct chain {
      chain(const chain&) = delete;
      chain(chain&& rhs) :
        head(rhs.head)
      {
        rhs.head = nullptr;
      }

      chain(callable_base* head) :
        head(head)
      {}
      ~chain(void) {
        // Deletion convenience:
        for(auto& x : *this) {
          (void)x;
        }
      }

      struct end_iterator;

      struct iterator {
        iterator(void) = default;
        iterator(callable_base*& e) :
          e(e)
        {}

        typedef std::input_iterator_tag iterator_category;
        typedef T value_type;
        typedef size_t difference_type;
        typedef const T* pointer;
        typedef value_type reference;

      private:
        callable_base*& e;

      public:
        iterator& operator++(int) = delete;

        iterator& operator++(void) {
          auto prior = e;
          e = e->m_pFlink;
          delete prior;
          return *this;
        }

        T& operator*(void) const { return static_cast<T&>(*e); }
        T* operator->(void) const { return static_cast<T*>(e); }
        bool operator==(const iterator& rhs) const { return e == rhs.e; }
        bool operator!=(const iterator& rhs) const { return e != rhs.e; }
        bool operator==(const end_iterator& rhs) const { return !e; }
        bool operator!=(const end_iterator& rhs) const { return !!e; }
      };

    private:
      callable_base* head = nullptr;

    public:
      iterator begin(void) {
        return{ head };
      }

      iterator end(void) {
        static callable_base* np = nullptr;
        return{ np };
      }

      chain& operator=(const chain& rhs) = delete;
      chain& operator=(chain&& rhs) {
        auto temp = rhs.head;
        rhs.head = head;
        head = temp;
        return *this;
      }
    };

    /// <summary>
    /// Retrieves a list of entries in the order of insertion
    /// </summary>
    /// <remarks>
    /// The caller is responsible for recursively freeing the returned entry
    /// </remarks>
    callable_base* release(void) throw();

    /// <summary>
    /// Retrieves a list of entries in the order of insertion
    /// </summary>
    template<typename T>
    chain<T> release(void) throw() {
      static_assert(std::is_base_of<callable_base, T>::value, "Can only obtain chains of types that inherit from callable_base");
      return { release() };
    }
  };
}

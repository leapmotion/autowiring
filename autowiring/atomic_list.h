// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <atomic>
#include <iterator>
#include <type_traits>
#include <utility>

namespace autowiring {
  struct atomic_entry {
    atomic_entry(void) = default;
    virtual ~atomic_entry(void) {};

    atomic_entry* pFlink = nullptr;
  };

  struct atomic_list {
  public:
    atomic_list(void) = default;
    atomic_list(const atomic_list&) = delete;
    ~atomic_list(void);

  private:
    // First entry in the atomic list
    std::atomic<atomic_entry*> m_pHead{ nullptr };

    // Current chain identifier
    std::atomic<uint32_t> m_chainID{ 0 };

  public:
    /// <returns>
    /// The current chain identifier
    /// </returns>
    /// <remarks>
    /// No guarantee is made about how long the returned value will be valid.  Calls to
    /// `release` will change the returned value.
    /// </remarks>
    uint32_t chain_id(void) volatile const throw() { return m_chainID.load(std::memory_order_relaxed); }

    /// <summary>
    /// Adds the already-constructed entry to the ist
    /// </summary>
    uint32_t push_entry(atomic_entry* e) throw();

    template<typename T>
    uint32_t push(T&& val) {
      static_assert(std::is_base_of<atomic_entry, T>::value, "Can only push types which inherit from atomic_entry");
      return push_entry(new T{ std::forward<T&&>(val) });
    }

    template<typename T, typename... Args>
    uint32_t emplace(Args... args) {
      static_assert(std::is_base_of<atomic_entry, T>::value, "Can only push types which inherit from atomic_entry");
      return push_entry(new T{ std::forward<Args>(args)... });
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

      chain(atomic_entry* head) :
        head(head)
      {}
      ~chain(void) {
        // Deletion convenience:
        for(auto& x : *this) {}
      }

      struct end_iterator;

      struct iterator {
        iterator(void) = default;
        iterator(atomic_entry*& e) :
          e(e)
        {}

        typedef std::input_iterator_tag iterator_category;
        typedef T value_type;
        typedef size_t difference_type;
        typedef const T* pointer;
        typedef value_type reference;

      private:
        atomic_entry*& e;

      public:
        iterator& operator++(int) = delete;

        iterator& operator++(void) {
          auto prior = e;
          e = e->pFlink;
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
      atomic_entry* head = nullptr;

    public:
      iterator begin(void) {
        return{ head };
      }

      iterator end(void) {
        static atomic_entry* np = nullptr;
        return{ np };
      }

      chain& operator=(const chain& rhs) = delete;
      chain& operator=(chain&& rhs) {
        std::swap(head, rhs.head);
        return *this;
      }
    };

    /// <summary>
    /// Retrieves a list of entries in the order of insertion
    /// </summary>
    /// <remarks>
    /// The caller is responsible for freeing the returned entry
    /// </remarks>
    template<typename T>
    chain<T> release(void) throw() {
      static_assert(std::is_base_of<atomic_entry, T>::value, "Can only obtain chains of types that inherit from atomic_entry");

      atomic_entry* head = m_pHead.load(std::memory_order_relaxed);
      do m_chainID.fetch_add(1, std::memory_order_acq_rel);
      while (
        !m_pHead.compare_exchange_weak(
          head,
          nullptr,
          std::memory_order_release,
          std::memory_order_relaxed
        )
      );
      
      // Take ownership of the dispatcher list and reverse this forward-linked list.
      atomic_entry* lastLink = nullptr;
      for (; head; std::swap(head, lastLink))
        std::swap(lastLink, head->pFlink);
      return { lastLink };
    }
  };
}

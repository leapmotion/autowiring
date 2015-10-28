// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <atomic>

namespace autowiring {
  template<typename T>
  struct atomic_list {
  public:
    ~atomic_list(void) {
      entry* next;
      for (entry* cur = m_pHead.load(std::memory_order_relaxed); cur; cur = next) {
        next = cur->pFlink;
        delete cur;
      }
    }

    struct entry {
      entry(void) = default;
      entry(const T& value) :
        value(value)
      {}
      entry(T&& value) :
        value(std::move(value))
      {}

      entry* pFlink = nullptr;
      T value;
    };

  private:
    // First entry in the atomic list
    std::atomic<entry*> m_pHead{ nullptr };

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
    uint32_t push(entry* e) throw() {
      entry* pHead;
      uint32_t retVal;
      do {
        pHead = m_pHead.load(std::memory_order_acquire);
        retVal = m_chainID.load(std::memory_order_relaxed);
        e->pFlink = pHead;
      } while (
        !m_pHead.compare_exchange_weak(
          pHead,
          e,
          std::memory_order_release,
          std::memory_order_relaxed
        )
      );
      return retVal;
    }

    uint32_t push(T&& rhs) { return push(new entry(std::move(rhs))); }
    uint32_t push(const T& rhs) { return push(new entry(rhs)); }

    /// <summary>
    /// Represents a single chain of elements added to the list
    /// </summary>
    struct chain {
      chain(const chain&) = delete;
      chain(chain&& rhs) :
        head(rhs.head)
      {
        rhs.head = nullptr;
      }

      chain(entry* head) :
        head(head)
      {}
      ~chain(void) {
        // Deletion convenience:
        for(auto& x : *this) {}
      }

      struct end_iterator;

      struct iterator {
        iterator(void) = default;
        iterator(entry*& e) :
          e(e)
        {}

        typedef std::input_iterator_tag iterator_category;
        typedef T value_type;
        typedef size_t difference_type;
        typedef const T* pointer;
        typedef value_type reference;

      private:
        entry*& e;

      public:
        iterator& operator++(int) = delete;

        iterator& operator++(void) {
          auto prior = e;
          e = e->pFlink;
          delete prior;
          return *this;
        }

        T& operator*(void) const { return e->value; }
        T* operator->(void) const { return &e->value; }
        bool operator==(const iterator& rhs) const { return e == rhs.e; }
        bool operator!=(const iterator& rhs) const { return e != rhs.e; }
        bool operator==(const end_iterator& rhs) const { return !e; }
        bool operator!=(const end_iterator& rhs) const { return !!e; }
      };

    private:
      entry* head = nullptr;

    public:
      iterator begin(void) {
        return{ head };
      }

      iterator end(void) {
        static entry* np = nullptr;
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
    chain release(void) throw() {
      entry* head = m_pHead.load(std::memory_order_relaxed);
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
      entry* lastLink = nullptr;
      for (; head; std::swap(head, lastLink))
        std::swap(lastLink, head->pFlink);
      return lastLink;
    }
  };
}

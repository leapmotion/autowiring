// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace autowiring {
  struct signal_relay_registration_table;
}

/// <summary>
/// Implements an asynchronous signal concept as an AutoFired alternative
/// </summary>
namespace autowiring {
  template<typename T>
  struct signal;

  struct signal_relay;

  namespace internal {
    // Linked list entry
    struct signal_node_base {
      signal_node_base(void) :
        pFlink(nullptr),
        pBlink(nullptr)
      {}

      virtual ~signal_node_base(void) {}

      // Forward and backward linkages:
      signal_node_base* pFlink;
      signal_node_base* pBlink;
      
      /// <summary>
      /// Removes this node from the list it's in, or throws if it's the head.
      /// </summary>
      void remove(){
        // Clear linkage
        if (this->pBlink)
          this->pBlink->pFlink = this->pFlink;
        if (this->pFlink)
          this->pFlink->pBlink = this->pBlink;

        // If we're the head pointer, throw an exception
        if (!this->pBlink)
          throw std::runtime_error("Cannot remove the head of the list");

        // Fully unlinked, delete
        delete this;
      }
    };

    // Holds a reference to one of the signal holders
    template<typename... Args>
    struct signal_node:
      signal_node_base
    {
      signal_node(const signal_node& rhs) = delete;

      signal_node(std::function<void(Args...)>&& fn) :
        fn(std::move(fn))
      {}

      const std::function<void(Args...)> fn;

      using signal_node_base::remove;

      /// <summary>
      /// Appends the specified handler to this list of nodes.
      /// </summary>
      internal::signal_node<Args...>* operator+=(std::function<void(Args...)>&& fn) {
        auto retVal = new internal::signal_node<Args...>(std::move(fn));
        retVal->pBlink = this;
        retVal->pFlink = pFlink;
        if (pFlink)
          pFlink->pBlink = retVal;
        pFlink = retVal;
        return retVal;
      }
    };

    struct signal_registration_base {
      signal_registration_base(void);

      // Associates signal entries with their corresponding relays
      std::unordered_map<size_t, std::shared_ptr<signal_relay>> entries;
    };

    /// <summary>
    /// Associates offsets in type T with signals in that type
    /// </summary>
    template<class T>
    struct signal_registration:
      signal_registration_base
    {};

    /// <summary>
    /// Obtains the signal relay for the specified member type:
    /// </summary>
    std::shared_ptr<signal_relay> ObtainRelay(void* pMember);
  }

  /// <summary>
  /// Associates abstract offsets with signal relays
  /// </summary>
  /// <remarks>
  /// One of these registration tables exists per context.
  /// </remarks>
  struct signal_relay_registration_table {
    struct registration {
      registration(const std::type_info* ti, size_t offset) :
        ti(ti),
        offset(offset)
      {}

      const std::type_info* ti;
      size_t offset;

      bool operator==(const registration& rhs) const {
        return *ti == *rhs.ti && offset == rhs.offset;
      }
    };

    struct registration_hash {
      size_t operator()(const registration& reg) const {
        return reg.ti->hash_code() + reg.offset;
      }
    };

    // Lock for the table:
    std::mutex lock;

    /// <summary>
    /// Associates an offset in the relay registration with a signal relay
    /// </summary>
    /// <remarks>
    /// This map assumes that users have some other way of inferring the types on the key side.
    /// </remarks>
    std::unordered_map<registration, std::shared_ptr<signal_relay>, registration_hash> table;

    /// <summary>
    /// Creates or returns the relay at the specified offset
    /// </summary>
    std::shared_ptr<signal_relay> GetSignalRelay(const std::type_info& ti, size_t offset);
  };

  /// <summary>
  /// Stores a signal 
  /// </summary>
  /// <remarks>
  /// This is functionally a sentinal head of the linked list.
  /// </remarks>
  struct signal_relay :
    internal::signal_node_base
  {
    signal_relay(void) {}

    ~signal_relay(void) {
      // Standard linked list cleaup
      internal::signal_node_base* next = nullptr;
      for (auto cur = pFlink; cur; cur = next) {
        next = cur->pFlink;
        delete cur;
      }
    }

    /// <summary>
    /// Searches the list for this node and removes it, or throws if it is not found.
    /// </summary>
    void operator-=(signal_node_base* node) {
      signal_node_base* cur;
      for (cur = pFlink; cur != nullptr; cur = cur->pFlink) {
        if (cur == node)
          return node->remove();
      }

      throw std::runtime_error("Attempted to remove node which is not part of this list.");
    }
  };

  /// <summary>
  /// Descriptor type used to add entries that will be constructed in a signal relay
  /// </summary>
  template<typename... Args>
  struct signal_relay_t:
    signal_relay
  {
    internal::signal_node<Args...>* GetHead(void) const {
      return static_cast<internal::signal_node<Args...>*>(pFlink);
    }

    /// <summary>
    /// Attaches the specified handler to this signal
    /// </summary>
    internal::signal_node<Args...>* operator+=(std::function<void(Args...)>&& fn) {
      return *reinterpret_cast<internal::signal_node<Args...>*>(this) += std::move(fn);
    }
  };

  /// <summary>
  /// A signal registration entry, for use as an embedded member variable of a context member.
  /// </summary>
  template<typename... Args>
  struct signal<void(Args...)>
  {
  public:
    signal(void) :
      m_relay(
        std::static_pointer_cast<signal_relay_t<Args...>>(
          internal::ObtainRelay(this)
        )
      )
    {}

  private:
    const std::shared_ptr<signal_relay_t<Args...>> m_relay;

  public:
    typedef internal::signal_node<Args...> registration_t;
    typedef std::function<void(Args...)> function_t;

    registration_t* operator+=(function_t&& fn) { return *m_relay += std::move(fn); }
    void operator-=(registration_t* node) { return *m_relay -= node; }

    /// <summary>
    /// Raises the signal and invokes all attached handlers
    /// </summary>
    void operator()(Args... args) const {
      for (
        auto cur = m_relay->GetHead();
        cur;
        cur = static_cast<decltype(cur)>(cur->pFlink)
      )
        cur->fn(args...);
    }
  };
}

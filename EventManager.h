#ifndef _EVENT_MANAGER_H
#define _EVENT_MANAGER_H
#include "ocuConfig.h"
#include "EventReceiver.h"
#include <boost/bind.hpp>
#include <map>
#include <set>

class DispatchQueue;

/// <summary>
/// Used to identify event managers
/// </summary>
class EventManagerBase {
public:
  virtual ~EventManagerBase(void);

protected:
  // Just the DispatchQueue listeners:
  typedef std::set<DispatchQueue*> t_stType;
  t_stType m_dispatch;
  
public:
  /// <summary>
  /// Invoked by the parent context when the context is shutting down in order to release all references
  /// </summary>
  virtual void Release(void) = 0;
  virtual EventManagerBase& operator+=(std::shared_ptr<EventReceiver>& rhs) = 0;
};

/// <summary>
/// A simple event manager class
/// </summary>
/// <param name="T">The event interface type</param>
template<class T>
class EventManager:
  public EventManagerBase
{
private:
  static_assert(
    std::is_base_of<EventReceiver, T>::value,
    "If you want an event interface, the interface must inherit from EventReceiver"
  );

  // Collection of all known listeners:
  typedef std::map<T*, std::shared_ptr<T>> t_mpType;
  t_mpType m_mp;

public:
  virtual void Release() override {
    m_mp.clear();
  }

  virtual EventManagerBase& operator+=(std::shared_ptr<EventReceiver>& rhs) {
    try {
      std::shared_ptr<T> casted = std::dynamic_pointer_cast<T, EventReceiver>(rhs);
      if(casted)
        return *this += casted;
    } catch(std::bad_cast&) {
    }
    return *this;
  }

  /// <summary>
  /// Adds the specified observer to receive events dispatched from this instace
  /// </su8mmary>
  EventManager<T>& operator+=(std::shared_ptr<T>& rhs) {
    // If we already contain the specified rhs, short-circuit.
    std::shared_ptr<T>& location = m_mp[rhs.get()];
    if(!location)
      location = rhs;

    // If the RHS implements DispatchQueue, add it to that collection as well:
    DispatchQueue* pDispatch = dynamic_cast<DispatchQueue*>(rhs.get());
    if(pDispatch)
      m_dispatch.insert(pDispatch);
    return *this;
  }

  // Multi-argument firing:
  void FireAsSingle0(void (T::*fnPtr)()) const {
    for(typename t_mpType::const_iterator q = m_mp.begin(); q != m_mp.end(); ++q)
      ((q->second.get())->*fnPtr)();
  }

  template<class Arg1, class Ty1>
  void FireAsSingle1(void (T::*fnPtr)(Arg1 arg1), const Ty1& ty1) const {
    for(typename t_mpType::const_iterator q = m_mp.begin(); q != m_mp.end(); ++q)
      ((q->second.get())->*fnPtr)(ty1);
  }
  
  template<class Arg1, class Arg2, class Ty1, class Ty2>
  void FireAsSingle2(void (T::*fnPtr)(Arg1 arg1, Arg2 arg2), const Ty1& ty1, const Ty2& ty2) const {
    for(typename t_mpType::const_iterator q = m_mp.begin(); q != m_mp.end(); ++q)
      ((q->second.get())->*fnPtr)(ty1, ty2);
  }
  
  template<class Arg1, class Arg2, class Arg3, class Ty1, class Ty2, class Ty3>
  void FireAsSingle3(void (T::*fnPtr)(Arg1 arg1, Arg2 arg2, Arg3 ty3), const Ty1& ty1, const Ty2& ty2, const Ty3& ty3) const {
    for(typename t_mpType::const_iterator q = m_mp.begin(); q != m_mp.end(); ++q)
      ((q->second.get())->*fnPtr)(ty1, ty2, ty3);
  }

  
  // Two-parenthetical invocations
  std::function<void ()> Fire(void (T::*fnPtr)()) const {
    return
      [this, fnPtr] () {
        this->FireAsSingle0(fnPtr);
      };
  }

  template<class Arg1>
  std::function<void (Arg1)> Fire(void (T::*fnPtr)(Arg1)) const {
    return
      [this, fnPtr] (Arg1 arg1) {
        this->FireAsSingle1(fnPtr, arg1);
      };
  }

  template<class Arg1, class Arg2>
  std::function<void (Arg1, Arg2)> Fire(void (T::*fnPtr)(Arg1, Arg2)) const {
    return
      [this, fnPtr] (Arg1 arg1, Arg2 arg2) {
        this->FireAsSingle2(fnPtr, arg1, arg2);
      };
  }

  template<class Arg1, class Arg2, class Arg3>
  std::function<void (Arg1, Arg2, Arg3)> Fire(void (T::*fnPtr)(Arg1, Arg2, Arg3)) const {
    return
      [this, fnPtr] (Arg1 arg1, Arg2 arg2, Arg3 arg3) {
        this->FireAsSingle3(fnPtr, arg1, arg2, arg3);
      };
  }

  // Two-parenthetical deferred invocations:
  std::function<void ()> Defer(void (T::*fnPtr)()) const {
    return
      [this, fnPtr] () {
        auto f = fnPtr;
        for(EventManager<T>::t_stType::const_iterator q = m_dispatch.begin(); q != m_dispatch.end(); q++) {
          T* ptr = dynamic_cast<T*>(*q);
          **q += [ptr, f] () {
            (ptr->*f)();
          };
        }
      };
  }

  template<class Arg1>
  std::function<void (const typename std::decay<Arg1>::type&)> Defer(void (T::*fnPtr)(Arg1)) const {
    // Converted args:
    typedef std::decay<Arg1>::type tArg1;

    return
      [this, fnPtr] (const tArg1& arg1) {
        auto f = fnPtr;
        for(EventManager<T>::t_stType::const_iterator q = m_dispatch.begin(); q != m_dispatch.end(); q++) {
          T* ptr = dynamic_cast<T*>(*q);

          // Force off the const modifier so we can copy into the lambda just once
          tArg1& arg1Forced = (tArg1&)arg1;

          // Pass the copy into the lambda:
          **q += [ptr, f, arg1Forced] () mutable {
            (ptr->*f)(
              std::move(arg1Forced)
            );
          };
        }
      };
  }
};
#endif

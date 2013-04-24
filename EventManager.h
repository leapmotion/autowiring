#pragma once
#include <boost/bind.hpp>
#include <map>

/// <summary>
/// Used to identify event recipients
/// </summary>
class EventReceiver {
public:
  virtual ~EventReceiver(void) {
  }
};

/// <summary>
/// Used to identify event managers
/// </summary>
class EventManagerBase {
public:
  virtual ~EventManagerBase(void) {
  }
  
  virtual EventManagerBase& operator+=(cpp11::shared_ptr<EventReceiver>& rhs) = 0;
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
    cpp11::is_base_of<EventReceiver, T>::value,
    "Uses of EventManager must be for interfaces that implement EventReceiver"
  );

  typedef std::map<T*, cpp11::shared_ptr<T> > t_mpType;
  t_mpType m_mp;

public:
  virtual EventManagerBase& operator+=(cpp11::shared_ptr<EventReceiver>& rhs) {
    try {
      cpp11::shared_ptr<T> casted = cpp11::dynamic_pointer_cast<T, EventReceiver>(rhs);
      if(casted)
        return *this += casted;
    } catch(std::bad_cast&) {
    }
    return *this;
  }

  /// <summary>
  /// Adds the specified observer to receive events dispatched from this instace
  /// </su8mmary>
  EventManager<T>& operator+=(cpp11::shared_ptr<T>& rhs) {
    // If we already contain the specified rhs, short-circuit.
    cpp11::shared_ptr<T>& location = m_mp[rhs.get()];
    if(!location)
      location = rhs;
    return *this;
  }

  // Multi-argument firing:
  template<class Arg1, class Ty1>
  void FireAsSingle1(void (T::*fnPtr)(Arg1 arg1), const Ty1& ty1) const {
    for(typename t_mpType::const_iterator q = m_mp.begin(); q != m_mp.end(); q++)
      ((q->second.get())->*fnPtr)(ty1);
  }
  
  template<class Arg1, class Arg2, class Ty1, class Ty2>
  void FireAsSingle2(void (T::*fnPtr)(Arg1 arg1, Arg2 arg2), const Ty1& ty1, const Ty2& ty2) const {
    for(typename t_mpType::const_iterator q = m_mp.begin(); q != m_mp.end(); q++)
      ((q->second.get())->*fnPtr)(ty1, ty2);
  }
  
  template<class Arg1, class Arg2, class Arg3, class Ty1, class Ty2, class Ty3>
  void FireAsSingle3(void (T::*fnPtr)(Arg1 arg1, Arg2 arg2, Arg3 ty3), const Ty1& ty1, const Ty2& ty2, const Ty3& ty3) const {
    for(typename t_mpType::const_iterator q = m_mp.begin(); q != m_mp.end(); q++)
      ((q->second.get())->*fnPtr)(ty1, ty2, ty3);
  }

  
  // Two-parenthetical invocations
  template<class Arg1>
  cpp11::function<void (Arg1)> Fire(void (T::*fnPtr)(Arg1)) const {
    return
#if LAMBDAS_AVAILABLE
      [this, fnPtr] (Arg1 arg1) {
        this->FireAsSingle1(fnPtr, arg1);
      };
#else
      boost::bind(
        boost::bind(
          &EventManager<T>::FireAsSingle1<Arg1, Arg1>,
          this
        ),
        fnPtr
      );
#endif
  }

  template<class Arg1, class Arg2>
  cpp11::function<void (Arg1, Arg2)> Fire(void (T::*fnPtr)(Arg1, Arg2)) const {
    return
#if LAMBDAS_AVAILABLE
      [this, fnPtr] (Arg1 arg1, Arg2 arg2) {
        this->FireAsSingle2(fnPtr, arg1, arg2);
      };
#else
      boost::bind(
        boost::bind(
          &EventManager<T>::FireAsSingle2<Arg1, Arg2, Arg1, Arg2>,
          this
        ),
        fnPtr
      );
#endif
  }

  template<class Arg1, class Arg2, class Arg3>
  cpp11::function<void (Arg1, Arg2, Arg3)> Fire(void (T::*fnPtr)(Arg1, Arg2, Arg3)) const {
    return
#if LAMBDAS_AVAILABLE
      [this, fnPtr] (Arg1 arg1, Arg2 arg2, Arg3 arg3) {
        this->FireAsSingle3(fnPtr, arg1, arg2, arg3);
      };
#else
      boost::bind(
        boost::bind(
          &EventManager<T>::FireAsSingle3<Arg1, Arg2, Arg3, Arg1, Arg2, Arg3>,
          this
        ),
        fnPtr
      );
#endif
  }
};

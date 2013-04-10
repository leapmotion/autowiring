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
  template<class Arg1, class Arg2, class Ty1, class Ty2>
  void FireAsSingle(void (T::*fnPtr)(Arg1 arg1, Arg2 arg2), const Ty1& ty1, const Ty2& ty2) {
    for(typename t_mpType::iterator q = m_mp.begin(); q != m_mp.end(); q++)
      ((q->second.get())->*fnPtr)(ty1, ty2);
  }

  template<class Arg1, class Arg2>
  cpp11::function<void (Arg1, Arg2)> Fire(void (T::*fnPtr)(Arg1, Arg2)) {
#if LAMBDAS_AVAILABLE
    // Done to prevent warning spam on MSVC
    return
      [this, fnPtr] (Arg1 arg1, Arg2 arg2) {
        FireAsSingle(fnPtr, arg1, arg2);
      };
#else
    return
      boost::bind(
        boost::bind(
          &EventManager<T>::FireAsSingle<Arg1, Arg2, Arg1, Arg2>,
          this
        ),
        fnPtr
      );
    
#endif
  }
};

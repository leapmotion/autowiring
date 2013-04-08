#pragma once
#include "Autowired.h"
#include <boost/bind.hpp>
#include <map>

/// <summary>
/// A simple event manager class
/// </summary>
/// <param name="T">The event interface type</param>
template<class T>
class EventManager
{
private:
  typedef std::map<T*, shared_ptr<T> > t_mpType;
  t_mpType m_mp;

public:
  /// <summary>
  /// Adds the specified observer to receive events dispatched from this instace
  /// </su8mmary>
  EventManager<T>& operator+=(cpp11::shared_ptr<T> rhs) {
    // If we already contain the specified rhs, short-circuit.
    if(m_mp.contains(rhs.get()))
      return *this;
  }

  /// <summary>
  /// Convenience method offered when W inherits from T
  /// </summary>
  template<class W>
  EventManager<T>& operator+=(Autowired<W> rhs) {
    return operator+=(
      cpp11::static_pointer_cast<T, W>(rhs)
    );
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

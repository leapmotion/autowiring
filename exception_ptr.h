#ifndef _EXCEPTION_PTR_H
#define _EXCEPTION_PTR_H

#ifndef __APPLE__
#error This header is a hacked-in header intended only to support OS 10.6, do not make use of it under any other circumstance
#endif

// TODO:  Get rid of this whole header, a library upgrade is probably a better choice
// and would provide the exception_ptr type (and a number of other C++11 types) natively.

namespace std {

class exception_ptr {
public:
  exception_ptr(std::exception& except);
  ~exception_ptr(void);

  friend exception_ptr current_exception(void);
  friend void rethrow_exception(const exception_ptr& except);

private:
  std::exception& m_except;

public:
  // Non-standard extension, do not use
  void _RethrowException(void) const;
};

inline void rethrow_exception(const exception_ptr& except) {except._RethrowException();}
}

#endif
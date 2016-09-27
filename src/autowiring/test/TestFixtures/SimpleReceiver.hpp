// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/CoreThread.h>
#include <vector>

/// <summary>
/// Exception thrown when an r-value is used in an expression more than once, or in more than one expression
/// </summary>
class invalid_copycounter_exception:
  public autowiring_error
{
public:
  invalid_copycounter_exception(void) :
    autowiring_error("Copy counter was moved, and incorrectly reused")
  {}
};

/// <summary>
/// A very simple class which just counts the number of times it has been copied
/// </summary>
class CopyCounter  {
public:
  CopyCounter(void) :
    m_count(0)
  {}

  CopyCounter(const CopyCounter& rhs) :
    m_count(rhs.m_count + 1)
  {
    rhs.Check();
  }

  /// <summary>
  /// Special case move ctor, which won't increment the count
  /// </summary>
  CopyCounter(CopyCounter&& rhs) :
    m_count(rhs.m_count)
  {
    rhs.Check();
    rhs.m_count = -1;
  }

  /// <summary>
  /// Destructor, which invalidates the count in a specific way to indicate use-after-free
  /// </summary>
  ~CopyCounter(void) {
    m_count = -2;
  }

  // The actual count:
  int m_count;

  // Throws an exception if this copycounter is invalid
  void Check(void) const {
    if(m_count < 0)
      throw invalid_copycounter_exception();
  }

  // Copy and move overrides
  CopyCounter& operator=(const CopyCounter& rhs) {
    rhs.Check();
    m_count = rhs.m_count + 1;
    return *this;
  }

  CopyCounter& operator=(CopyCounter&& rhs) {
    rhs.Check();
    m_count = rhs.m_count;
    rhs.m_count = -1;
    return *this;
  }
};

/// <summary>
/// A simple class which allows copy attempts to be detected
/// </summary>
class NoCopyClass
{
public:
  NoCopyClass() :
    m_value(100)
  {}

  explicit NoCopyClass(NoCopyClass& rhs) :
    m_value(101)
  {}

  int m_value;
};

class CallableInterface
{
public:
  virtual void ZeroArgs(void) {}
  virtual void OneArg(int arg) {}
  virtual void CopyVectorForwarded(std::vector<int>&& vec) {}
  virtual void AllDone(void) {}

  virtual void NoCopyMethod(NoCopyClass& noCopy) {}
};

class CallableInterfaceDeferred
{
public:
  virtual void CopyVectorDeferred(const std::vector<int>& vec) = 0;
  virtual void ZeroArgsDeferred(void) = 0;
  virtual void OneArgDeferred(int arg) = 0;
  virtual void TrackCopy(CopyCounter&& ctr) = 0;
  virtual void AllDoneDeferred(void) = 0;
};

class SimpleReceiver:
  public CoreThread,
  public CallableInterface,
  public CallableInterfaceDeferred
{
public:
  SimpleReceiver(void) :
    CoreThread("SimpleReceiver"),
    m_barrierDone(false),
    m_zero(false),
    m_one(false),
    m_oneArg(0)
  {}

private:
  // Continuity signal:
  std::mutex m_lock;
  std::condition_variable m_continueCond;
  bool m_barrierDone;

public:
  // Manifest of functions called:
  bool m_zero;

  bool m_one;
  int m_oneArg;

  // Copy operation fields:
  std::vector<int> m_myVec;
  CopyCounter m_myCtr;

  ///
  // Interface utility methods:
  ///
  void ZeroArgs(void) override {
    m_zero = true;
  }

  void OneArg(int arg) override {
    m_one = true;
    m_oneArg = arg;
  }

  virtual void ZeroArgsDeferred(void) override {
    *this += [this] {
      m_zero = true;
    };
  }

  virtual void OneArgDeferred(int arg) override {
    *this += [this, arg] {
      m_one = true;
      m_oneArg = arg;
    };
  }

  void CopyVectorDeferred(const std::vector<int>& vec) override {
    // Copy out the argument:
    *this += [this, vec] {
      m_myVec = vec;
    };
  }

  void TrackCopy(CopyCounter&& ctr) override {
    m_myCtr = std::forward<CopyCounter>(ctr);
  }

  void CopyVectorForwarded(std::vector<int>&& vec) override {
    // Copy out the argument:
    m_myVec = vec;
  }

  void NoCopyMethod(NoCopyClass& noCopy) override {
  }

  // Trivial shutdown override
  void AllDone(void) override {
    Stop();
  }

  void AllDoneDeferred(void) override {
    *this += [this] {
      Stop();
    };
  }

  // Overridden here so we can hit the barrier if we're still waiting on it
  void OnStop(void) override {
    Proceed();
  }

  /// <summary>
  /// Invoked to cause Run to continue its processing
  /// </summary>
  void Proceed(void) {
    std::lock_guard<std::mutex> lk(m_lock);
    if(m_barrierDone)
      return;
    m_barrierDone = true;
    m_continueCond.notify_all();
  }

  ///
  // Runs the thread
  ///
  void Run(void) override {
    {
      std::unique_lock<std::mutex> lk(m_lock);
      m_continueCond.wait(lk, [this]() {return m_barrierDone; });
    }
    CoreThread::Run();
  }
};

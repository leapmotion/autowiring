// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include <thread>

using namespace autowiring;

namespace {
  class CountsCopies {
  public:
    CountsCopies(void) = default;
    CountsCopies(const CountsCopies&) { nCopies++; }
    CountsCopies(CountsCopies&& rhs) { nMoves++; }
    void operator=(const CountsCopies&) { nCopies++; }
    void operator=(CountsCopies&& rhs) { nMoves++; }

    static int nCopies;
    static int nMoves;
  };

  int CountsCopies::nCopies = 0;
  int CountsCopies::nMoves = 0;
}

class AutoSignalTest:
  public testing::Test
{
  void SetUp(void) override {
    CountsCopies::nCopies = 0;
    CountsCopies::nMoves = 0;
  }
};

class RaisesASignal {
public:
  autowiring::signal<void(int)> signal;
};

TEST_F(AutoSignalTest, SimpleSignalTest) {
  autowiring::signal<void(int)> signal;

  bool handler_called = false;
  int val = 0;

  // Register a handler directly on the signal:
  auto registration =
    signal += [&](int v) {
      handler_called = true;
      val = v;
    };

  // Trivially raise the signal:
  signal(101);
  ASSERT_TRUE(handler_called) << "Handler was not called on a stack-allocated signal";
  ASSERT_EQ(101, val) << "Handler did not correctly copy a passed value";

  // Unregister and verify we can still raise the signal:
  signal -= registration;

  handler_called = false;
  signal(102);
  ASSERT_FALSE(handler_called) << "Handler was called after being unregistered";
}

TEST_F(AutoSignalTest, SignalWithAutowiring) {
  bool handler_called = false;
  bool handler2_called = false;
  int val = 202;

  {
    Autowired<RaisesASignal> ras;

    // Register a signal handler:
    ras(&RaisesASignal::signal) += [&](int v) {
      handler_called = true;
      val = v;
    };

    // Inject type type after the signal has been registered
    AutoRequired<RaisesASignal>();

    ras(&RaisesASignal::signal) += [&](int v) {
      handler2_called = true;
    };

    // Now raise the signal, see what happens:
    ras->signal(55);

    // Verify that the handler got called with the correct value:
    ASSERT_TRUE(handler_called) << "Signal handler was not invoked";
    ASSERT_TRUE(handler2_called) << "Second Signal handler was not invoked";
    ASSERT_EQ(55, val) << "Signal handler not called with the correct parameter as expected";
  }

  // Raise the signal again, this should not cause anything to break:
  Autowired<RaisesASignal> ras;
  handler_called = false;
  ras->signal(99);
  ASSERT_FALSE(handler_called) << "A handler was unexpectedly called after it should have been destroyed";
}

struct RaisesASignalDerived:
  public RaisesASignal
{};

TEST_F(AutoSignalTest, SignalWithAutowiringDerived) {
  bool handler_called = false;
  bool wired = false;
  int val = 202;

  {
    Autowired<RaisesASignal> ras;

    // Register a signal handler on the base type
    ras(&RaisesASignal::signal) += [&](int v) {
      handler_called = true;
      val = v;
    };

    ras.NotifyWhenAutowired([&]() {
      wired = true;
    });

    // Inject derived type type after the signal has been registered
    AutoRequired<RaisesASignalDerived> derived;

    // Now raise the signal on the derived object and see what happens:
    derived->signal(55);

    // Verify that the handler got called with the correct value:
    ASSERT_TRUE(wired) << "Signal handler was not autowired";
    ASSERT_TRUE(handler_called) << "Signal handler was not invoked";
    ASSERT_EQ(55, val) << "Signal handler not called with the correct parameter as expected";
  }

  // Raise the signal again, this should not cause anything to break:
  Autowired<RaisesASignal> ras;
  handler_called = false;
  ras->signal(99);
  ASSERT_FALSE(handler_called) << "A handler was unexpectedly called after it should have been destroyed";
}

struct ContainsRaises {
  ContainsRaises(void) {
    ras(&RaisesASignal::signal) += [this](int v) {
      count++;
    };
  }

  ~ContainsRaises() {
    static int i = 0; 
    i++;
  }
  Autowired<RaisesASignal> ras;
  int count = 0;
};

TEST_F(AutoSignalTest, ConstructorAutowiredRegistration) {
  bool handler_called = false;
  int val = 202;

  {
    AutoCurrentContext rootContext;
    auto ctxt = rootContext->Create<void>();
    CurrentContextPusher pshr(ctxt);
    (void)pshr;
    {

      Autowired<RaisesASignalDerived> rasDerived;

      // Register a signal handler:
      rasDerived(&RaisesASignal::signal) += [&](int v) {
        handler_called = true;
        val = v;
      };

      AutoRequired<ContainsRaises> cRas;

      // Inject type type after the signal has been registered
      AutoRequired<RaisesASignalDerived>();

      // Now raise the signal, see what happens:
      rasDerived->signal(55);

      // Verify that the handler got called with the correct value:
      ASSERT_TRUE(handler_called) << "Signal handler was not invoked";
      ASSERT_TRUE(cRas->count > 0) << "Signal hander in object constructor was not invoked";
      ASSERT_EQ(55, val) << "Signal handler not called with the correct parameter as expected";
    }

    // Raise the signal again, this should not cause anything to break:
    Autowired<RaisesASignal> ras;
    handler_called = false;
    ras->signal(99);
    ASSERT_FALSE(handler_called) << "A handler was unexpectedly called after it should have been destroyed";
  }
}

TEST_F(AutoSignalTest, MultipleSlotsTest) {
  autowiring::signal<void(void)> signal;
  
  bool handler_called1 = false;
  bool handler_called2 = false;
  
  auto registration1 =
    signal += [&] {
      handler_called1 = true;
    };
  
  // Registration 2
  signal += [&] {
    handler_called2 = true;
  };
  
  // Trivially raise the signal:
  signal();
  ASSERT_TRUE(handler_called1) << "Handler 1 was not called on a stack-allocated signal";
  ASSERT_TRUE(handler_called2) << "Handler 2 was not called on a stack-allocated signal";
  
  // Unregister the first signal and reset the variables
  signal -= registration1;
  
  handler_called1 = false;
  handler_called2 = false;
  
  // Verify that registration 2 can still receive the signals
  signal();
  ASSERT_FALSE(handler_called1) << "Handler 1 was called after being unregistered";
  ASSERT_TRUE(handler_called2) << "Handler 2 was inadvertently unregistered";
}

TEST_F(AutoSignalTest, RaiseASignalWithinASlotTest) {
  autowiring::signal<void(void)> signal1;
  autowiring::signal<void(void)> signal2;
  
  bool handler_called1 = false;
  bool handler_called2 = false;
  
  signal1 += [&] {
    handler_called1 = true;
    signal2();
  };
  
  signal2 += [&] {
    handler_called2 = true;
  };
  
  // Trivially raise the signal:
  signal1();
  ASSERT_TRUE(handler_called1) << "Handler 1 was not called on a stack-allocated signal";
  ASSERT_TRUE(handler_called2) << "Handler 2 was not called on a stack-allocated signal";
}

TEST_F(AutoSignalTest, NodeRemoval) {
  autowiring::signal<void(void)> signal1;
  autowiring::signal<void(void)> signal2;

  bool handler_called1 = false;
  bool handler_called2 = false;

  auto registration1 = signal1 += [&] { handler_called1 = true; };
  auto registration2 = signal2 += [&] { handler_called2 = true; };
  
  ASSERT_ANY_THROW(signal1 -= registration2) << "Removing a registration from a different signal than it was registered to failed to throw an exception";

  signal1 -= registration1;

  signal1();
  signal2();
  
  ASSERT_FALSE(handler_called1) << "Handler1 was called after being unregistered";
  ASSERT_TRUE(handler_called2) << "Handler2 was removed after an invalid -= operation";
}

TEST_F(AutoSignalTest, CallOrdering) {
  autowiring::signal<void(void)> signal1;

  bool handler_called1 = false;
  bool handler_called2 = false;

  //handlers are inserted at the end, so this will be called first.
  signal1 += [&] { handler_called1 = true; };

  signal1 += [&] {
    ASSERT_TRUE(handler_called1) << "Handler1 was not called before handler2";
    handler_called2 = true;
  };

  signal1();

  ASSERT_TRUE(handler_called1) << "Handler1 was called after being unregistered";
  ASSERT_TRUE(handler_called2) << "Handler2 was removed after an invalid -= operation";
}

TEST_F(AutoSignalTest, SelfReferencingCall) {
  typedef autowiring::signal<void(int)> signal_t;
  signal_t signal1;

  bool handler_called1 = false;
  int magic_number = 123;

  //The main test is just if this thing will compile
  registration_t registration1 =
    signal1 += [&](registration_t reg, int magic) {
      ASSERT_EQ(magic, magic_number);
      ASSERT_EQ(registration1, reg);
      handler_called1 = true;
    };

  signal1(magic_number);

  ASSERT_TRUE(handler_called1) << "Handler was not called!";
}

TEST_F(AutoSignalTest, SelfModifyingCall) {
  typedef autowiring::signal<void(int)> signal_t;
  signal_t signal1;
  
  int handler_called1 = 0;
  int handler_called2 = 0;
  int handler_called3 = 0;
  
  int magic_number = 123;
  
  registration_t registration1 =
    signal1 += [&](registration_t reg, int magic) {
      ASSERT_EQ(magic, magic_number);
      ASSERT_EQ(registration1, reg);
      ++handler_called1;
      signal1 -= reg;
    };
  
  auto lambda3 = [&](int magic) {
    ++handler_called3;
  };

  registration_t registration2 =
    signal1 += [&](registration_t reg, int magic) {
      ASSERT_EQ(magic, magic_number);
      ASSERT_EQ(registration2, reg);
      ++handler_called2;
    
      //+= is an append operation, but because when we're traveling the list and we grab the next pointer
      //*before* the function get's called, this append won't be picked up until the 2nd pass.
      signal1 += std::move(lambda3);
      signal1 -= reg;
    };
  
  signal1(magic_number);
  signal1(magic_number);
  
  ASSERT_EQ(handler_called1, 1) << "Handler was unable to remove itself!";
  ASSERT_EQ(handler_called2, 1) << "Specific handler was unable to remove itself";
  ASSERT_EQ(handler_called3, 1) << "Handler was unable to append to itself or was called prematurely.";
}

struct VBase
{
  int someData;
};

struct Derived :
  public virtual VBase
{
  typedef autowiring::signal<void(void)> signal_t;
  signal_t signal1;
};

TEST_F(AutoSignalTest, ComplexInheritanceSignal) {
  int handlerCount = 0;
  {
    Autowired<Derived> derivedSlot;

    // Register a signal handler:
    derivedSlot(&Derived::signal1) += [&]{
      ++handlerCount;
    };

    // Inject type type after the signal has been registered
    AutoRequired<Derived>();

    // Now raise the signal, see what happens:
    derivedSlot->signal1();

    // Verify that the handler got called with the correct value:
    ASSERT_TRUE(handlerCount > 0) << "Signal handler was not invoked";
  }
}

TEST_F(AutoSignalTest, CanMoveSignal) {
  autowiring::signal<void()> signal;
  bool hit = false;

  {
    autowiring::signal<void()> innerSignal;
    innerSignal += [&hit] { hit = true; };

    autowiring::signal<void()> innerSignal2(std::move(innerSignal));
    innerSignal();
    ASSERT_FALSE(hit) << "A signal handler was hit on a signal that should have been moved";

    innerSignal2();
    ASSERT_TRUE(hit) << "Registered listeners were not correctly moved under move construction";
    hit = false;

    signal = std::move(innerSignal2);
    innerSignal2();
    ASSERT_FALSE(hit) << "A signal handler was hit on a signal that should have been moved";
  }

  signal();
  ASSERT_TRUE(hit) << "Registered listeners were not correctly moved under move assignment";
}

TEST_F(AutoSignalTest, OneRemovesAll) {
  autowiring::signal<void()> sig;
  std::vector<registration_t> regs;
  auto sentry = std::make_shared<bool>();

  int nRun = 0;

  {
    auto clearall = [&, sentry] {
      nRun++;
      for (auto& reg : regs)
        sig -= reg;
      regs.clear();
      ASSERT_EQ(3UL, sentry.use_count()) << "A registered lambda was destroyed before all lambdas were run";
    };

    static_assert(Decompose<decltype(&decltype(clearall)::operator())>::N == 0, "clearall lambda did not decompose correctly");

    regs.push_back(sig += clearall);
    regs.push_back(sig += clearall);
  }
  sig();
  ASSERT_TRUE(sentry.unique()) << "Some signal handlers were leaked on teardown";
  ASSERT_EQ(2, nRun) << "Should have executed all signals even if all signals were unlinked under a callback";
}

namespace {
  class ObjectA;
  class ObjectB;

  class ObjectA {
  public:
    ObjectA(void);

    autowiring::signal<void()> x;
    Autowired<ObjectB> b;
  };

  class ObjectB {
  public:
    ObjectB(void);

    autowiring::signal<void()> y;
    Autowired<ObjectA> a;
  };

  ObjectA::ObjectA(void) {
    b(&ObjectB::y) += [] {};
  }

  ObjectB::ObjectB(void) {
    a(&ObjectA::x) += [] {};
  }
}

TEST_F(AutoSignalTest, CyclicRegistrationUnlink) {
  std::weak_ptr<CoreContext> ctxt;
  {
    AutoCreateContext ctxt;
    ctxt->SetUnlinkOnTeardown(true);

    AutoRequired<ObjectA> a(ctxt);
    AutoRequired<ObjectB> b(ctxt);
  }

  ASSERT_TRUE(ctxt.expired()) << "A context pointer was linked in an intentional cyclic dependency network";
}

TEST_F(AutoSignalTest, TrivialCountCase) {
  autowiring::signal<void(const CountsCopies&)> sig;
  sig(CountsCopies{});
  ASSERT_EQ(0UL, CountsCopies::nCopies) << "Copy made under trivial forwarding";
}

TEST_F(AutoSignalTest, TrivialMoveCountCase) {
  autowiring::signal<void(const std::unique_ptr<int>&)> sig;

  // We have this line just to ensure it compiles
  sig(std::unique_ptr<int>{ new int });

  std::unique_ptr<int> val{ new int };
  sig(std::move(val));
  ASSERT_NE(nullptr, val) << "Value was moved when it should have been possible to operate on it in-place";
}

TEST_F(AutoSignalTest, TotalMoveCount) {
  autowiring::signal<void(const CountsCopies&)> sig;

  bool flag = true;
  sig += [&] (const CountsCopies&) {
    if(flag) {
      // This recursive call will require that CountsCopies be moved at least once
      sig(CountsCopies{});
      flag = false;
    }
  };

  // This should not require that the move ctor is invoked at all
  sig(CountsCopies{});
  ASSERT_EQ(1UL, CountsCopies::nMoves) << "Too many move constructions were required to invoke the signal";
}

TEST_F(AutoSignalTest, NecessaryMoveCase) {
  autowiring::signal<void(const std::unique_ptr<int>&)> sig;

  std::unique_ptr<int> mustBeMoved{ new int };
  int callCt = 0;
  sig += [&] (const std::unique_ptr<int>& cc) {
    if (callCt == 0)
      sig(std::move(mustBeMoved));
    callCt++;
  };

  std::unique_ptr<int> val{ new int };
  sig(std::move(val));
  ASSERT_NE(nullptr, val) << "Value was moved when it should not have been moved";
  ASSERT_EQ(nullptr, mustBeMoved) << "Value was not moved when it should have been moved";
}

TEST_F(AutoSignalTest, NestedSignalRegistration) {
  autowiring::signal<void()> sig;

  bool outer = false;
  bool inner = false;
  sig += [&] {
    outer = true;

    // This registration will occur too late for the handler to be called, because at the time
    // of registration, the signal was already asserted.
    sig += [&] {
      inner = true;
    };
  };
  sig();
  ASSERT_TRUE(outer) << "Outer signal was not asserted even though it was registered in advance";
  ASSERT_FALSE(inner) << "Inner signal was asserted even though it should not yet have been registered";
}

TEST_F(AutoSignalTest, NoLingeringListeners) {
  autowiring::signal<void()> sig;
  auto x = std::make_shared<bool>(false);

  auto reg = sig += [x] {};
  sig += [&] { sig -= reg; };

  // Invoke the signal, which will defer unregistration of the other listener and should
  // not cause leakage.
  sig();
  ASSERT_TRUE(x.unique()) << "Lambda function unregistration was leaked";
}

TEST_F(AutoSignalTest, PathologicalSyncTest) {
  autowiring::signal<void()> sig;
  bool proceed = true;

  // This thread slams the signal in a tight loop
  std::thread t([&] {
    while (proceed)
      sig();
  });

  // Make sure our thread exits properly
  auto r = MakeAtExit([&] {
    proceed = false;
    t.join();
  });

  // Need to make sure that the signal is only ever asserted from the thread above:
  auto tid = t.get_id();
  bool succeeded = false;
  size_t nAssertions = 0;
  sig += [&, tid] {
    succeeded &= (tid == std::this_thread::get_id());
    nAssertions++;
  };

  // Register and unregister the same listener in a tight loop, this should create chaos
  while (nAssertions < 1000) {
    auto x = std::make_shared<bool>(false);
    auto r = sig += [x] {
      *x = x.unique();
    };
    sig -= r;
    ASSERT_FALSE(*x) << "Lambda was invoked after it was destroyed";
  }
}

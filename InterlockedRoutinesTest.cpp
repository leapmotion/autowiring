#include "stdafx.h"
#include "InterlockedRoutinesTest.h"
#include "InterlockedExchange.h"
#include <thread>
#include <boost/thread/barrier.hpp>

template<void* (*fn)(void**, void*)>
void CheckFn() {
  int spot;

  // Trivial exchange check:
  void* dest = &spot;
  void* exchangeVal = nullptr;
  void* pExchanged = exchange_acquire(&dest, exchangeVal);

  // Verify all known states:
  EXPECT_EQ(&spot, pExchanged) << "Return value of exchange_acquire as incorrect";
  EXPECT_EQ(nullptr, dest) << "Exchange destination was not properly assigned";
}

TEST_F(InterlockedRoutinesTest, VerifyExchangeAcquire) {
  CheckFn<&exchange_acquire>();
}

TEST_F(InterlockedRoutinesTest, VerifyExchangeRelease) {
  CheckFn<&exchange_release>();
}

TEST_F(InterlockedRoutinesTest, VerifyCompareExchange) {
  int spot1;
  int spot2;
  int spot3;

  void* dest = &spot1;
  void* pExchanged;

  // A compare-exchange that is known to fail:
  pExchanged = compare_exchange(&dest, &spot2, &spot3);
  EXPECT_EQ(&spot1, dest) << "Exchanged values even though the comparand was not equal";
  EXPECT_EQ(&spot1, pExchanged) << "Returned exchange value was not the value of dest";

  // Reset:
  dest = &spot1;

  // A compare-exchange that should succeed:
  pExchanged = compare_exchange(&dest, &spot2, &spot1);
  EXPECT_EQ(&spot2, dest) << "Destination was not given a correct value";
  EXPECT_EQ(&spot1, pExchanged) << "Return value under a successful exchange was not the original value";
}

TEST_F(InterlockedRoutinesTest, VerifyCompareExchangePathological) {
  static const size_t threadCount = 100;

  // State-checking value.  The value of this address isn't actually used, it's only here
  // to give us a convenient address we can start from.
  char base = 100;

  // A counter, implemented as a pointer, whose value we will monotonically increase.
  void* counter = &base;

  // Barrier, in order to exacerbate race conditions:
  boost::barrier bar(threadCount);

  auto contender = [&counter, &bar] () {
    void* presumption;
    void* addend;

    bar.wait();

    do {
      // This is an implementation of interlocked_add that is based on compare-exchange.
      // It works by peeking the current value of the counter, computing the new value,
      // and then swapping the newly computed value if and only if the original value is
      // unchanged.
      presumption = counter;
      addend = (char*)presumption + 1;

      // Transition allowable only if the addend's value hasn't changed since we computed it
    } while(compare_exchange(&counter, addend, presumption) != presumption);
  };

  std::thread allThreads[threadCount];
  for(size_t i = 0; i < threadCount; i++) {
    allThreads[i] = std::thread(contender);
  }

  // Wait for all threads to complete:
  for(size_t i = 0; i < threadCount; i++)
    allThreads[i].join();

  // Verify that no illegal transitions have taken place by ensuring the count is precisely
  // the count we expect:
  size_t offset = (char*)counter - &base;
  EXPECT_EQ(offset, threadCount) << "Interlocked exchange under heavy contention failed to honor interlocking requirements";
}
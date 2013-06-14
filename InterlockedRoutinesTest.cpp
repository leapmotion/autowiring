#include "stdafx.h"
#include "InterlockedRoutinesTest.h"
#include "InterlockedExchange.h"

template<void* (*fn)(void*volatile*, void*)>
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


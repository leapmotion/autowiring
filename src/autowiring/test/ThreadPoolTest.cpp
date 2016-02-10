// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include <autowiring/ManualThreadPool.h>
#include <autowiring/NullPool.h>
#include <autowiring/SystemThreadPoolStl.h>
#include FUTURE_HEADER

#ifdef _MSC_VER
#include "SystemThreadPoolWinXP.hpp"
#include "SystemThreadPoolWinLH.hpp"
#endif

template<typename T>
class ThreadPoolTest:
  public testing::Test
{
public:
  ThreadPoolTest(void) {
    m_pool->SuggestThreadPoolSize(2);
    token = m_pool->Start();
  }

  void TearDown(void) {
    token.reset();
  }

  std::shared_ptr<T> m_pool = std::make_shared<T>();
  std::shared_ptr<void> token;
};

TYPED_TEST_CASE_P(ThreadPoolTest);

TYPED_TEST_P(ThreadPoolTest, PoolOverload) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  size_t cap = 1000;
  auto ctr = std::make_shared<std::atomic<size_t>>(cap);
  auto p = std::make_shared<std::promise<void>>();

  for (size_t i = cap; i--;)
    *this->m_pool += [=] {
      if (!--*ctr)
        p->set_value();
    };

  auto rs = p->get_future();
  ASSERT_EQ(std::future_status::ready, rs.wait_for(std::chrono::seconds(5))) << "Pool saturation did not complete in a timely fashion";
}

REGISTER_TYPED_TEST_CASE_P(ThreadPoolTest, PoolOverload);

typedef ::testing::Types<
#ifdef _MSC_VER
  // These pool types are Windows-only
  autowiring::SystemThreadPoolWinXP,
  autowiring::SystemThreadPoolWinLH,
#endif

  // All platforms test the STL thread pool
  autowiring::SystemThreadPoolStl
> t_testTypes;

INSTANTIATE_TYPED_TEST_CASE_P(My, ThreadPoolTest, t_testTypes);

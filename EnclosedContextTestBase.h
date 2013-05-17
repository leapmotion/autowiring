#ifndef _ENCLOSED_CONTEXT_TEST_BASE_H
#define _ENCLOSED_CONTEXT_TEST_BASE_H
#include "Autowired.h"
#include <gtest/gtest.h>
#include <memory>

class EnclosedContextTestBase:
  public testing::Test
{
public:
  virtual void SetUp(void) override;
  virtual void TearDown(void) override;

private:
  // The context proper.  This is automatically assigned as the current
  // context when SetUp is invoked.
  std::shared_ptr<CoreContext> m_create;
};

#endif
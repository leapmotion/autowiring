#include "stdafx.h"

class AutoConstructTest:
  public testing::Test
{};

class HasDefaultCtorAndOthers {
public:
  HasDefaultCtorAndOthers(void) :
    v(101)
  {}
  HasDefaultCtorAndOthers(int v) :
    v(v)
  {}

  const int v;
};

TEST_F(AutoConstructTest, AutoConstructNoArgs) {
  AutoConstruct<HasDefaultCtorAndOthers> hdcao;
  ASSERT_EQ(101, hdcao->v) << "Default constructor was not called as expected";
}

TEST_F(AutoConstructTest, AutoConstructWithArgs) {
  AutoConstruct<HasDefaultCtorAndOthers> hdcao(495);
  ASSERT_EQ(495, hdcao->v) << "Constructor call was not made as expected";
}
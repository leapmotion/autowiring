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

class CanOnlyAcceptMovedInput {
public:
  CanOnlyAcceptMovedInput(std::unique_ptr<std::shared_ptr<int>>&& ptr) :
    m_ptr(std::move(ptr))
  {}

  std::unique_ptr<std::shared_ptr<int>> m_ptr;
};

TEST_F(AutoConstructTest, CanConstructRvalueCtor) {
  auto originalPtr = std::make_shared<int>(555);

  // Make a unique pointer to a shared pointer, and pass it in:
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    std::unique_ptr<std::shared_ptr<int>> forwarded{new std::shared_ptr<int>(originalPtr)};
    AutoConstruct<CanOnlyAcceptMovedInput> coami(std::move(forwarded));

    // Should have the correct number of references, no more and no less
    ASSERT_EQ(2UL, originalPtr.use_count()) << "Forwarding unique pointer did not correctly";
  }

  ASSERT_TRUE(originalPtr.unique()) << "Memory leak detected due to incorrect forwarding of a unique pointer";
}

struct ClassWithAutoInit:
  std::enable_shared_from_this<ClassWithAutoInit>
{
  ClassWithAutoInit(void) :
    m_constructed(true),
    m_postConstructed(false)
  {}

  void AutoInit(void) {
    ASSERT_TRUE(m_constructed) << "A postconstruct routine was called BEFORE the corresponding constructor";
    m_postConstructed = true;

    auto myself = shared_from_this();
    ASSERT_EQ(this, myself.get()) << "Reflexive shared_from_this did not return a shared pointer to this as expected";
  }

  bool m_constructed;
  bool m_postConstructed;
};

static_assert(has_autoinit<ClassWithAutoInit>::value, "AutoInit-bearing class did not pass a static type check");

TEST_F(AutoConstructTest, PostConstructGetsCalled) {
  AutoRequired<ClassWithAutoInit> cwai;
  ASSERT_TRUE(cwai->m_constructed) << "Trivial constructor call was not made as expected";
  ASSERT_TRUE(cwai->m_postConstructed) << "Auto-initialization routine was not called on an initializable type";
}
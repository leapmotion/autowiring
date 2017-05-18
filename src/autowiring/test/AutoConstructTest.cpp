// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"

class AutoConstructTest:
  public testing::Test
{};

namespace {
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
}

static_assert(
  autowiring::select_strategy<HasDefaultCtorAndOthers, int>::value == autowiring::construction_strategy::standard,
  "Construction strategy incorrectly inferred"
  );

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

TEST_F(AutoConstructTest, CanCopyAutoConstruct) {
  AutoConstruct<HasDefaultCtorAndOthers> v(100);
}

namespace {
  class MyPrivateCtorClass:
    public CoreObject
  {
    MyPrivateCtorClass(void):
      ival(-10)
    {}
    MyPrivateCtorClass(int ival) :
      ival(ival)
    {}

  public:
    const int ival;

    static MyPrivateCtorClass* New(int ival) {
      return new MyPrivateCtorClass{ ival };
    }
  };
}

static_assert(
  autowiring::has_static_new<MyPrivateCtorClass, int>::value,
  "Failed to find factory new on a type that carries it"
);
static_assert(
  autowiring::select_strategy<MyPrivateCtorClass, int>::value == autowiring::construction_strategy::factory_new,
  "Construction strategy incorrectly inferred"
);
static_assert(
  !std::is_constructible<MyPrivateCtorClass>::value,
  "Type reported as being constructable when it was not"
);
static_assert(
  !autowiring::has_simple_constructor<MyPrivateCtorClass>::value,
  "Simple constructor detected when said constructor should have been private"
);

TEST_F(AutoConstructTest, FactoryNewPrivateCtor) {
  AutoConstruct<MyPrivateCtorClass> mpcc{ 1002 };
  ASSERT_NE(nullptr, mpcc.get()) << "Null not expected as a return type from a factory new construction";
  ASSERT_EQ(1002, mpcc->ival) << "Correct ctor was not invoked on a type with a private ctor";
}

namespace {
  class MyPrivateCtorStringClass :
    public CoreObject
  {
    MyPrivateCtorStringClass(void) :
      istr("default_string")
    {}
    MyPrivateCtorStringClass(const char* istr) :
      istr(istr)
    {}

  public:
    const char* istr;

    static MyPrivateCtorStringClass* New(const char* str) {
      return new MyPrivateCtorStringClass{ str };
    }
  };
}

static_assert(
  autowiring::has_static_new<MyPrivateCtorStringClass, decltype("")>::value,
  "Failed to find factory new on a type that carries it"
  );
static_assert(
  autowiring::select_strategy<MyPrivateCtorStringClass, decltype("")>::value == autowiring::construction_strategy::factory_new,
  "Construction strategy incorrectly inferred"
  );
static_assert(
  !std::is_constructible<MyPrivateCtorStringClass>::value,
  "Type reported as being constructable when it was not"
  );
static_assert(
  !autowiring::has_simple_constructor<MyPrivateCtorStringClass>::value,
  "Simple constructor detected when said constructor should have been private"
  );

TEST_F(AutoConstructTest, FactoryNewPrivateStringCtor) {
  AutoConstruct<MyPrivateCtorStringClass> mpcc{ "a new string" };
  ASSERT_NE(nullptr, mpcc.get()) << "Null not expected as a return type from a factory new construction";
  ASSERT_STREQ("a new string", mpcc->istr) << "Correct ctor was not invoked on a type with a private ctor";
}

class MultiStaticNew : public CoreObject {
  MultiStaticNew() {}
public:
  static char* New() { return nullptr; }
  static MultiStaticNew* New(int x) { return nullptr; }
};
static_assert(
  !autowiring::has_static_new<MultiStaticNew>::value,
  "Found a bad static new"
  );
static_assert(
  autowiring::has_static_new<MultiStaticNew, int>::value,
  "Failed to find a good static new override"
  );

TEST_F(AutoConstructTest, MultiStaticNewTest) {
  AutoConstruct<MultiStaticNew> badstatic{ 1 };
}
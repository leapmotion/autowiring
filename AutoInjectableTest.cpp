#include "stdafx.h"
#include "AutoInjectableTest.h"
#include "AutoInjectable.h"
#include "TestFixtures/SimpleObject.h"

TEST_F(AutoInjectableTest, VerifySimpleInjection) {
  auto injector = MakeInjectable<SimpleObject>();
  injector();

  Autowired<SimpleObject> myobj;
  ASSERT_TRUE(myobj.IsAutowired()) << "Injectable failed to introduce a zero-arguments constructed item into the context";
}

class StealsConstructorArgument {
public:
  StealsConstructorArgument(std::string&& myVal) :
    myVal(std::forward<std::string>(myVal))
  {}

  std::string myVal;
};

TEST_F(AutoInjectableTest, VerifyConstructedInjection) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello World!");
  injector();

  Autowired<StealsConstructorArgument> myobj;
  ASSERT_TRUE(myobj.IsAutowired()) << "Injectable failed to introduce a single-arguments constructed item into the context";
  ASSERT_EQ("Hello World!", myobj->myVal) << "Injectable failed to copy an rvalue argument properly";
}

TEST_F(AutoInjectableTest, VerifyReduplicatedInjection) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello") + MakeInjectable<StealsConstructorArgument>("World");
  injector();

  Autowired<StealsConstructorArgument> myobj;
  ASSERT_TRUE(myobj.IsAutowired()) << "Injectable failed to introduce a single-arguments constructed item into the context";
  ASSERT_EQ("Hello", myobj->myVal) << "Injectable failed to copy an rvalue argument properly";
}

TEST_F(AutoInjectableTest, VerifyCombinedInjection) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello") + MakeInjectable<SimpleObject>();
  injector();

  Autowired<StealsConstructorArgument> myobj;
  ASSERT_TRUE(myobj.IsAutowired()) << "Combined injectable failed to introduce a single-arguments constructed item into the context";
  ASSERT_EQ("Hello", myobj->myVal) << "Combined injectable failed to copy an rvalue argument properly";
  ASSERT_TRUE(myobj.IsAutowired()) << "Combined injectable failed to introduce a zero-arguments constructed item into the context";
}
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

TEST_F(AutoInjectableTest, VerifyNullInjection) {
  AutoCurrentContext ctxt;
  size_t currentCount = ctxt->GetMemberCount();

  auto injector = MakeInjectable<void>();
  injector();

  ASSERT_EQ(currentCount, ctxt->GetMemberCount()) << "A void injector incorrectly added a context member";
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
  Autowired<SimpleObject> mySimpleObj;
  ASSERT_TRUE(myobj.IsAutowired()) << "Combined injectable failed to introduce a single-arguments constructed item into the context";
  ASSERT_EQ("Hello", myobj->myVal) << "Combined injectable failed to copy an rvalue argument properly";
  ASSERT_TRUE(mySimpleObj.IsAutowired()) << "Combined injectable failed to introduce a zero-arguments constructed item into the context";
}

TEST_F(AutoInjectableTest, VerifyInjectableAdditionPermutation1) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello");
  auto injector2 = injector + MakeInjectable<SimpleObject>();
  injector2();

  Autowired<StealsConstructorArgument> myStealObj;
  Autowired<SimpleObject> mySimpleObj;
  ASSERT_TRUE(myStealObj.IsAutowired()) << "Combined injectable failed to introduce a single-argument constructed item";
  ASSERT_EQ("Hello", myStealObj->myVal) << "Combined injectable failed to copy an rvalue argument";
  ASSERT_TRUE(mySimpleObj.IsAutowired()) << "Combined injectable failed to introduce a zero-arguments constructed";
}


TEST_F(AutoInjectableTest, VerifyInjectableAdditionPermutation2) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello");
  auto injector2 = MakeInjectable<SimpleObject>() + injector;
  injector2();

  Autowired<StealsConstructorArgument> myStealObj;
  Autowired<SimpleObject> mySimpleObj;
  ASSERT_TRUE(myStealObj.IsAutowired()) << "Combined injectable failed to introduce a single-argument constructed item";
  ASSERT_EQ("Hello", myStealObj->myVal) << "Combined injectable failed to copy an rvalue argument";
  ASSERT_TRUE(mySimpleObj.IsAutowired()) << "Combined injectable failed to introduce a zero-arguments constructed";
}

TEST_F(AutoInjectableTest, VerifyInjectableAdditionPermutation3) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello");
  auto injector2 = MakeInjectable<SimpleObject>();
  auto injector3 = injector + injector2;
  injector3();

  Autowired<StealsConstructorArgument> myStealObj;
  Autowired<SimpleObject> mySimpleObj;
  ASSERT_TRUE(myStealObj.IsAutowired()) << "Combined injectable failed to introduce a single-argument constructed item";
  ASSERT_EQ("Hello", myStealObj->myVal) << "Combined injectable failed to copy an rvalue argument";
  ASSERT_TRUE(mySimpleObj.IsAutowired()) << "Combined injectable failed to introduce a zero-arguments constructed";
}

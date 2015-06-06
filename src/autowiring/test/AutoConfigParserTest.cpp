// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <src/autowiring/AutoConfigParser.hpp>
#include <AutoConfig.h>

class AutoConfigParserTest:
  public testing::Test
{};

TEST_F(AutoConfigParserTest, ExtractKeyTestWin) {
  std::string type1("struct ConfigTypeExtractor<struct Namespace1, struct XYZ>");
  const auto key1 = autowiring::ExtractKey(type1);
  ASSERT_EQ("Namespace1.XYZ", key1) << "Failed to properly extract Key with a single namespace";

  std::string type2("struct ConfigTypeExtractor<struct Namespace1, struct Namespace2, struct XYZ>");
  const auto key2 = autowiring::ExtractKey(type2);
  ASSERT_EQ("Namespace1.Namespace2.XYZ", key2) << "Failed to properly extract Key with multiple namespaces";

  std::string type3("struct ConfigTypeExtractor<struct XYZ>");
  const auto key3 = autowiring::ExtractKey(type3);
  ASSERT_EQ("XYZ", key3) << "Failed to properly extract Key with no namespace";

  std::string type4("struct ConfigTypeExtractor<class ClassNamespace1, class AutoConfigTest, struct XYZ>");
  const auto key4 = autowiring::ExtractKey(type4);
  ASSERT_EQ("ClassNamespace1.AutoConfigTest.XYZ", key4) << "Failed to properly extract Key with class namespaces";

  std::string type5("struct ConfigTypeExtractor<class ClassNamespace1, class Base::Nested, struct XYZ>");
  const auto key5 = autowiring::ExtractKey(type5);
  ASSERT_EQ("ClassNamespace1.Base::Nested.XYZ", key5) << "Failed to properly extract Key from nested";
}

TEST_F(AutoConfigParserTest, ExtractKeyTestPOSIX) {
  std::string type1("ConfigTypeExtractor<Namespace1, XYZ>");
  const auto key1 = autowiring::ExtractKey(type1);
  ASSERT_EQ("Namespace1.XYZ", key1) << "Failed to properly extract Key with a single namespace";

  std::string type2("ConfigTypeExtractor<Namespace1, Namespace2, XYZ>");
  const auto key2 = autowiring::ExtractKey(type2);
  ASSERT_EQ("Namespace1.Namespace2.XYZ", key2) << "Failed to properly extract Key with multiple namespaces";

  std::string type3("ConfigTypeExtractor<XYZ>");
  const auto key3 = autowiring::ExtractKey(type3);
  ASSERT_EQ("XYZ", key3) << "Failed to properly extract Key with no namespace";

  std::string type4("ConfigTypeExtractor<ClassNamespace1, AutoConfigTest, XYZ>");
  const auto key4 = autowiring::ExtractKey(type4);
  ASSERT_EQ("ClassNamespace1.AutoConfigTest.XYZ", key4) << "Failed to properly extract Key with class namespaces";
  
  std::string type5("ConfigTypeExtractor<ClassNamespace1, Base::Nested, XYZ>");
  const auto key5 = autowiring::ExtractKey(type5);
  ASSERT_EQ("ClassNamespace1.Base::Nested.XYZ", key5) << "Failed to properly extract Key from nested";
}

struct Namespace1 {};
struct Namespace2 {};
struct XYZ {};

struct MyConfigurableClass {
  AutoConfig<int, Namespace1, XYZ> m_myName;
};

TEST_F(AutoConfigParserTest, VerifyCorrectDeconstruction) {
  AutoRequired<MyConfigurableClass> mcc;

  ASSERT_STREQ("Namespace1.XYZ", mcc->m_myName->m_key.c_str())
    << "Configuration variable name was not correctly extracted";
}

// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/config_descriptor.h>
#include <autowiring/config.h>
#include <autowiring/ConfigRegistry.h>
#include <autowiring/observable.h>
#include <cstring>
#include <cstdlib>

namespace aw = autowiring;

class AutoConfigTest:
  public testing::Test
{};

namespace {
  class multi_meta {
  public:
    static const bool is_multi = true;

    std::string name;

    template<typename T>
    void bind(const aw::config_field& field, T&) {
      name = field.name;
    }
  };

  static_assert(aw::valid<int, multi_meta>::value, "Float metadata field not detected as being valid");

  class single_meta {
  public:
    single_meta(const char* name) :
      name(name)
    {}

    const char* name;

    // The default is for configuration metadata to be treated as a "single" entry.  This means that the
    // entry cannot be specified more than once in a configuration descriptor.  It is not necessary for
    // users to specify is_multi in this case.
    //static const bool is_multi = false;
  };

  class MyConfigurableClass {
  public:
    autowiring::config<std::string> a{ "Hello world!" };
    std::atomic<int> b{ 929 };
    std::atomic<uint64_t> bUnsigned{ 92999 };
    volatile bool c = false;
    volatile float d = 1.0f;
    volatile double e = 99.2;
    autowiring::observable<int> obs{ 44 };
    autowiring::config<int> cfg{ 929999 };

    static autowiring::config_descriptor GetConfigDescriptor(void) {
      return {
        { "a", "Field A description", &MyConfigurableClass::a, aw::default_value<std::string>("Hello world!") },
        { "b", "Field B description", &MyConfigurableClass::b, aw::default_value(929), aw::bounds<int>{ 0, 1000 }, multi_meta{}, multi_meta{} },
        { "bUnsigned", "Description", &MyConfigurableClass::bUnsigned, aw::default_value(4444), single_meta{"Hello world!"} },
        { "c", &MyConfigurableClass::c },
        { "d", &MyConfigurableClass::d },
        { "e", &MyConfigurableClass::e },
        { "obs", &MyConfigurableClass::obs },
        { "cfg", &MyConfigurableClass::cfg }
      };
    }
  };

  class DuplicateClass {
  public:
    autowiring::config<std::string> a { "Hello world" };

    static autowiring::config_descriptor GetConfigDescriptor(void) {
      return{
        { "a", "Field A description 2", &DuplicateClass::a, aw::default_value<std::string>("Hello universe!")}
      };
    }
  };

  class BadClass {
  public:
    autowiring::config_descriptor GetConfigDescriptor(void) { return{}; }
  };
}

static_assert(!aw::is_multi<aw::bounds<int>>::value, "Bounds should not have been marked as a multi-select type");
static_assert(!aw::is_multi<single_meta>::value, "Single metadata type incorrectly detected as supporting multiple instantiations");
static_assert(aw::is_multi<multi_meta>::value, "Multi metadata type not correctly detecting as supporting multiple instantiations");
static_assert(aw::has_getconfigdescriptor<MyConfigurableClass>::value, "Static new not correctly detected");
static_assert(!aw::has_getconfigdescriptor<BadClass>::value, "Bad class cannot have a configuration descriptor");
static_assert(!aw::has_bind<aw::bounds<int>, int>::value, "Bind incorrectly detected on bounds field");
static_assert(aw::has_bind<multi_meta, aw::config_field, int>::value, "Contextual bind not detected on multimeta type");

TEST_F(AutoConfigTest, ConfigFieldAssign) {
  autowiring::config<std::string> x{ "Hello world!" };
  ASSERT_TRUE(x.is_dirty()) << "Config values are assumed to be initially dirty";
}

TEST_F(AutoConfigTest, ConfigFieldSetBad) {
  MyConfigurableClass c;

  std::string expected{ "There is no config" };
  ASSERT_ANY_THROW(autowiring::ConfigSet("z", c, expected.c_str())) << "Tried to set an invalid config key and did not fail.";
}

TEST_F(AutoConfigTest, ConfigDefault) {
  // Default value will be whatever bUnsigned is assigned to in the inline initializer.  This is
  // because MyConfigurableClass is not actually in a context, which means Autowiring will not
  // attempt to configure it.
  MyConfigurableClass c;
  ASSERT_EQ(92999, static_cast<uint64_t>(c.bUnsigned));
}

TEST_F(AutoConfigTest, String) {
  MyConfigurableClass c;

  std::string expected{ "Forescore and seven years ago" };
  autowiring::ConfigSet("a", c, expected.c_str());
  ASSERT_TRUE(c.a.clear_dirty());
  ASSERT_STREQ(expected.c_str(), c.a->c_str()) << "String configuration value not assigned";
}

TEST_F(AutoConfigTest, Integer) {
  MyConfigurableClass c;

  autowiring::ConfigSet("b", c, "999");
  ASSERT_EQ(c.b, 999) << "Integer configuration value not assigned";

  autowiring::ConfigSet("b", c, "-999");
  ASSERT_EQ(c.b, -999) << "Negative integer configuration value not assigned";
}

TEST_F(AutoConfigTest, IntegerUnsigned) {
  MyConfigurableClass c;

  c.bUnsigned = 10929;
  std::string strVal = autowiring::ConfigGet("bUnsigned", c);
  ASSERT_STREQ("10929", strVal.c_str());

  autowiring::ConfigSet("bUnsigned", c, "999");
  ASSERT_EQ(static_cast<uint64_t>(c.bUnsigned), 999) << "Integer configuration value not assigned";
  ASSERT_ANY_THROW(autowiring::ConfigSet("bUnsigned", c, "-999")) << "Incorrectly assigned as signed value to an unsigned field";
}

TEST_F(AutoConfigTest, Bool) {
  MyConfigurableClass c;

  autowiring::ConfigSet("c", c, "true");
  ASSERT_TRUE(c.c) << "Boolean configuration value not assigned";
}

TEST_F(AutoConfigTest, Float) {
  MyConfigurableClass c;

  c.d = 10929.4f;
  std::string strVal = autowiring::ConfigGet("d", c);
  const float fVal = strtof(strVal.c_str(), nullptr);
  ASSERT_EQ(c.d, fVal) << "Floating point config value didn't successfully round trip";

  autowiring::ConfigSet("d", c, "123.4");
  ASSERT_FLOAT_EQ(c.d, 123.4f) << "Float configuration value not assigned";
}

TEST_F(AutoConfigTest, Double) {
  MyConfigurableClass c;
  std::string strVal;

  c.e = 10929.4423;
  strVal = autowiring::ConfigGet("e", c);
  double outVal = strtod(strVal.c_str(), nullptr);
  ASSERT_EQ(c.e, outVal) << "Double config value failed to round trip";

  c.e = 109290000000000;
  strVal = autowiring::ConfigGet("e", c);
  ASSERT_STREQ("109290000000000", strVal.c_str());

  c.e = -0.0099291;
  strVal = autowiring::ConfigGet("e", c);
  outVal = strtod(strVal.c_str(), nullptr);
  ASSERT_EQ(c.e, outVal) << "Double config value failed to round trip with a negative number";

  autowiring::ConfigSet("e", c, "77482.4");
  ASSERT_DOUBLE_EQ(c.e, 77482.4) << "Double configuration value not assigned";
}

TEST_F(AutoConfigTest, Observable) {
  MyConfigurableClass c;

  bool hit = false;
  c.obs.onChanged += [&hit] { hit = true; };
  autowiring::ConfigSet("obs", c, "101");
  ASSERT_EQ(101, c.obs) << "Value not properly assigned";
  ASSERT_TRUE(hit) << "Observable signal not asserted";
}

TEST_F(AutoConfigTest, Configurable) {
  AutoCurrentContext ctxt;
  AutoRequired<MyConfigurableClass> c;

  ASSERT_TRUE(c->cfg.clear_dirty()) << "Dirty flag was not set on a value that should have been dirty";
  ASSERT_FALSE(c->cfg.is_dirty());
  ctxt->Config.Set("cfg", "888");
  ASSERT_TRUE(c->cfg.is_dirty()) << "Dirty flag not set correctly after being updated in configuration";
  ASSERT_TRUE(c->cfg.clear_dirty());
  ASSERT_EQ(888, c->cfg);
}

TEST_F(AutoConfigTest, ContextSetSimple) {
  AutoCurrentContext ctxt;
  AutoRequired<MyConfigurableClass> mcc;

  ctxt->Config.Set("b", "1029");
  ASSERT_EQ(mcc->b, 1029);
}

TEST_F(AutoConfigTest, ContextSetAfter) {
  AutoCurrentContext ctxt;
  ctxt->Config.Set("b", "10442");

  AutoRequired<MyConfigurableClass> mcc;
  ASSERT_EQ(mcc->b, 10442);
}

TEST_F(AutoConfigTest, ContextGetSimple) {
  AutoCurrentContext ctxt;
  AutoRequired<MyConfigurableClass> mcc;

  mcc->b = 10442;
  ASSERT_EQ("929", ctxt->Config.Get("b")) << "Non-Observable value had an unexpected value.";

  ctxt->Config.Set("b", "10443");
  ASSERT_EQ(10443, mcc->b) << "Non-Observable value set in the context not modified in the backing value.";
}

TEST_F(AutoConfigTest, ContextMultiReference) {
  AutoCurrentContext ctxt;
  AutoRequired<MyConfigurableClass> mcc;
  AutoRequired<DuplicateClass> dc;

  const auto expectStr1 = "Hello Multiverse";
  mcc->a.force_assign(expectStr1);
  ASSERT_STREQ("Hello world", dc->a->c_str());
  ASSERT_STREQ("Hello world!", ctxt->Config.Get("a").c_str());

  ASSERT_TRUE(mcc->a.clear_dirty());
  ASSERT_TRUE(dc->a.clear_dirty());

  const auto expectStr2 = "Hello Universe 616";
  ctxt->Config.Set("a", expectStr2);
  ASSERT_TRUE(mcc->a.is_dirty());
  ASSERT_TRUE(dc->a.is_dirty());
  mcc->a.clear_dirty();
  dc->a.clear_dirty();
  ASSERT_STREQ(expectStr2, mcc->a->c_str());
  ASSERT_STREQ(expectStr2, dc->a->c_str());
}

TEST_F(AutoConfigTest, SubContextPropagation) {
  AutoCurrentContext ctxt;
  AutoCreateContext subCtxt(ctxt);

  std::shared_ptr<MyConfigurableClass> mcc = subCtxt->Inject<MyConfigurableClass>();
  ctxt->Config.Set("b", "10442");

  ASSERT_EQ(mcc->b, 10442) << "Setting of the \'b\' config was not propogated to the sub context.";
}

TEST_F(AutoConfigTest, SubContextDelayedPropagation) {
  AutoCurrentContext ctxt;
  AutoCreateContext subCtxt(ctxt);

  ctxt->Config.Set("b", "10442");
  std::shared_ptr<MyConfigurableClass> mcc = subCtxt->Inject<MyConfigurableClass>();

  ASSERT_EQ(mcc->b, 10442) << "Setting of the \'b\' config was not propogated to the sub context when it was injected after \'b\' was set.";
}

namespace {
  class slider
  {
  public:
    template<typename U>
    struct valid {
      static const bool value = std::is_arithmetic<U>::value;
    };
  };

  static_assert(aw::valid<float, slider>::value, "Float metadata field not detected as being valid");
  static_assert(!slider::valid<std::string>::value, "Slider should not believe strings are valid");
  static_assert(!aw::valid<std::string, slider>::value, "String metadata field incorrectly detected as being valid");

  class SliderManager {
  public:
    SliderManager(void) {
      AutoCurrentContext ctxt;
      ctxt->Config.WhenFn(
        [this] (const aw::config_event&, const slider& slider) {
          sliderReg.push_back(slider);
        }
      );
    }

    std::vector<slider> sliderReg;
  };

  class ClassWithBoundsField {
  public:
    std::atomic<int> b{ 929 };

    static autowiring::config_descriptor GetConfigDescriptor(void) {
      return{
        aw::config_field { "crazyjenkins", "Field B description", &ClassWithBoundsField::b, 929, slider{}, aw::bounds<int>{ 10, 423 } },
      };
    }
  };
}

TEST_F(AutoConfigTest, WhenFn) {
  AutoRequired<SliderManager> mgr;
  AutoRequired<ClassWithBoundsField> cwbf;

  ASSERT_EQ(1UL, mgr->sliderReg.size()) << "Slider registration count did not match expectations";
}

TEST_F(AutoConfigTest, CanEnumRegistry) {
  size_t nFound = 0;
  for (const config_descriptor& desc : autowiring::config_registry{}) {
    for (const auto& field : desc.fields) {
      if (!std::strcmp("crazyjenkins", field.second.name))
        nFound++;
    }
  }
  ASSERT_EQ(1U, nFound) << "Failed to find a descriptor field in the total descriptor enumeration";
}

TEST_F(AutoConfigTest, TeardownUnreference) {
  std::weak_ptr<ClassWithBoundsField> cwbf_weak;
  AutoCurrentContext ctxt;
  ctxt->Config.Set("crazyjenkins", "29291");

  {
    AutoCreateContext child;
    AutoRequired<ClassWithBoundsField> cwbf{ child };
    cwbf_weak = cwbf;
  }
  ASSERT_TRUE(cwbf_weak.expired()) << "Object leaked after context destruction";
}

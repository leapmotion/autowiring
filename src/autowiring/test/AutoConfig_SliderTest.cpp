// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/config_descriptor.h>
#include <autowiring/config.h>
#include <autowiring/ConfigRegistry.h>
#include <autowiring/observable.h>

namespace aw = autowiring;

class AutoConfig_SliderTest :
  public testing::Test
{};

namespace {
  struct slider {
    template<typename T>
    struct valid {
      static const bool value = std::is_integral<T>::value;
    };
  };

  class ClassWithASlider {
  public:
    std::string api_path;
    int timeout;

    static aw::config_descriptor GetConfigDescriptor(void) {
      return{
        { "api_path", &ClassWithASlider::api_path },
        { "timeout", &ClassWithASlider::timeout, slider{} }
      };
    }
  };

  class SliderManager :
    public autowiring::ConfigWatcher<slider>
  {
  public:
    std::vector<autowiring::config_event> all_configs;

    void OnMetadata(const aw::config_event& evt, const slider& metadata, aw::once& shutdown) override {
      all_configs.push_back(evt);
    }
  };
}

TEST_F(AutoConfig_SliderTest, CanFindAllSliders) {
  AutoRequired<SliderManager> mgr;

  for (size_t i = 0; i < 2; i++) {
    AutoCreateContext ctxt;
    ctxt->Inject<ClassWithASlider>();
  }

  ASSERT_EQ(2U, mgr->all_configs.size()) << "Slider manager failed to detect all added slider instances";
}

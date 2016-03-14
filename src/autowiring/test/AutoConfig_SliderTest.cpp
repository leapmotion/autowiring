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
}

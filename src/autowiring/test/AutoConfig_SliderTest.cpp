// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/config_descriptor.h>
#include <autowiring/config.h>
#include <autowiring/ConfigRegistry.h>
#include <autowiring/CoreThread.h>
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

    std::function<void(uint64_t)> assigner;
    int size = 0;

    void bind(volatile bool& field) {
      size = 1;
      assigner = [&field](uint64_t value) {
        field = value != 0;
      };
    }

    template<typename T>
    void bind(volatile T& field) {
      size = sizeof(T);
      assigner = [&field] (uint64_t value) {
        field = static_cast<T>(value);
      };
    }
  };
  static_assert(aw::has_bind<slider, int>::value, "Context-free bind not detected on a non-contextual binding type");
  static_assert(!aw::has_bind<slider, int, aw::config_field>::value, "Contextual bind incorrectly detected on a non-contextual binding type");

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
    public autowiring::ConfigBolt<slider>
  {
  public:
    std::vector<const slider*> all_sliders;

    void OnMetadata(const aw::config_event& evt, const slider& metadata, aw::once& shutdown) override {
      all_sliders.push_back(&metadata);
    }
  };
}

TEST_F(AutoConfig_SliderTest, CanFindAllSliders) {
  AutoRequired<SliderManager> mgr;

  for (size_t i = 0; i < 2; i++) {
    AutoCreateContext ctxt;
    ctxt->Inject<ClassWithASlider>();
  }

  ASSERT_EQ(2U, mgr->all_sliders.size()) << "Slider manager failed to detect all added slider instances";
}

namespace {
  class MonotonicIncreaseChecker :
    public CoreThread
  {
  public:
    volatile int value = 0;
    int priorValue = -1;

    volatile bool quit = false;

    // Success variable, holds false if we had a problem while processing
    bool succeeded = false;

    static aw::config_descriptor GetConfigDescriptor(void) {
      return{
        { "value", &MonotonicIncreaseChecker::value, slider{} },
        { "quit", &MonotonicIncreaseChecker::quit, slider{} }
      };
    }

    bool Spin(void) {
      while (!ShouldStop() && !quit) {
        if (value < priorValue)
          // Error, values received out of sequence!
          return false;
        priorValue = value;
      }
      return true;
    }

    void Run(void) override {
      succeeded = Spin();
    }
  };
}

TEST_F(AutoConfig_SliderTest, ConcurrentModification) {
  AutoCurrentContext ctxt;
  auto mgr = ctxt->Inject<SliderManager>();
  auto mic = ctxt->Inject<MonotonicIncreaseChecker>();
  ctxt->Initiate();

  ASSERT_EQ(2U, mgr->all_sliders.size()) << "Slider manager did not find all sliders as expected";

  // Set all integer sliders:
  for (int i = 0; i < 100; i++)
  {
    for (auto& slider : mgr->all_sliders)
      if(sizeof(int) == slider->size)
        slider->assigner(i);
  }

  // Now set boolean sliders, this should cause our class to exit because
  // the only boolean slider is there to cause the thread to exit
  for (auto& slider : mgr->all_sliders)
    if (sizeof(bool) == slider->size)
      slider->assigner(1);

  ASSERT_TRUE(mic->WaitFor(std::chrono::seconds{ 5 })) << "Counter class did not exit in time";
}

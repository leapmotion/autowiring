// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ConfigBolt.h"
#include "autowiring.h"

using namespace autowiring;

void ConfigWatcherBase::OnMetadata(const config_event& evt) {
  AutoCurrentContext ctxt;
  OnMetadata(evt, ctxt->onShutdown);
}

void ConfigWatcherBase::SetSelf(const std::shared_ptr<ConfigWatcherBase>& self) {
  m_self = self;
}

void ConfigWatcherBase::ContextCreated(void) {
  auto self = m_self.lock();
  if (!self)
    return;

  AutoCurrentContext ctxt;
  ctxt->Config.When(self);
}

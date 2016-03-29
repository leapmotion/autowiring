#include "stdafx.h"
#include "ConfigBolt.h"
#include "Autowiring.h"

using namespace autowiring;


void ConfigWatcherBase::OnMetadata(const config_event& evt) {
  AutoCurrentContext ctxt;
  OnMetadata(evt, ctxt->onShutdown);
}

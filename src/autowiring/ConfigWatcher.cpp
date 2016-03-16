#include "stdafx.h"
#include "ConfigWatcher.h"
#include "Autowiring.h"

using namespace autowiring;


void ConfigWatcherBase::OnMetadata(const config_event& evt) {
  AutoCurrentContext ctxt;
  OnMetadata(evt, ctxt->onShutdown);
}

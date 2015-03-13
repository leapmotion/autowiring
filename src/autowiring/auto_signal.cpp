// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "auto_signal.h"
#include "Autowired.h"
#include "SlotInformation.h"

using namespace autowiring;
using namespace autowiring::internal;

signal_registration_base::signal_registration_base(void) {

}

std::shared_ptr<signal_relay> signal_relay_registration_table::GetSignalRelay(const std::type_info& ti, size_t offset)
{
  std::lock_guard<std::mutex> lk(lock);
  auto& retVal = table[registration(&ti, offset)];
  if (!retVal)
    retVal = std::make_shared<signal_relay>();
  return retVal;
}

std::shared_ptr<signal_relay> autowiring::internal::ObtainRelay(void* pMember)
{
  auto location = SlotInformationStackLocation::CurrentStackLocation();
  if (!location || !location->Encloses(pMember))
    // Signal is being constructed outside of an autowired member, we need to create a default relay
    return std::make_shared<signal_relay>();

  // Signal is being constructed as a member, the registration table should know where the relay is
  return AutoRequired<signal_relay_registration_table>()->GetSignalRelay(
    location->stump.ti,
    location->Offset(pMember)
  );
}
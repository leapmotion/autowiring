#include "stdafx.h"
#include <autowiring/autowiring.h>
#include <autowiring/SatCounter.h>

class AutoFilterDiagnosticsTest:
  public testing::Test
{
public:
  AutoFilterDiagnosticsTest(void) {
    AutoCurrentContext()->Initiate();
  }
};

class FilterMemberNotInheritingObject {
public:
  void AutoFilter(const int&) {}
};

TEST_F(AutoFilterDiagnosticsTest, CanGetExpectedTrueType) {
  AutoRequired<FilterMemberNotInheritingObject> mnio;
  AutoRequired<AutoPacketFactory> factory;

  auto packet = factory->NewPacket();
  auto dispositions = packet->GetDispositions();
  ASSERT_EQ(1UL, dispositions.size()) << "Dispositions collection contained an unexpected AutoFilter";

  auto& disposition = dispositions.front();
  ASSERT_EQ(1UL, disposition.m_subscribers.size()) << "Expected exactly one subscriber for the sole present type";

  const SatCounter* descriptor = disposition.m_subscribers.front();
  AnySharedPointer asp(descriptor->GetAutoFilter());

  // Get more information about this object from the enclosing context:
  AutoCurrentContext ctxt;
  const std::type_info* ti;
  ASSERT_NO_THROW(ti = &ctxt->GetAutoTypeId(asp)) << "Exception thrown while attempting to get true type information";
  ASSERT_EQ(typeid(FilterMemberNotInheritingObject), *ti) << "True type not correctly reported, got " << autowiring::demangle(*ti) << ", expected FilterMemberNotInheritingObject";
}
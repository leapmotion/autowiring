#include "stdafx.h"
#include "MarshalingTest.h"
#include "EventOutputStream.h"
#include "uuid.h"

DECLARE_UUID(EventWithUuid, "6EC2129F-5DD7-43D5-ACB5-864E8BB5D6B4") :
  public virtual EventReceiver
{
public:
  virtual void SampleEventFiring(const std::string* str) = 0;
  virtual Deferred SampleEventDeferred(const std::string* str) = 0;
};

void VerifyProperStreamReceipt(EventOutputStream* os, const AutoFired<EventWithUuid>& ewuuid) {
  // Test fire an event:
  std::string str("012345678900123456789012345678901234567890");
  ewuuid(&EventWithUuid::SampleEventFiring)(&str);

  // Verify that the output stream is no longer empty and has some default properties:
  EXPECT_FALSE(os->IsEmpty()) << "An output stream on an event was empty, even though an event it should have caught was just fired";
  EXPECT_LT(0UL, os->GetSize()) << "The output stream should have had more than zero bytes in it after recording an event, but it did not";

  // Verify that the output stream is _at least as long_ as the string argument we passed earlier:
  EXPECT_LT(str.size(), os->GetSize()) << "An output stream contained fewer uncompressed bytes than were transmitted in a fired event";
}

TEST_F(MarshalingTest, VerifyListenersUpdated) {
  AutoFired<EventWithUuid> ewuuid;
  EXPECT_FALSE(ewuuid.HasListeners()) << "A newly created event incorrectly reported that listeners existed where none were subscribed";

  AutoCurrentContext ctxt;
  std::shared_ptr<EventOutputStream> os = ctxt->CreateEventOutputStream<EventWithUuid>();
  ASSERT_NE(nullptr, os.get()) << "Attempted to obtain an event stream from a context, but the returned pointer was null";

  // Should be listeners now:
  EXPECT_TRUE(ewuuid.HasListeners()) << "An output stream creation did not change the HasListeners disposition";

  // Let the output stream fall out of scope and verify that the listener existence disposition is updated:
  os.reset();
  EXPECT_FALSE(ewuuid.HasListeners()) << "An event incorrectly reported that it had listeners, even though its only listener--an output stream--is out of scope";
}

TEST_F(MarshalingTest, VerifyOutOfOrderFiring) {
  // We should be able to create an output stream on an event even if nobody fires this event right now
  AutoCurrentContext ctxt;
  std::shared_ptr<EventOutputStream> os = ctxt->CreateEventOutputStream<EventWithUuid>();
  ASSERT_NE(nullptr, os.get()) << "Failed to create an output stream in a context where no firers of the underlying event exist";

  // Verify that we get stream reciept even if we fire at _this_ point, after the stream exists
  VerifyProperStreamReceipt(os.get(), AutoFired<EventWithUuid>());
}

TEST_F(MarshalingTest, VerifySimpleSerialization) {
  AutoFired<EventWithUuid> ewuuid;

  AutoCurrentContext ctxt;
  std::shared_ptr<EventOutputStream> os = ctxt->CreateEventOutputStream<EventWithUuid>();
  ASSERT_NE(nullptr, os.get());

  // Should be empty before we fire anything:
  EXPECT_TRUE(os->IsEmpty()) << "Output stream was empty, even though it was just created";
  EXPECT_EQ(0UL, os->GetSize()) << "Output stream reported " << os->GetSize() << " bytes contained, it should have been empty";

  // Scoping behavior
  VerifyProperStreamReceipt(os.get(), ewuuid);

  // Flush the output stream and verify it does, in fact, reset its properties:
  os->Reset();
  EXPECT_TRUE(os->IsEmpty()) << "Output stream was not empty after being reset";

  // Let the output stream fall out of scope and verify that the listener existence disposition is updated:
  os.reset();
  EXPECT_FALSE(ewuuid.HasListeners()) << "An event incorrectly reported that it had listeners, even though its only listener--an output stream--is out of scope";
}
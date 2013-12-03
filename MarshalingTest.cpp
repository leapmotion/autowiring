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

TEST_F(MarshalingTest, VerifySimpleSerialization) {
  AutoFired<EventWithUuid> ewuuid;
  EXPECT_FALSE(ewuuid.HasListeners()) << "A newly created event incorrectly reported that listeners existed where none were subscribed";

  AutoCurrentContext ctxt;
  std::shared_ptr<EventOutputStream> outputStream = ctxt->CreateEventOutputStream<EventWithUuid>();
  ASSERT_NE(nullptr, outputStream.get()) << "Attempted to obtain an event stream from a context, but the returned pointer was null";

  // Should be empty before we fire anything:
  EXPECT_TRUE(outputStream->IsEmpty()) << "Output stream was empty, even though it was just created";
  EXPECT_EQ(0UL, outputStream->GetSize()) << "Output stream reported " << outputStream->GetSize() << " bytes contained, it should have been empty";

  // We DO have a listener, now that the output stream was created:
  EXPECT_TRUE(ewuuid.HasListeners()) << "An output stream creation did not change the HasListeners disposition";

  // Test fire an event:
  std::string str("012345678900123456789012345678901234567890");
  ewuuid(&EventWithUuid::SampleEventFiring)(&str);

  // Verify that the output stream is no longer empty and has some default properties:
  EXPECT_FALSE(outputStream->IsEmpty()) << "An output stream on an event was empty, even though an event it should have caught was just fired";
  EXPECT_LT(0UL, outputStream->GetSize()) << "The output stream should have had more than zero bytes in it after recording an event, but it did not";

  // Verify that the output stream is _at least as long_ as the string argument we passed earlier:
  EXPECT_LT(str.size(), outputStream->GetSize()) << "An output stream contained fewer uncompressed bytes than were transmitted in a fired event";

  // Flush the output stream and verify it does, in fact, reset its properties:
  outputStream->Reset();
  EXPECT_TRUE(outputStream->IsEmpty()) << "Output stream was not empty after being reset";

  // Let the output stream fall out of scope and verify that the listener existence disposition is updated:
  outputStream.reset();
  EXPECT_FALSE(ewuuid.HasListeners()) << "An event incorrectly reported that it had listeners, even though its only listener--an output stream--is out of scope";
}
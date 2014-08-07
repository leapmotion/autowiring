// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/EventInputStream.h>
#include <autowiring/EventOutputStream.h>
#include <autowiring/CoreThread.h>
#include <autowiring/Decompose.h>
#include <autowiring/uuid.h>
#include <string>
#include <sstream>

class MarshalingTest:
  public testing::Test
{};
                                                         
/*
EXTRA SPECS to code to that have not yet been implemented as unit tests.

1. Sender and Client each import the same header file which defines a UUID event.
2. Sender encodes UUID of its event in the serialized stream
3. Client reads UUID and args, and reverse-look ups the correct memfn to call the memfn
4. Client gracefully handles receiving UUID events it can't reverse lookup
5. EVERYONE should know when you change ANY identifying properties of a DECLARE_UUID
you must change the UUID as well or you will get version incompatilibity erros.
Extra points for figuring out how to enforce this.

Other Goals:
Broader SFINAE: force server and client to define their own Autoserialize or Autodeserialized
If the types aren't POD. At the very least, tests need to be written which violate the 
"I can do everything with stringstreams" implementation currently passing
*/

struct StandardType : public autowiring::Serialize{
  std::string m_str1;
  std::string m_str2;
  std::string m_str3;

  StandardType(){}

  StandardType(std::string s1, std::string s2, std::string s3){
    m_str1 = s1;
    m_str2 = s2;
    m_str3 = s3;
  }

  std::string AutoSerialize(void){
    return  m_str1 + "$" + m_str2 + "$" + m_str3 + "$";
  }

  void AutoDeserialize(std::string data){ 
    std::istringstream buf(data);
    std::deque<std::string> d;
    std::string s;
    while (std::getline(buf, s, '$'))
      d.push_back(s);

    m_str1 = d[0];
    m_str2 = d[1];
    m_str3 = d[2];
    return;
  }
};

DECLARE_UUID(EventWithUuid, "6EC2129F-5DD7-43D5-ACB5-864E8BB5D6B4")
{
public:
  virtual void SampleEventFiring(const std::string* str) = 0;
  virtual void SampleEventFiring0(void) = 0;
  virtual void SampleEventFiring3(const std::string* str1, const std::string* str2, const std::string* str3) = 0;
  virtual void SampleStandardFiring(const StandardType S1) = 0;
  virtual Deferred SampleEventDeferred(const std::string* str) = 0;
};

class ListenerForUuid :
  public CoreThread,
  public EventWithUuid
{
public:
  ListenerForUuid(void) :
    m_called(false)
  {
    //Need to call ready here. Should be refactored out of existence.
  }

  bool m_called;
  std::string m_str;
  std::string m_str2;
  std::string m_str3;

  void SampleEventFiring0() override {
    m_called = true;
  }

  void SampleEventFiring(const std::string* str) override {
    m_called = true;
    m_str = *str;
  }

  void SampleEventFiring3(const std::string* str1, const std::string* str2, const std::string* str3) override {
    m_called = true;
    m_str = *str1;
    m_str2 = *str2;
    m_str3 = *str3;
  }


  void SampleStandardFiring(const StandardType S1){
    m_called = true;
    m_str = S1.m_str1;
    m_str2 = S1.m_str2;
    m_str3 = S1.m_str3;
  }


  Deferred SampleEventDeferred(const std::string* str) override {
    return Deferred(this);
  }
};

template<class T>
void VerifyProperStreamReceipt(EventOutputStream<T>* os, const AutoFired<EventWithUuid>& ewuuid) {
  // Register our expected event type:
  os->template EnableIdentity(&EventWithUuid::SampleEventFiring);

  // Test fire an event:
  std::string str("012345678900123456789012345678901234567890");
  ewuuid(&EventWithUuid::SampleEventFiring)(&str);

  // Verify that the output stream is no longer empty and has some default properties:
  EXPECT_FALSE(os->IsEmpty()) << "An output stream on an event was empty, even though an event it should have caught was just fired";
  EXPECT_LT(0UL, os->GetSize()) << "The output stream should have had more than zero bytes in it after recording an event, but it did not";

  // Verify that the output stream is _at least as long_ as the string argument we passed earlier:
  EXPECT_LE(str.size(), os->GetSize()) << "An output stream contained fewer uncompressed bytes than were transmitted in a fired event";
}

template<class T>
void VerifyProperStreamReceiptZeroArgs(EventOutputStream<T>* os, const AutoFired<EventWithUuid>& ewuuid) {
  // Register our expected event type:
  os->template EnableIdentity(&EventWithUuid::SampleEventFiring0);

  // Test fire an event:
  ewuuid(&EventWithUuid::SampleEventFiring0)();

  // Verify that the output stream is no longer empty and has some default properties:
  EXPECT_FALSE(os->IsEmpty()) << "0Args: An output stream on an event was empty, even though an event it should have caught was just fired";
  EXPECT_LT(0UL, os->GetSize()) << "0Args: The output stream should have had more than zero bytes in it after recording an event, but it did not";
}

template<class T>
void VerifyProperStreamReceiptThreeArgs(EventOutputStream<T>* os, const AutoFired<EventWithUuid>& ewuuid) {
  // Register our expected event type:
  os->template EnableIdentity(&EventWithUuid::SampleEventFiring3);

  // Test fire an event:
  std::string str1("012345678900123456789012345678901234567890");
  std::string str2("012345678900123456789012345678901234567890");
  std::string str3("012345678900123456789012345678901234567890");
  ewuuid(&EventWithUuid::SampleEventFiring3)(&str1, &str2, &str3);

  // Verify that the output stream is no longer empty and has some default properties:
  EXPECT_FALSE(os->IsEmpty()) << "3Args: An output stream on an event was empty, even though an event it should have caught was just fired";
  EXPECT_LT(0UL, os->GetSize()) << "3Args: The output stream should have had more than zero bytes in it after recording an event, but it did not";

  // Verify that the output stream is _at least as long_ as the string argument we passed earlier:
  EXPECT_LE(str1.size() * 3, os->GetSize()) << "3Args: An output stream contained fewer uncompressed bytes than were transmitted in a fired event";
}

TEST_F(MarshalingTest, VerifyListenersUpdated) {
  AutoFired<EventWithUuid> ewuuid;
  EXPECT_FALSE(ewuuid.HasListeners()) << "A newly created event incorrectly reported that listeners existed where none were subscribed";

  AutoCurrentContext ctxt;
  std::shared_ptr<EventOutputStream<EventWithUuid>> os = ctxt->CreateEventOutputStream<EventWithUuid>();
  ASSERT_FALSE(nullptr == os.get()) << "Attempted to obtain an event stream from a context, but the returned pointer was null";

  // Should be listeners now:
  EXPECT_TRUE(ewuuid.HasListeners()) << "An output stream creation did not change the HasListeners disposition";
  os.reset();

  EXPECT_FALSE(ewuuid.HasListeners()) << "An event incorrectly reported that it had listeners, even though its only listener--an output stream--is out of scope";
}

TEST_F(MarshalingTest, VerifyOutOfOrderFiring) {
  // We should be able to create an output stream on an event even if nobody fires this event right now
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  std::shared_ptr<EventOutputStream<EventWithUuid>> os = ctxt->CreateEventOutputStream<EventWithUuid>();
  ASSERT_FALSE(nullptr == os.get()) << "Failed to create an output stream in a context where no firers of the underlying event exist";

  // Verify that we get stream reciept even if we fire at _this_ point, after the stream exists
  VerifyProperStreamReceipt(os.get(), AutoFired<EventWithUuid>());
}

TEST_F(MarshalingTest, VerifySimpleSerialization) {
  AutoFired<EventWithUuid> ewuuid;

  AutoCurrentContext ctxt;
  ctxt->Initiate();
  std::shared_ptr<EventOutputStream<EventWithUuid>> os = ctxt->CreateEventOutputStream<EventWithUuid>();

  ASSERT_NE(static_cast<void*>(nullptr), os.get());

  // Should be empty before we fire anything:
  EXPECT_TRUE(os->IsEmpty()) << "Output stream was empty, even though it was just created";
  EXPECT_EQ(0UL, os->GetSize()) << "Output stream reported " << os->GetSize() << " bytes contained, it should have been empty";

  // Scoping behavior
  VerifyProperStreamReceipt(os.get(), ewuuid);
  os->Reset();
  VerifyProperStreamReceiptZeroArgs(os.get(), ewuuid);
  os->Reset();
  VerifyProperStreamReceiptThreeArgs(os.get(), ewuuid);
  // Flush the output stream and verify it does, in fact, reset its properties:
  os->Reset();
  EXPECT_TRUE(os->IsEmpty()) << "Output stream was not empty after being reset";

  // Let the output stream fall out of scope and verify that the listener existence disposition is updated:
  os.reset();
  EXPECT_FALSE(ewuuid.HasListeners()) << "An event incorrectly reported that it had listeners, even though its only listener--an output stream--is out of scope";
}

TEST_F(MarshalingTest, VerifySimpleDeserialization) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  // Serialize a fired event first:
  AutoFired<EventWithUuid> ewuuid;
  std::shared_ptr<EventOutputStream<EventWithUuid>> os = ctxt->CreateEventOutputStream<EventWithUuid>();
  ASSERT_NE(static_cast<void*>(nullptr), os.get());

  std::string helloWorld = "Hello, world!";
  std::string helloWorldAgain = "Hello, world, again!";
  std::string helloWorldYetAgain = "Hello, world, yet again!";
  
  ewuuid(&EventWithUuid::SampleEventFiring)(&helloWorld);
  ewuuid(&EventWithUuid::SampleEventFiring)(&helloWorldAgain);
  ewuuid(&EventWithUuid::SampleEventFiring3)(&helloWorld, &helloWorldAgain, &helloWorldYetAgain);
  ASSERT_FALSE(os->IsEmpty());
  
  // Inject the listener into the context:
  AutoRequired<ListenerForUuid> listener;
  
  ASSERT_TRUE(listener->m_str.empty()) << "Listener unexpectedly received messages fired before its construction";
  
  // Now we create an input stream and use it to replay events from the output stream:
  std::shared_ptr<EventInputStream<EventWithUuid>> is = ctxt->CreateEventInputStream<EventWithUuid>();

  ASSERT_FALSE(nullptr == is.get()) << "Event input stream was empty";
  
  // Register our expected event type:
  is->EnableIdentity(&EventWithUuid::SampleEventFiring3);
  is->EnableIdentity(&EventWithUuid::SampleEventFiring);
  is->EnableIdentity(&EventWithUuid::SampleEventDeferred);
 
  const void* ptr = os->GetData(); //This is damn unsafe. Who is supposed to be doing cleanup?
  size_t nRemaining = os->GetSize();
  size_t advanceBy = is->FireSingle(ptr, nRemaining);
   
  ASSERT_NE(0UL, advanceBy) << "Input stream did not correctly report the number of bytes deserialized";
  ASSERT_LE(advanceBy, nRemaining) << "Input stream processed more bytes from the passed buffer than were available for processing";
 
  // Verify that the listener got _something_, and the thing it got was the thing we sent earlier:
  EXPECT_TRUE(listener->m_called) << "Listener failed to receive any events from the event input stream";
  EXPECT_EQ(helloWorld, listener->m_str) << "Listener received an event, but the payload of the event was not the same as what was originally serialized";
  
  // Clear, advance, and fire the next event:
  listener->m_called = false;
  listener->m_str.clear();
  (char*&) ptr += advanceBy;
  nRemaining -= advanceBy;
  advanceBy = is->FireSingle(ptr, nRemaining);
  ASSERT_NE(0UL, advanceBy) << "A second attempt to fire an event failed to parse any bytes";
  ASSERT_LE(advanceBy, nRemaining) << "Input stream overran its buffer for the second fired event";
  
  // Now verify that we got called again:
  ASSERT_TRUE(listener->m_called) << "Second event was not received from the event input stream";
  ASSERT_EQ(helloWorldAgain, listener->m_str) << "Listener did not receive the second message payload from the input stream";

  // Clear, advance, and fire the next event:
  listener->m_called = false;
  listener->m_str.clear();
  (char*&)ptr += advanceBy;
  nRemaining -= advanceBy;
  advanceBy = is->FireSingle(ptr, nRemaining);
  ASSERT_NE(0UL, advanceBy) << "A third attempt to fire an event failed to parse any bytes";
  ASSERT_LE(advanceBy, nRemaining) << "Input stream overran its buffer for the third fired event";

  // Now verify that we got called again:
  ASSERT_TRUE(listener->m_called) << "Third event was not received from the event input stream";
  ASSERT_EQ(helloWorld, listener->m_str) << "Listener did not receive the third message payload, 1 arg,  from the input stream";
  ASSERT_EQ(helloWorldAgain, listener->m_str2) << "Listener did not receive the third message payload, 2 arg,  from the input stream";
  ASSERT_EQ(helloWorldYetAgain, listener->m_str3) << "Listener did not receive the third message payload, 3 arg,  from the input stream";

  // Ensure that we processed EXACTLY the number of bytes that were in the output stream:
  EXPECT_EQ(advanceBy, nRemaining) << "Output stream wrote extraneous bytes to its buffer which were not used during deserialization";
}

TEST_F(MarshalingTest, VerifyComplexDeserialization) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  // Serialize a fired event first:
  AutoFired<EventWithUuid> ewuuid;
  std::shared_ptr<EventOutputStream<EventWithUuid>> os = ctxt->CreateEventOutputStream<EventWithUuid>();
  ASSERT_NE(static_cast<void*>(nullptr), os.get());

  std::string helloWorld = "Hello, world!";
  std::string helloWorldAgain = "Hello, world, again!";
  std::string helloWorldYetAgain = "Hello, world, yet again!";

  ewuuid(&EventWithUuid::SampleEventFiring3)(&helloWorld, &helloWorldAgain, &helloWorldYetAgain);
  ASSERT_FALSE(os->IsEmpty());

  // Inject the listener into the context:
  AutoRequired<ListenerForUuid> listener;

  ASSERT_TRUE(listener->m_str.empty()) << "Listener unexpectedly received messages fired before its construction";

  // Now we create an input stream and use it to replay events from the output stream:
  std::shared_ptr<EventInputStream<EventWithUuid>> is = ctxt->CreateEventInputStream<EventWithUuid>();

  ASSERT_FALSE(nullptr == is.get()) << "Event input stream was empty";

  // Register our expected event type:
  is-> EnableIdentity(&EventWithUuid::SampleEventFiring3);

  const void* ptr = os->GetData(); //This is damn unsafe. Who is supposed to be doing cleanup?
  size_t nRemaining = os->GetSize();
  size_t advanceBy = is->FireSingle(ptr, nRemaining);

  ASSERT_NE(0UL, advanceBy) << "Input stream did not correctly report the number of bytes deserialized";
  ASSERT_LE(advanceBy, nRemaining) << "Input stream processed more bytes from the passed buffer than were available for processing";

  // Verify that the listener got _something_, and the thing it got was the thing we sent earlier:
  ASSERT_TRUE(listener->m_called) << "Third event was not received from the event input stream";
  ASSERT_EQ(helloWorld, listener->m_str) << "Listener did not receive the first message payload, 1 arg,  from the input stream";
  ASSERT_EQ(helloWorldAgain, listener->m_str2) << "Listener did not receive the first message payload, 2 arg,  from the input stream";
  ASSERT_EQ(helloWorldYetAgain, listener->m_str3) << "Listener did not receive the first message payload, 3 arg,  from the input stream";  

  // Ensure that we processed EXACTLY the number of bytes that were in the output stream:
  EXPECT_EQ(advanceBy, nRemaining) << "Output stream wrote extraneous bytes to its buffer which were not used during deserialization";
}

TEST_F(MarshalingTest, VerifyAutoSerAndDeser) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  // Serialize a fired event first:
  AutoFired<EventWithUuid> ewuuid;
  std::shared_ptr<EventOutputStream<EventWithUuid>> os = ctxt->CreateEventOutputStream<EventWithUuid>();
  ASSERT_NE(static_cast<void*>(nullptr), os.get());

  // Register our expected event type:
  os->EnableIdentity(&EventWithUuid::SampleStandardFiring);

  std::string helloWorld = "Hello, world!";
  std::string helloWorldAgain = "Hello, world, again!";
  std::string helloWorldYetAgain = "Hello, world, yet again!";

  StandardType st(helloWorld, helloWorldAgain, helloWorldYetAgain);

  ewuuid(&EventWithUuid::SampleStandardFiring)(st);
  ASSERT_FALSE(os->IsEmpty());

  // Inject the listener into the context:
  AutoRequired<ListenerForUuid> listener;

  ASSERT_TRUE(listener->m_str.empty()) << "Listener unexpectedly received messages fired before its construction";

  // Now we create an input stream and use it to replay events from the output stream:
  std::shared_ptr<EventInputStream<EventWithUuid>> is = ctxt->CreateEventInputStream<EventWithUuid>();

  ASSERT_NE(static_cast<void*>(nullptr), is.get()) << "Event input stream was empty";

  // Register our expected event type:
  is->EnableIdentity(&EventWithUuid::SampleStandardFiring);

  const void* ptr = os->GetData(); //This is damn unsafe. Who is supposed to be doing cleanup?
  size_t nRemaining = os->GetSize();
  size_t advanceBy = is->FireSingle(ptr, nRemaining);

  ASSERT_NE(0UL, advanceBy) << "Input stream did not correctly report the number of bytes deserialized";
  ASSERT_LE(advanceBy, nRemaining) << "Input stream processed more bytes from the passed buffer than were available for processing";

  // Verify that the listener got _something_, and the thing it got was the thing we sent earlier:
  ASSERT_TRUE(listener->m_called) << "Third event was not received from the event input stream";
  ASSERT_EQ(helloWorld, listener->m_str) << "Listener did not receive the first message payload, 1 arg,  from the input stream";
  ASSERT_EQ(helloWorldAgain, listener->m_str2) << "Listener did not receive the first message payload, 2 arg,  from the input stream";
  ASSERT_EQ(helloWorldYetAgain, listener->m_str3) << "Listener did not receive the first message payload, 3 arg,  from the input stream";

  // Ensure that we processed EXACTLY the number of bytes that were in the output stream:
  EXPECT_EQ(advanceBy, nRemaining) << "Output stream wrote extraneous bytes to its buffer which were not used during deserialization";
}

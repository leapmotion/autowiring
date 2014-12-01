// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/AutoMerge.h>
#include <autowiring/AutoStile.h>
#include <autowiring/NewAutoFilter.h>

class AutoFilterPipeTest:
  public testing::Test
{
public:
  AutoFilterPipeTest(void) {
    AutoCurrentContext()->Initiate();
  }
};


class FilterDiamondIn:
  public ContextMember
{
public:
  int m_called;
  FilterDiamondIn(void) : m_called(0) {}
  void AutoFilter(auto_out<Decoration<0>> init) {
    ++m_called;
    init->i = 1;
  }
};

class FilterDiamondA:
  public ContextMember
{
public:
  int m_called;
  FilterDiamondA(void) : m_called(0) {}
  void AutoFilter(const Decoration<0>& in, auto_out<Decoration<1>> out) {
    ++m_called;
    out->i = 2;
  }
};

class FilterDiamondB:
  public ContextMember
{
public:
  int m_called;
  FilterDiamondB(void) : m_called(0) {}
  void AutoFilter(const Decoration<0>& in, auto_out<Decoration<1>> out) {
    ++m_called;
    out->i = 3;
  }
};

class FilterDiamondOut:
  public ContextMember
{
public:
  int m_called;
  Decoration<1> m_inLast;
  FilterDiamondOut(void) : m_called(0) {}
  void AutoFilter(const Decoration<1>& in) {
    ++m_called;
    m_inLast = in;
  }
};

class DiamondFilter:
  public ContextMember
{
public:
  DiamondFilter() {
    Reset();
  }

  void Reset() {
    In->m_called = 0;
    A->m_called = 0;
    B->m_called = 0;
    Out->m_called = 0;
    In_expected = 0;
    A_expected = 0;
    B_expected = 0;
    Out_expected = 0;
  }

  void Verify() {
    ASSERT_EQ(In_expected, In->m_called) << "Diamond Filter I called " << In->m_called << " expected " << In_expected;
    ASSERT_EQ(A_expected, A->m_called) << "Diamond Filter A called " << A->m_called << " expected " << A_expected;
    ASSERT_EQ(B_expected, B->m_called) << "Diamond Filter B called " << B->m_called << " expected " << B_expected;
    ASSERT_EQ(Out_expected, Out->m_called) << "Diamond Filter O called " << Out->m_called << " expected " << Out_expected;
  }

  AutoRequired<FilterDiamondIn> In;
  AutoRequired<FilterDiamondA> A;
  AutoRequired<FilterDiamondB> B;
  AutoRequired<FilterDiamondOut> Out;

  int In_expected;
  int A_expected;
  int B_expected;
  int Out_expected;
};

TEST_F(AutoFilterPipeTest, DataPipeTest) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;
  DiamondFilter diamond;

  //Diamond configuration will throw on creation of the packet, preventing any calls
  ASSERT_THROW(factory->NewPacket(), std::runtime_error) << "Failed to anticipate broadcast collision";
  diamond.Verify();
  diamond.Reset();

  //Incorrect pipe declarations will throw
  ASSERT_THROW(factory->BroadcastDataIn<FilterDiamondIn>(&typeid(Decoration<1>),false), std::runtime_error) << "Failed to throw missing type";
  ASSERT_THROW(factory->BroadcastDataIn<FilterDiamondA>(&typeid(Decoration<1>),false), std::runtime_error) << "Failed to throw incorrect orientation";

  //Permit DiamondA to use pipes only, which will prevent data collision, even though all filters are called.
  factory->BroadcastDataOut<FilterDiamondA>(&typeid(Decoration<1>),false);
  ASSERT_NO_THROW(factory->NewPacket()) << "Incorrect data collision";
  ++diamond.In_expected;
  ++diamond.A_expected;
  ++diamond.B_expected;
  ++diamond.Out_expected;
  diamond.Verify();
  diamond.Reset();

  //Permit DiamondIn to use pipes only, which will prevent data propagation
  factory->BroadcastDataOut<FilterDiamondIn>(&typeid(Decoration<0>),false);
  factory->NewPacket();
  ++diamond.In_expected;
  diamond.Verify();
  diamond.Reset();

  //Connect DiamondIn to DiamondA
  factory->PipeData<FilterDiamondIn, FilterDiamondA>(&typeid(Decoration<0>));
  factory->NewPacket();
  ++diamond.In_expected;
  ++diamond.A_expected;
  diamond.Verify();
  diamond.Reset();

  //Connect DiamondA to DiamondOut, which will cause a collision
  //PROBLEM: Exception is thrown, but termination in ~AutoCheckout is not caught
  /*
  factory->PipeData<FilterDiamondIn, FilterDiamondB>(); //Pipe all correctly oriented types
  factory->PipeData<FilterDiamondA, FilterDiamondOut>(); //Pipe all correctly oriented types
  ASSERT_THROW(factory->NewPacket(), std::runtime_error) << "Data failed to collide";
   */
}

template<int num = 0>
class FilterOD0 {
public:
  void AutoFilter(auto_out<Decoration<0>> out) {
    out->i = num;
  }
};

class FilterID0 {
public:
  void AutoFilter(const Decoration<0>& in) {}
};

TEST_F(AutoFilterPipeTest, DISABLED_VerifyMergedOutputs) {
  AutoRequired<AutoPacketFactory> factory;

  AutoRequired<FilterOD0<0>> f0D0_broadcast;
  AutoRequired<FilterOD0<1>> f1D0_pipe_merge;
  AutoRequired<FilterOD0<2>> f1D0_pipe_fID0;
  AutoRequired<FilterID0> fID0_pipe_f2D0;

  AutoRequired<AutoMerge<Decoration<0>>> merge;

  // Configure Data Flow
  factory->BroadcastDataIn<FilterID0>(nullptr, false);
  factory->BroadcastDataOut<FilterOD0<1>>(nullptr, false);
  factory->BroadcastDataOut<FilterOD0<2>>(nullptr, false);
  factory->PipeData<FilterOD0<1>, AutoMerge<Decoration<0>>>();
  factory->PipeData<FilterOD0<2>, FilterID0>();
  // Resulting Flow:
  // f0D0 is broadcast, and will be received by merge
  // f1D0 -> merge only
  // f2D0 -> fID0 only

  AutoMerge<Decoration<0>>::merge_data extracted;
  {
    std::shared_ptr<AutoPacket> packet;
    ASSERT_NO_THROW(packet = factory->NewPacket()) << "Multi-Decorate hit an exception";
    packet->Decorate(AutoMerge<Decoration<0>>::merge_call([&extracted](const AutoMerge<Decoration<0>>::merge_data& data) {
      extracted = data;
    }));

    const Decoration<0>* dec0;
    ASSERT_TRUE(packet->Get(dec0)) << "Broadcast data should be present";
    ASSERT_EQ(dec0->i, 0) << "Incorrect value for broadcast data";
    ASSERT_FALSE(packet->Get(dec0, typeid(SelectTypeUnifier<FilterOD0<0>>::type))) << "Sourced data should be absent from broadcasting source";
    ASSERT_TRUE(packet->Get(dec0, typeid(SelectTypeUnifier<FilterOD0<1>>::type))) << "Sourced data should be present from non-broadcasting source";
    ASSERT_EQ(dec0->i, 1) << "Incorrect value for piped data";
    ASSERT_TRUE(packet->Get(dec0, typeid(SelectTypeUnifier<FilterOD0<2>>::type))) << "Sourced data should be present from non-broadcasting source";
    ASSERT_EQ(dec0->i, 2) << "Incorrect value for piped data";

    // Final-Call methods
    ASSERT_EQ(1, packet->HasAll<Decoration<0>>()) << "Single Broadcast source only";
    ASSERT_EQ(1, packet->HasAll<Decoration<0>>(typeid(SelectTypeUnifier<AutoMerge<Decoration<0>>>::type))) << "Single Piped source only";
    ASSERT_EQ(1, packet->HasAll<Decoration<0>>(typeid(SelectTypeUnifier<FilterID0>::type))) << "Single Piped source only";
  }
  ASSERT_EQ(2, extracted.size()) << "Should collect 1 broadcast & 1 pipe";
}

TEST_F(AutoFilterPipeTest, VerifyForwardAll) {
  const std::shared_ptr<Decoration<0>>* contents = nullptr;
  AutoRequired<AutoPacketFactory> factory;
  std::shared_ptr<AutoPacket> toPacket = factory->NewPacket();
  {
    std::shared_ptr<AutoPacket> fromPacket = factory->NewPacket();
    fromPacket->Decorate(Decoration<0>(1));
    contents = nullptr; //contents does not own referenced data
    ASSERT_TRUE(fromPacket->Get(contents)) << "ForwardedAll failed to move data";
    ASSERT_EQ(1, (*contents)->i) << "ForwardedAll failed to move data";

    fromPacket->ForwardAll(toPacket);
    contents = nullptr; //contents does not own referenced data
    ASSERT_TRUE(toPacket->Get(contents)) << "ForwardedAll failed to move data";
    ASSERT_EQ(1, (*contents)->i) << "ForwardedAll failed to move data";
  }
  contents = nullptr; //contents does not own referenced data
  ASSERT_TRUE(toPacket->Get(contents)) << "Forwarded data is not persistent";
  ASSERT_EQ(1, (*contents)->i) << "Forwarded data is not persistent";
}

class JunctionDeferred01:
  public CoreThread
{
public:
  Deferred AutoFilter(const Decoration<0>& in, auto_out<Decoration<1>> out) {
    // Wait for AutoStile method to exit
    AutoCurrentContext()->Wait(std::chrono::milliseconds(10));

    out->i = in.i;
    return Deferred(this);
  }
};

class SlaveContext {};
class MasterContext {};

TEST_F(AutoFilterPipeTest, VerifyContextStile) {
  // Stile injects Decoration<0> and extracts Decoration<1>
  typedef AutoStile<
    const Decoration<0>&,
    auto_out<Decoration<1>>,
    auto_out<Decoration<2>>
  > test_stile;
  std::shared_ptr<test_stile> stile;
  std::shared_ptr<AutoPacketFactory> master_factory;

  AutoCreateContextT<SlaveContext> slave_context;
  {
    CurrentContextPusher pusher(slave_context);
    AutoRequired<AutoPacketFactory>();
    AutoRequired<JunctionDeferred01>();
    slave_context->Initiate();
  }

  AutoCreateContextT<MasterContext> master_context;
  {
    CurrentContextPusher pusher(master_context);
    master_factory = AutoRequired<AutoPacketFactory>();
    stile = AutoRequired<test_stile>();
    master_context->Initiate();
  }

  stile->Leash(slave_context);
  {
    CurrentContextPusher pusher(master_context);
    std::shared_ptr<AutoPacket> master_packet;
    master_packet = master_factory->NewPacket();
    int init = -1;
    master_packet->Decorate(Decoration<0>(init));

    // Create a last-call so that completion of slave context
    // is verified.
    master_packet->AddRecipient(std::function<void(const AutoPacket&)>([init](const AutoPacket& master_final){
      const Decoration<1>* out1;
      ASSERT_TRUE(master_final.Get(out1)) << "Stile failed to send & retrieve data";
      ASSERT_EQ(init, out1->i) << "Output was not from junction in slave context";
      const Decoration<2>* out2;
      ASSERT_FALSE(master_final.Get(out2)) << "Decoration<2> was not produced in slave context";
    }));
  }
}

class Junction01 {
public:
  void AutoFilter(const Decoration<0>& in, auto_out<Decoration<1>> out) {
    out->i = in.i;
  }
};

TEST_F(AutoFilterPipeTest, VerifyStileExtractAll) {
  // Stile injects Decoration<0> and extracts Decoration<1>
  std::shared_ptr<AutoStile<const Decoration<0>&>> stile;
  std::shared_ptr<AutoPacketFactory> master_factory;

  AutoCreateContextT<SlaveContext> slave_context;
  {
    CurrentContextPusher pusher(slave_context);
    AutoRequired<AutoPacketFactory>();
    AutoRequired<Junction01>();
    slave_context->Initiate();
  }

  AutoCreateContextT<MasterContext> master_context;
  {
    CurrentContextPusher pusher(master_context);
    master_factory = AutoRequired<AutoPacketFactory>();
    stile = AutoRequired<AutoStile<const Decoration<0>&>>();
    master_context->Initiate();
  }

  stile->Leash(slave_context);
  {
    CurrentContextPusher pusher(master_context);
    std::shared_ptr<AutoPacket> master_packet;
    master_packet = master_factory->NewPacket();
    master_packet->Decorate(Decoration<0>());
    ASSERT_TRUE(master_packet->Has<Decoration<1>>()) << "Stile failed to send & retrieve data";
  }
}

TEST_F(AutoFilterPipeTest, DISABLED_ExtractMergedData) {
  std::shared_ptr<AutoStile<const AutoMergeStile<Decoration<0>>::merge_call&>> stile;
  std::shared_ptr<AutoPacketFactory> master_factory;

  AutoCreateContextT<SlaveContext> slave_context;
  {
    CurrentContextPusher pusher(slave_context);

    AutoRequired<AutoPacketFactory>();
    AutoRequired<FilterOD0<0>> f0D0;
    AutoRequired<FilterOD0<1>> f1D0;

    slave_context->Initiate();
  }

  AutoCreateContextT<MasterContext> master_context;
  {
    CurrentContextPusher pusher(master_context);

    master_factory = AutoRequired<AutoPacketFactory>();
    stile = AutoRequired<AutoStile<const AutoMergeStile<Decoration<0>>::merge_call&>>();
    AutoConstruct<AutoMergeStile<Decoration<0>>> merge(*slave_context);

    merge->GetMerge()->PipeToMerge<FilterOD0<0>>();
    merge->GetMerge()->PipeToMerge<FilterOD0<1>>();

    master_context->Initiate();
  }

  stile->Leash(slave_context);
  {
    CurrentContextPusher pusher(master_context);
    std::shared_ptr<AutoPacket> master_packet;
    master_packet = master_factory->NewPacket();
    master_packet->Decorate(Decoration<0>());
    const AutoMergeStile<Decoration<0>>::merge_data* data = nullptr;
    ASSERT_TRUE(master_packet->Get(data)) << "Stile failed to send & retrieve data";
    ASSERT_EQ(2, data->size()) << "Merge failed to gather all data";
  }
}


template<int out, int in>
class FilterGatherAutoOut {
public:
  FilterGatherAutoOut() :
    FilterGather_AutoGather(this, &FilterGatherAutoOut<out, in>::AutoGather),
    m_called_out(0),
    m_called_in(0),
    m_out(out),
    m_in(in)
  {}

  void AutoFilter(auto_out<Decoration<out>> output) {
    ++m_called_out;
    output->i = m_out;
  }

  NewAutoFilter FilterGather_AutoGather;
  void AutoGather(const Decoration<in>& input) {
    ++m_called_in;
    m_in = input.i;
  }

  int m_called_out;
  int m_called_in;
  int m_out;
  int m_in;
};

TEST_F(AutoFilterPipeTest, VerifyTwoAutoFilterCallsAutoOut) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<FilterGatherAutoOut<0, 1>> zero2one;
  AutoRequired<FilterGatherAutoOut<1, 0>> one2zero;
  zero2one->m_out = 3;
  one2zero->m_out = 4;

  //Verify that calls made on allocation from object pool do not introduce a race condition
  {
    std::shared_ptr<AutoPacket> packet = factory->NewPacket();
    ASSERT_EQ(1, zero2one->m_called_out) << "AutoFilter with auto_out as only argument was called " << zero2one->m_called_out << " times";
    ASSERT_EQ(1, zero2one->m_called_in) << "AutoFilter of implicitly decorated type was called " << zero2one->m_called_in << " times";
    ASSERT_EQ(4, zero2one->m_in) << "AutoFilter received incorrect input of " << zero2one->m_in;
    ASSERT_EQ(1, one2zero->m_called_out) << "AutoFilter with auto_out as only argument was called " << one2zero->m_called_out << " times";
    ASSERT_EQ(1, one2zero->m_called_in) << "AutoFilter of implicitly decorated type was called " << one2zero->m_called_in << " times";
    ASSERT_EQ(3, one2zero->m_in) << "AutoFilter received incorrect input of " << one2zero->m_in;
    zero2one->m_out = 5;
    one2zero->m_out = 6;
  }
  //Verify that no additional calls are made during return of packet to object pool
  ASSERT_EQ(1, zero2one->m_called_out) << "AutoFilter with auto_out as only argument was called " << zero2one->m_called_out << " times";
  ASSERT_EQ(1, zero2one->m_called_in) << "AutoFilter of implicitly decorated type was called " << zero2one->m_called_in << " times";
  ASSERT_EQ(4, zero2one->m_in) << "AutoFilter received incorrect input of " << zero2one->m_in;
  ASSERT_EQ(1, one2zero->m_called_out) << "AutoFilter with auto_out as only argument was called " << one2zero->m_called_out << " times";
  ASSERT_EQ(1, one2zero->m_called_in) << "AutoFilter of implicitly decorated type was called " << one2zero->m_called_in << " times";
  ASSERT_EQ(3, one2zero->m_in) << "AutoFilter received incorrect input of " << one2zero->m_in;
}

TEST_F(AutoFilterPipeTest, EmptyTupleHash) {
  std::tuple<> empty;
  std::hash<std::tuple<>> hasher;
  size_t foo = hasher(empty);
  ASSERT_EQ(foo, 0);
}

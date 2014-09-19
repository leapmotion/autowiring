// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/auto_in.h>
#include <autowiring/auto_out.h>
#include <autowiring/auto_arg.h>
#include <autowiring/has_autofilter.h>
#include <autowiring/is_autofilter.h>

class ArgumentTypeTest:
public testing::Test
{
public:
  ArgumentTypeTest(void) {
    // All decorator tests must run from an initiated context
    AutoCurrentContext()->Initiate();
  }
};

template<int N>
class Argument {
public:
  int i;
  Argument(int j = N) : i(j) {}
};

void FilterFunction(const Argument<0>& typeIn, auto_out<Argument<1>> typeOut) {
  typeOut->i += 1 + typeIn.i;
}

typedef std::function<void(const Argument<0>&, auto_out<Argument<1>>)> FilterFunctionType;

typedef std::function<void(void)> NonFilterFunctionType0;
typedef std::function<void(Argument<1> noEdge, const Argument<0>& typeIn)> NonFilterFunctionType1;
typedef std::function<void(const Argument<0>& typeIn, Argument<1> noEdge)> NonFilterFunctionType2;
typedef std::function<int(const Argument<0>& typeIn, auto_out<Argument<1>>& typeOut)> NonFilterFunctionType3;

TEST_F(ArgumentTypeTest, AutoFilterTemplateTests) {
  ASSERT_TRUE(static_cast<const bool>(is_required_input<const Argument<0>&>::value)) << "Input type FAIL";
  ASSERT_TRUE(static_cast<const bool>(is_required_output<Argument<0>&>::value)) << "Output type FAIL";
  ASSERT_TRUE(static_cast<const bool>(is_autofilter_arg<const Argument<0>&>::is_input)) << "Input type FAIL";
  ASSERT_TRUE(static_cast<const bool>(is_autofilter_arg<Argument<0>&>::is_output)) << "Output type FAIL";

  ASSERT_TRUE(is_optional_ptr<optional_ptr<Argument<0>>>::value) << "Type of optional_ptr instance incorrectly identified";
  ASSERT_TRUE(is_auto_out<auto_out<Argument<0>>>::value) << "Type of auto_out instance incorrectly identified";

  ASSERT_FALSE(static_cast<const bool>(is_autofilter_arg<const Argument<0>>::value)) << "Validity of AutoFilter input incorrectly identified";
  ASSERT_FALSE(static_cast<const bool>(is_autofilter_arg<Argument<0>>::value)) << "Validity of AutoFilter input incorrectly identified";

  ASSERT_TRUE(is_autofilter_arg<const Argument<0>&>::is_input) << "Incorrect IO assessment";
  ASSERT_TRUE(is_autofilter_arg<Argument<0>&>::is_output) << "Incorrect IO assessment";
  ASSERT_TRUE(is_autofilter_arg<auto_out<Argument<0>>>::is_output) << "Incorrect IO assessment";

  ASSERT_TRUE(static_cast<const bool>(is_autofilter_arg<const Argument<0>&>::is_required)) << "Incorrect OR assessment";
  ASSERT_TRUE(static_cast<const bool>(is_autofilter_arg<Argument<0>&>::is_required)) << "Incorrect OR assessment";
  ASSERT_TRUE(static_cast<const bool>(is_autofilter_arg<auto_out<Argument<0>, true>>::is_required)) << "Incorrect OR assessment";
  ASSERT_TRUE(static_cast<const bool>(is_autofilter_arg<optional_ptr<Argument<0>>>::is_optional)) << "Incorrect OR assessment";
  ASSERT_TRUE(static_cast<const bool>(is_autofilter_arg<auto_out<Argument<0>, false>>::is_optional)) << "Incorrect OR assessment";

  ASSERT_TRUE(static_cast<const bool>(is_autofilter_arg<const Argument<0>&>::value)) << "Validity of AutoFilter input incorrectly identified";
  ASSERT_TRUE(static_cast<const bool>(is_autofilter_arg<Argument<0>&>::value)) << "Validity of AutoFilter output incorrectly identified";

  ASSERT_TRUE(static_cast<const bool>(is_autofilter_arg<auto_out<Argument<0>>>::value)) << "Validity of AutoFilter output incorrectly indentified";
  //ASSERT_FALSE(static_cast<const bool>(is_autofilter_arg<auto_out<Argument<0>>&>::value)) << "Validity of AutoFilter output incorrectly indentified";

  ASSERT_TRUE(static_cast<const bool>(is_autofilter_arg<optional_ptr<Argument<0>>>::value)) << "Validity of AutoFilter output incorrectly indentified";
  //ASSERT_FALSE(static_cast<const bool>(is_autofilter_arg<optional_ptr<Argument<0>>&>::value)) << "Validity of AutoFilter output incorrectly indentified";

  ASSERT_FALSE(static_cast<const bool>(all_autofilter_args<const Argument<0>&, Argument<0>>::value)) << "Invalid argument list incorrectly identified";
  ASSERT_FALSE(static_cast<const bool>(all_autofilter_args<Argument<0>, const Argument<0>&>::value)) << "Invalid argument list incorrectly identified";
  ASSERT_FALSE(static_cast<const bool>(all_autofilter_args<>::value)) << "Invalid argument list incorrectly identified";
  ASSERT_TRUE(static_cast<const bool>(all_autofilter_args<const Argument<0>&>::value)) << "Valid argument list incorrectly identified";
  ASSERT_TRUE(static_cast<const bool>(all_autofilter_args<auto_out<Argument<1>>>::value)) << "Valid argument list incorrectly identified";
  ASSERT_TRUE(static_cast<const bool>(all_autofilter_args<const Argument<0>&, auto_out<Argument<1>>>::value)) << "Valid argument list incorrectly identified";

  ASSERT_FALSE(static_cast<const bool>(is_autofilter_return<int>::value)) << "Incorrect identification of int as valid AutoFilter return type";
  ASSERT_TRUE(static_cast<const bool>(is_autofilter_return<void>::value)) << "Incorrect identification of void as invalid AutoFilter return type";
  ASSERT_TRUE(static_cast<const bool>(is_autofilter_return<Deferred>::value)) << "Incorrect identification of Deferred as invalid AutoFilter return type";

  ASSERT_FALSE(static_cast<const bool>(is_autofilter<NonFilterFunctionType0>::value)) << "Trivial function identified as valid";
  ASSERT_FALSE(static_cast<const bool>(is_autofilter<NonFilterFunctionType1>::value)) << "Function with invalid first argument identified as valid";
  ASSERT_FALSE(static_cast<const bool>(is_autofilter<NonFilterFunctionType2>::value)) << "Function with invalid second argument identified as valid";
  ASSERT_FALSE(static_cast<const bool>(is_autofilter<NonFilterFunctionType3>::value)) << "Function with invalid return type identified as valid";

  ASSERT_TRUE(static_cast<const bool>(is_autofilter<FilterFunctionType>::value)) << "Valid AutoFilter function identified as invalid";
}

TEST_F(ArgumentTypeTest, TestAutoIn) {
  AutoRequired<AutoPacketFactory> factory;
  std::shared_ptr<AutoPacket> packet = factory->NewPacket();
  packet->Decorate(Argument<0>(1));
  auto_in<Argument<0>> in(packet);
  ASSERT_TRUE(in.is_input) << "Incorrect orientation";
  ASSERT_TRUE(in.is_output) << "Incorrect orientation";
  ASSERT_EQ(1, in->i) << "Incorrect initialization";

  // Base Cast
  const Argument<0>& base_in = in;
  ASSERT_EQ(1, base_in.i) << "Incorrect base cast";

  // Shared Cast
  std::shared_ptr<const Argument<0>> shared_in = in;
  ASSERT_EQ(1, shared_in->i) << "Incorrect base cast";

  // Deduced Type
  auto_arg<const Argument<0>&> arg(packet);
  shared_in = arg;
  ASSERT_EQ(3, in.use_count()) << "AutoPacket + in + arg == 3";
}

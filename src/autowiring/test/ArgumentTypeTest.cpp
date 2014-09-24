// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "var_logic.h"
#include <autowiring/auto_arg.h>
#include <autowiring/has_autofilter.h>

class ArgumentTypeTest:
public testing::Test
{
public:
  ArgumentTypeTest(void) {
    // All decorator tests must run from an initiated context
    AutoCurrentContext()->Initiate();
  }
};

TEST_F(ArgumentTypeTest, constexpr_in_template) {
  ASSERT_TRUE(var_or<bool>(true)) << "Single argument should be invariant";
  ASSERT_FALSE(var_or<bool>(false)) << "Single argument should be invariant";
  ASSERT_TRUE(static_cast<const bool>(std::integral_constant<bool, var_or(true,false)>::value)) << "True || False == True";
  ASSERT_TRUE(var_and<bool>(true)) << "Single argument should be invariant";
  ASSERT_FALSE(var_and<bool>(false)) << "Single argument should be invariant";
  ASSERT_FALSE(static_cast<const bool>(std::integral_constant<bool, var_and(true,false)>::value)) << "True && False == False";
}

template<int N>
class Argument {
public:
  int i;
  Argument(int j = N) : i(j) {}
};

void FilterFunction(const Argument<0>& typeIn, auto_out<Argument<1>> typeOut) {
  typeOut->i += 1 + typeIn.i;
}

typedef Argument<0> copied_in;
typedef const Argument<0> copied_in_const;
typedef const Argument<0>& required_in;
typedef std::shared_ptr<const Argument<0>> required_in_shared;
typedef auto_in<Argument<0>> fundamental_in;
typedef optional_ptr<Argument<0>> optional_in_shared;
typedef Argument<0>& required_out;
typedef std::unique_ptr<Argument<0>, std::function<void(Argument<0>*)>> optional_out_shared;
typedef auto_out<Argument<0>> fundamental_out;

TEST_F(ArgumentTypeTest, AutoFilterTemplateTests) {
  ASSERT_TRUE(auto_arg<copied_in>::is_input) << "Should be input";
  ASSERT_TRUE(auto_arg<copied_in_const>::is_input) << "Should be input";
  ASSERT_TRUE(auto_arg<required_in>::is_input) << "Should be input";

  ASSERT_TRUE(auto_arg<required_in_shared>::is_input) << "Should be input";
  ASSERT_TRUE(auto_arg<required_in_shared>::is_shared) << "Input is a shared ptr";

  ASSERT_TRUE(auto_arg<fundamental_in>::is_input) << "Should be input";
  ASSERT_TRUE(auto_arg<fundamental_in>::is_shared) << "Input is a shared ptr";
  ASSERT_FALSE(auto_arg<fundamental_in>::is_optional) << "Input is not an optional input";

  ASSERT_TRUE(auto_arg<optional_in_shared>::is_input) << "Should be input";
  ASSERT_TRUE(auto_arg<optional_in_shared>::is_shared) << "Input is a shared ptr";
  ASSERT_TRUE(auto_arg<optional_in_shared>::is_optional) << "Input is not an optional input";

  ASSERT_FALSE(auto_arg<required_out>::is_input) << "Should be input";
  ASSERT_FALSE(auto_arg<required_out>::is_shared) << "Input is a shared ptr";
  ASSERT_FALSE(auto_arg<required_out>::is_optional) << "Input is not an optional input";

  ASSERT_FALSE(auto_arg<optional_out_shared>::is_input) << "Should be input";
  ASSERT_TRUE(auto_arg<optional_out_shared>::is_shared) << "Input is a shared ptr";
  ASSERT_TRUE(auto_arg<optional_out_shared>::is_optional) << "Input is not an optional input";

  ASSERT_FALSE(auto_arg<fundamental_out>::is_input) << "Should be input";
  ASSERT_TRUE(auto_arg<fundamental_out>::is_shared) << "Input is a shared ptr";
  ASSERT_TRUE(auto_arg<fundamental_out>::is_optional) << "Input is not an optional input";
}

TEST_F(ArgumentTypeTest, TestAutoIn) {
  AutoRequired<AutoPacketFactory> factory;
  std::shared_ptr<AutoPacket> packet = factory->NewPacket();
  packet->Decorate(Argument<0>(1));
  auto_in<Argument<0>> in(packet, typeid(void));
  ASSERT_TRUE(in.is_input) << "Incorrect orientation";
  ASSERT_FALSE(in.is_output) << "Incorrect orientation";
  ASSERT_EQ(1, in->i) << "Incorrect initialization";

  // Base Cast
  {
    const Argument<0>& base_in = in;
    ASSERT_EQ(1, base_in.i) << "Incorrect base cast";
  }

  // Shared Cast
  {
    std::shared_ptr<const Argument<0>> shared_in = in;
    ASSERT_EQ(1, shared_in->i) << "Incorrect base cast";
  }

  // Deduced Type
  auto_arg<const Argument<0>&> arg(packet);
  ASSERT_EQ(3, in.use_count()) << "AutoPacket + in + arg == 3";
}

TEST_F(ArgumentTypeTest, TestAutoOut) {
  AutoRequired<AutoPacketFactory> factory;
  std::shared_ptr<AutoPacket> packet = factory->NewPacket();
  {
    auto_out<Argument<0>> out(packet, typeid(void));
    ASSERT_FALSE(out.is_input) << "Incorrect orientation";
    ASSERT_TRUE(out.is_output) << "Incorrect orientation";

    out->i = 1;

    // Copy
    auto_out<Argument<0>> out1(out);

    // Assign
    auto_out<Argument<0>> out2;
    out2 = out1;
  }

  const Argument<0>* arg = nullptr;
  ASSERT_TRUE(packet->Get(arg)) << "Missing output";
  ASSERT_EQ(1, arg->i) << "Output was not copied";
}

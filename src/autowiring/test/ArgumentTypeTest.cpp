// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/var_logic.h>
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
  ASSERT_FALSE(var_or<>::value) << "False is idempotent with respect to Or";
  ASSERT_TRUE(var_or<true>::value) << "Single argument should be invariant";
  ASSERT_FALSE(var_or<false>::value) << "Single argument should be invariant";
  ASSERT_TRUE(static_cast<const bool>(var_or<true,false>::value)) << "True || False == True";
  ASSERT_TRUE(var_and<>::value) << "True is idempotent with respect to Or";
  ASSERT_TRUE(var_and<true>::value) << "Single argument should be invariant";
  ASSERT_FALSE(var_and<false>::value) << "Single argument should be invariant";
  ASSERT_FALSE(static_cast<const bool>(var_and<true,false>::value)) << "True && False == False";
}

template<int N>
class Argument {
public:
  int i;
  Argument(int j = N) : i(j) {}
};

typedef Argument<0> copied_in;
typedef const Argument<0> copied_in_const;
typedef const Argument<0>& required_in;
typedef std::shared_ptr<const Argument<0>> required_in_shared;
typedef auto_in<Argument<0>> fundamental_in;
typedef Argument<0>& required_out;
typedef auto_out<Argument<0>> fundamental_out;

TEST_F(ArgumentTypeTest, AutoFilterTemplateTests) {
  ASSERT_TRUE(auto_arg<copied_in>::is_input) << "Should be input";
  ASSERT_TRUE(auto_arg<copied_in_const>::is_input) << "Should be input";
  ASSERT_TRUE(auto_arg<required_in>::is_input) << "Should be input";

  ASSERT_TRUE(auto_arg<required_in_shared>::is_input) << "Should be input";
  ASSERT_TRUE(auto_arg<required_in_shared>::is_shared) << "Input is a shared ptr";

  ASSERT_TRUE(auto_arg<fundamental_in>::is_input) << "Should be input";
  ASSERT_FALSE(auto_arg<fundamental_in>::is_shared) << "Input is not explicitly a shared ptr";

  ASSERT_FALSE(auto_arg<required_out>::is_input) << "Should be output";
  ASSERT_FALSE(auto_arg<required_out>::is_shared) << "Output is a shared ptr";

  ASSERT_FALSE(auto_arg<fundamental_out>::is_input) << "Should be output";
  ASSERT_FALSE(auto_arg<fundamental_out>::is_shared) << "Output is a shared ptr";
}

TEST_F(ArgumentTypeTest, TestAutoIn) {
  AutoRequired<AutoPacketFactory> factory;
  std::shared_ptr<AutoPacket> packet = factory->NewPacket();
  packet->Decorate(Argument<0>(1));

  typedef auto_arg<const Argument<0>> t_arg;
  typedef auto_arg<std::shared_ptr<const Argument<0>>> t_argShared;

  ASSERT_TRUE(t_arg::is_input) << "Incorrect orientation";
  ASSERT_FALSE(t_arg::is_output) << "Incorrect orientation";

  // Base Cast
  {
    const auto& in = t_arg::arg(*packet);
    ASSERT_EQ(1, in.i) << "Incorrect initialization";
  }

  // Shared Cast
  {
    auto shared_in = t_argShared::arg(*packet);
    ASSERT_EQ(1, shared_in->i) << "Incorrect shared cast";
  }

  // Deduced Type
  const auto& arg = t_argShared::arg(*packet);
  ASSERT_EQ(1UL, arg.use_count()) << "AutoPacket should store exactly one shared pointer reference to a decorated entry";
}

TEST_F(ArgumentTypeTest, TestAutoOut) {
  AutoRequired<AutoPacketFactory> factory;
  std::shared_ptr<AutoPacket> packet = factory->NewPacket();

  std::shared_ptr<Argument<0>> a0;
  {
    typedef auto_arg<Argument<0>&> t_argType;
    std::shared_ptr<Argument<0>> out = t_argType::arg(*packet);
    ASSERT_FALSE(t_argType::is_input) << "Incorrect orientation";
    ASSERT_TRUE(t_argType::is_output) << "Incorrect orientation";

    // Implicit commitment to output
    out->i = 1;

    // Normally this happens in CallExtractor, but we will do it here by hand
    packet->Decorate(out);

    // Track the shared pointer in order to ensure no copying takes place
    a0 = out;
  }

  const Argument<0>* arg = nullptr;
  ASSERT_TRUE(packet->Get(arg)) << "Missing output";
  ASSERT_EQ(a0, *packet->GetShared<Argument<0>>()) << "Shared pointer copied incorrectly";
  ASSERT_EQ(1, arg->i) << "Output was not copied";
}

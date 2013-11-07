#include "stdafx.h"
#include "AutoOutputTest.h"
#include "AutoPacketFactory.h"
#include "TestFixtures/Decoration.h"

class SimpleAutoOut {
public:
  void AutoFilter(const Decoration<0>& decoration, auto_output<Decoration<1>> out1) {

  }
};

TEST_F(AutoOutputTest, VerifySimpleInput) {

}
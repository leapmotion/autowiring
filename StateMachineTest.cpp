// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "StateMachine.h"
#include "StateMachineTest.h"

using namespace std;


TEST_F(StateMachineTest, SimpleStates) {
  
  enum class states {BLUE, GREEN, RED, BAD_STATE};
  
  StateMachine<states,int> machine((states::BLUE));
  
  auto trans = [](int i){
    switch (i){
      case 0:
        return states::BLUE;
        break;
      case 1:
        return states::GREEN;
        break;
      case 2:
        return states::RED;
        break;
    }
    return states::BAD_STATE;
  };
  
  machine.addTransition(states::BLUE, trans);
  machine.addTransition(states::GREEN, trans);
  machine.addTransition(states::RED, trans);
  
  EXPECT_EQ(states::BLUE, machine.getState());
  
  machine.transition(2);
  
  EXPECT_EQ(states::RED, machine.getState());

  machine.transition(1);
  
  EXPECT_EQ(states::GREEN, machine.getState());
  
}



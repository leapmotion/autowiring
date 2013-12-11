// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H
#include <map>

template<typename T_enum, typename T_input>
struct TransitionObject {
  TransitionObject(T_enum startState):
    startState(startState)
  {}

private:
  T_enum startState;

public:
  struct LhsWithTransition {
    LhsWithTransition(TransitionObject& owner, T_enum lhs, T_input input):
      owner(owner),
      lhs(lhs),
      input(input)
    {}

    TransitionObject& owner;
    T_enum lhs;
    T_input input;

    TransitionObject&& operator<<(T_enum rhs) {
      // TODO:  Inert the lhs, input, rhs trio into our owning transition object
      return std::move(owner);
    }
  };

  struct LhsState {
    LhsState(TransitionObject& owner, T_enum lhs):
      owner(owner),
      lhs(lhs)
    {}

    TransitionObject& owner;
    T_enum lhs;

    LhsWithTransition operator<<(T_input input) {
      return LhsWithTransition(owner, lhs, input);
    }
  };

  LhsState operator<<(T_enum lhs) {
    return LhsState(*this, lhs);
  }
};

template<typename T_enum, typename T_input>
class StateMachine
{
typedef T_enum (*t_func)(T_input);
public:
  StateMachine(TransitionObject<T_enum, T_input>&& tobj):
    m_currentState(tobj.GetStartState())
  {
  }
  
  virtual ~StateMachine(){};
  
  T_enum transition(T_input input){
    m_currentState = m_transitionMap[m_currentState](input);
    return m_currentState;
  };
  
  void addTransition(T_enum state, t_func lambda){
    m_transitionMap[state] = lambda;
  };
  
  T_enum getState(){
    return m_currentState;
  };

private:
  T_enum m_currentState;
  std::map<T_enum, t_func> m_transitionMap;

protected:
  static TransitionObject<T_enum, T_input> machine(T_enum startState) {
    return TransitionObject<T_enum, T_input>(startState);
  }

  // Operator overloads for behavior introduction:
  template<class T>
  StateMachine& operator+=(void (T::*memFn)()) {
    return *this;
  }

  StateMachine& operator+=(std::function<void()>&& rhs) {
    return *this;
  }
};

#endif
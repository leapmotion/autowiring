// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H
#include "stdafx.h"
#include <map>
#include <utility>
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER

template<typename T_states, typename T_input>
struct TransitionObject {
  TransitionObject(T_states startState):
    startState(startState)
  {}

private:
  T_states startState;

public:
  struct LhsWithTransition {
    LhsWithTransition(TransitionObject& owner, T_states lhs, T_input input):
      owner(owner),
      lhs(lhs),
      input(input)
    {}

    TransitionObject& owner;
    T_states lhs;
    T_input input;

    TransitionObject&& operator<<(T_states rhs) {
      // TODO:  Inert the lhs, input, rhs trio into our owning transition object
      return std::move(owner);
    }
  };

  struct LhsState {
    LhsState(TransitionObject& owner, T_states lhs):
      owner(owner),
      lhs(lhs)
    {}

    TransitionObject& owner;
    T_states lhs;

    LhsWithTransition operator<<(T_input input) {
      return LhsWithTransition(owner, lhs, input);
    }
  };

  LhsState operator<<(T_states lhs) {
    return LhsState(*this, lhs);
  }
};

template<typename T_states, typename T_input>
class StateMachine
{
typedef std::function<T_states(T_input)> t_func;
public:
  StateMachine(TransitionObject<T_states, T_input>&& tobj):
    m_currentState(tobj.GetStartState())
  {
  }
  
  virtual ~StateMachine(){};
  
  T_states transition(T_input input){
    m_currentState = m_transitionMap[m_currentState](input);
    return m_currentState;
  };
  
  void addTransition(T_states state, t_func lambda){
    m_transitionMap[state] = lambda;
  };
  
  T_states getState(){
    return m_currentState;
  };

private:
  T_states m_currentState;
  std::map<T_states, t_func> m_transitionMap;

protected:
  static TransitionObject<T_states, T_input> machine(T_states startState) {
    return TransitionObject<T_states, T_input>(startState);
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
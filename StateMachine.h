// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H
#include "stdafx.h"
#include <map>
#include <utility>
#include <iostream>

#include FUNCTIONAL_HEADER
#include RVALUE_HEADER

template<typename T_states, typename T_input>
struct TransitionObject {
  TransitionObject(T_states startState):
    startState(startState)
  {}

private:
  T_states startState;
  typedef std::pair<T_states,T_input> t_pair;
  typedef std::map<t_pair,T_states> t_transitionMap;
  t_transitionMap m_transitionMap;

public:
  // Accessor methods:
  T_states GetStartState(void) const { return startState; }
  t_transitionMap&& GetTransitions(void) { return std::move(m_transitionMap); }

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
      // TODO:  Insert the lhs, input, rhs trio into our owning transition object
      owner.m_transitionMap[std::make_pair(lhs,input)] = rhs;
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
public:
  StateMachine(TransitionObject<T_states, T_input>&& tobj):
    m_currentState(tobj.GetStartState()),
    m_transitionMap(tobj.GetTransitions())
  {}
  
  virtual ~StateMachine(){};
  
  T_states transition(T_input input){
    m_currentState = m_transitionMap[std::make_pair(m_currentState,input)];
    
    auto behavior_func = m_behaviors.find(m_currentState);
    if (behavior_func != m_behaviors.end()) {
      behavior_func->second();
    }
    
    auto transition_func = m_transitionHandlers.find(input);
    if (transition_func != m_transitionHandlers.end()) {
      transition_func->second();
    }
    
    return m_currentState;
  };
  
  T_states getState(){
    return m_currentState;
  };

private:
  T_states m_currentState;
  typedef std::pair<T_states,T_input> t_pair;
  typedef std::map<t_pair,T_states> t_transitionMap;
  t_transitionMap m_transitionMap;

  typedef std::map<T_states, std::function<void()>> t_behaviors;
  t_behaviors m_behaviors;

  typedef std::map<T_input, std::function<void()>> t_transitionHandlers;
  t_transitionHandlers m_transitionHandlers;
  

protected:
  static TransitionObject<T_states, T_input> machine(T_states startState) {
    return TransitionObject<T_states, T_input>(startState);
  }

  struct BehaviorEntry
  {
    BehaviorEntry(StateMachine* parent_this, std::function<void()>& entry):
      entry(entry),
      p_this(parent_this)
    {}

    std::function<void()>& entry;
    StateMachine* p_this;
    
    // Operator overloads for behavior introduction:
    template<class T>
    void operator,(void (T::*memFn)()) {
      entry = [this,memFn]{
        T* herpderp = reinterpret_cast<T*>(p_this);
        (herpderp->*memFn)();
      };
    }

    void operator,(std::function<void()>&& rhs) {
      entry = rhs;
    }
  };
  
  struct TransitionEntry
  {
    TransitionEntry(StateMachine* parent_this, std::function<void()>& entry):
      entry(entry),
      p_this(parent_this)
    {}

    std::function<void()>& entry;
    StateMachine* p_this;
    
    // Operator overloads for behavior introduction:
    template<class T>
    void operator,(void (T::*memFn)()) {
      auto p_this = this->p_this;
      entry = [p_this,memFn]{
        T* herpderp = reinterpret_cast<T*>(p_this);
        (herpderp->*memFn)();
      };
    }

    void operator,(std::function<void()>&& rhs) {
      entry = rhs;
    }
  };

  // Operator overloads for behavior introduction:
  BehaviorEntry operator+=(T_states state) {
    return BehaviorEntry(this, m_behaviors[state]);
  }

  TransitionEntry operator+=(T_input input) {
    return TransitionEntry(this, m_transitionHandlers[input]);
  }
};

#endif
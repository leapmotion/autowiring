// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H
#include "stdafx.h"
#include <map>
#include <utility>
#include <iostream>
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER

/// <summary>
/// Represents state transitions when parsing input to StateMachine constructor
/// </summary>
/// <param name="T_State"> An enum of states in the machine</param>
/// <param name="T_Event"> An enum of events that change state</param>
/// <remarks>
/// Used by StateMachine. You shouldn't need to interact with this directly
/// </remarks>
template<typename T_State, typename T_Event>
struct TransitionObject {
  TransitionObject(T_State startState):
    startState(startState)
  {}

private:
  typedef std::pair<T_State,T_Event> t_pair;
  typedef std::map<t_pair,T_State> t_transitionMap;
  
  t_transitionMap m_transitionMap;
  T_State startState;

public:
  T_State GetStartState(void) const { return startState; }
  t_transitionMap& GetTransitions(void) { return m_transitionMap; }

  struct LhsWithTransition {
    LhsWithTransition(TransitionObject& owner, T_State lhs, T_Event event):
      owner(owner),
      lhs(lhs),
      event(event)
    {}

    TransitionObject& owner;
    T_State lhs;
    T_Event event;

    TransitionObject&& operator<<(T_State rhs) {
      owner.m_transitionMap[std::make_pair(lhs,event)] = rhs;
      return std::move(owner);
    }
  };

  struct LhsState {
    LhsState(TransitionObject& owner, T_State lhs):
      owner(owner),
      lhs(lhs)
    {}

    TransitionObject& owner;
    T_State lhs;

    LhsWithTransition operator<<(T_Event event) {
      return LhsWithTransition(owner, lhs, event);
    }
  };

  LhsState operator<<(T_State lhs) {
    return LhsState(*this, lhs);
  }
};

/// <summary>
/// Inherit from this class implement a state machine
/// </summary>
/// <param name="T_State"> An enum of states in the machine</param>
/// <param name="T_Event"> An enum of events that change state</param>
/// <remarks>
/// See StateMachineTest.cpp for an example of how to use
/// </remarks>
template<typename T_State, typename T_Event>
struct StateMachine {
  StateMachine(TransitionObject<T_State, T_Event>&& tobj):
    m_currentState(tobj.GetStartState()),
    m_transitionMap(std::move(tobj.GetTransitions()))
  {}
  virtual ~StateMachine(){};
  
private:
  typedef std::pair<T_State,T_Event> t_pair;
  typedef std::map<t_pair,T_State> t_transitionMap;
  typedef std::map<T_State, std::function<void()>> t_stateBehaviors;
  typedef std::map<T_Event, std::function<void()>> t_transitionHandlers;
  
  t_transitionMap m_transitionMap;
  t_stateBehaviors m_stateBehaviors;
  t_transitionHandlers m_transitionHandlers;
  T_State m_currentState;
  
protected:
  static TransitionObject<T_State, T_Event> initializeStateMachine(T_State startState) {
    return TransitionObject<T_State, T_Event>(startState);
  }

  struct BehaviorEntry
  {
    BehaviorEntry(StateMachine* parent_this, std::function<void()>& entry):
      entry(entry),
      p_this(parent_this)
    {}

    std::function<void()>& entry;
    StateMachine* p_this;
    
    template<class T>
    void operator,(void (T::*memFn)()) {
      auto p_this = static_cast<T*>(this->p_this);
      entry = [p_this,memFn]{
        (p_this->*memFn)();
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
      auto p_this = static_cast<T*>(this->p_this);
      entry = [p_this,memFn]{
        (p_this->*memFn)();
      };
    }

    void operator,(std::function<void()>&& rhs) {
      entry = rhs;
    }
  };

  // Operator overloads for behavior introduction:
  BehaviorEntry operator+=(T_State state) {
    return BehaviorEntry(this, m_stateBehaviors[state]);
  }

  TransitionEntry operator+=(T_Event event) {
    return TransitionEntry(this, m_transitionHandlers[event]);
  }
  
public:
  /// <summary>
  /// Transition to the next state based on the current state and input event
  /// Returns the new state and sets it's new current state
  /// </summary>
  /// <param name="event">Input to the state machine. May change the current state</param>
  /// <remarks>
  /// This is the only function you need to use to control the state machine
  /// after initialization
  /// </remarks>
  T_State transition(T_Event event){
    m_currentState = m_transitionMap[std::make_pair(m_currentState,event)];
    
    auto behavior_func = m_stateBehaviors.find(m_currentState);
    if (behavior_func != m_stateBehaviors.end()) {
      behavior_func->second();
    }
    
    auto transition_func = m_transitionHandlers.find(event);
    if (transition_func != m_transitionHandlers.end()) {
      transition_func->second();
    }
    
    return m_currentState;
  };
  
  T_State getState(){
    return m_currentState;
  };
};

#endif
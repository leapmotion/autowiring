// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H
#include <map>

template<typename T_enum, typename T_input>
class StateMachine
{
typedef T_enum (*t_func)(T_input);
public:
  StateMachine(T_enum startState){
    m_currentState = startState;
  };
  
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
};

#endif
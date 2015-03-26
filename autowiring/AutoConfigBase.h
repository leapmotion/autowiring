// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ContextMember.h"
#include "auto_signal.h"

#include <string>
#include <C++11/cpp11.h>
#include TYPE_INDEX_HEADER

struct AnySharedPointer;

/// \internal
/// <summary>
/// Utility base type & interface for configuration members
/// </summary>
class AutoConfigVarBase : public ContextMember
{
public:
  AutoConfigVarBase(const std::type_info& tiName, bool configured = false);
  void AutoInit();

  // Key used to identify this config value
  const std::string m_key;

  bool IsConfigured() const { return m_isConfigured; }
  bool IsInherited() const { return m_parentRegistration != nullptr; }

  typedef autowiring::signal<void(const AutoConfigVarBase& val)> t_OnChangedSignal;
  t_OnChangedSignal onChangedSignal;

  virtual void Get(void* pValue) const = 0;
  virtual void Set(const void* pValue) = 0;
  virtual void SetParsed(const std::string& value) = 0;

protected:
  bool m_isConfigured;
  t_OnChangedSignal::t_registration* m_parentRegistration;
};

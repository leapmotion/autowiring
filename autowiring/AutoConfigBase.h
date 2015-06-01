// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ContextMember.h"
#include "auto_signal.h"

#include <string>
#include "C++11/cpp11.h"
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

  // True if this config was set at all
  bool IsConfigured() const { return m_isConfigured; }
  bool IsInherited() const { return (bool)m_parentRegistration; }
  //True if the config was set from within this context (isn't inherited)
  bool IsLocal() const { return IsConfigured() && !IsInherited(); }

  typedef autowiring::signal<void(const AutoConfigVarBase& val)> t_OnChangedSignal;
  t_OnChangedSignal onChangedSignal;

  virtual void Get(void* pValue) const = 0;
  virtual void Set(const void* pValue) = 0;
  virtual void SetParsed(const std::string& value) = 0;

protected:
  void OnSetLocally();

  bool m_isConfigured;
  autowiring::registration_t m_parentRegistration;
};

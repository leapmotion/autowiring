// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ConfigManager.h"

using namespace autowiring;

ConfigManager::ConfigManager(void) :
  m_empty("")
{}

void ConfigManager::Register(void* pObj, const config_descriptor& desc) {
  std::lock_guard<autowiring::spin_lock> lk(m_lock);
  for (const auto& field : desc.fields) {
    Entry& entry = m_config[field.second.name];
    entry.attached.push_back({
      &field.second,
      static_cast<uint8_t*>(pObj) + field.second.offset
    });

    if (entry.value)
      entry.attached.back().field->marshaller->unmarshal(
        entry.attached.back().pObj,
        entry.value->c_str()
      );
  }
}

const std::string& ConfigManager::Get(std::string name) const {
  auto q = m_config.find(name);
  if (q == m_config.end())
    return m_empty;

  return *q->second.value;
}

void ConfigManager::Set(std::string&& name, std::string&& value) {
  std::lock_guard<autowiring::spin_lock> lk(m_lock);
  auto q = m_config.find(name);
  if (q == m_config.end())
    q = m_config.insert(q, { std::move(name), {} });

  Entry& entry = q->second;
  entry.value = std::move(value);

  for (Entry::Attachment& attachment : entry.attached)
    attachment.field->marshaller->unmarshal(
      attachment.pObj,
      entry.value->c_str()
    );
}

// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ConfigManager.h"
#include "at_exit.h"
#include <mutex>
#include <unordered_set>

using namespace autowiring;

struct entry {};

ConfigManager::ConfigManager(void) :
  m_empty("")
{}

void ConfigManager::Register(void* pObj, const config_descriptor& desc) {
  // Deferred call registrations:
  std::vector<std::pair<internal::WhenWatcher*, const ConfigManager::metadata_value*>> deferred;

  {
    std::lock_guard<autowiring::spin_lock> lk(m_lock);
    for (const auto& field : desc.fields) {
      // We need to attach this field to the corresponding configuration entry,
      // we perform the attachment by name
      Entry& entry = m_config[field.second.name];
      entry.attached.emplace_back(
        field.second,
        static_cast<uint8_t*>(pObj) + field.second.offset
      );

      // Default value generation:
      if (entry.value)
        entry.attached.back().configField->marshaller->unmarshal(
          entry.attached.back().pField,
          entry.value->c_str()
        );

      // Deferred signalling on all watcher collections.  This part causes When handlers to be invoked.
      for (const auto& m : field.second.metadata->get_list()) {
        auto q = metadata_collection.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(m->id()),
          std::forward_as_tuple(&field.second, m->value())
        );

        auto er = watchers.equal_range(m->id());
        for (auto r = er.first; r != er.second; r++)
          deferred.emplace_back(r->second.get(), &q->second);
      }
    }
  }

  for (auto& e : deferred)
    (*e.first)(*e.second->field, e.second->metadata);
}

void ConfigManager::WhenInternal(std::unique_ptr<internal::WhenWatcher>&& watcher) {
  std::vector<metadata_value> matched;

  const internal::WhenWatcher& w = *watcher.get();
  {
    // Add the watcher to the colleciton, and also make a list of all existing metadata
    // that will need to be passed to the watcher:
    std::lock_guard<autowiring::spin_lock> lk(m_lock);
    watchers.emplace(
      watcher->id,
      std::move(watcher)
    );
    auto r = metadata_collection.equal_range(w.id);
    for (auto q = r.first; q != r.second; q++)
      matched.emplace_back(q->second.field, q->second.metadata);
  }

  for (metadata_value& match : matched)
    w(*match.field, match.metadata);
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
    q = m_config.emplace(std::move(name), Entry{}).first;

  Entry& entry = q->second;
  entry.value = std::move(value);

  for (Entry::Attachment& attachment : entry.attached)
    attachment.configField->marshaller->unmarshal(
      attachment.pField,
      entry.value->c_str()
    );
}

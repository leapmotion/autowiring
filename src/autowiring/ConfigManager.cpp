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
  std::vector<std::pair<WhenWatcher*, const config_event*>> deferred;

  {
    std::lock_guard<autowiring::spin_lock> lk(m_lock);

    // This goes through all of the fields in the user-specified descriptor.  These fields describe offsets
    // in pObj, and each one could require us to either populate offsets in pObj with values that we are
    // keeping in m_config, or it could require us to update m_config based on metadata described by these
    // fields.
    for (const auto& field : desc.fields) {
      const config_field& field_desc = field.second;

      // We need to attach this field to the corresponding configuration entry, we perform the attachment by
      // name.  After we have our entry we can fill it or use it to fill other objects as needed.
      Entry& entry = m_config[field_desc.name];
      entry.attached.emplace_back(
        field_desc,
        static_cast<uint8_t*>(pObj) + field_desc.offset
      );

      Entry::Attachment& attachment = entry.attached.back();
      if (entry.value)
        // We have a value already.  Force the attachment's field to take on the value we
        // are storing locally.
        attachment.configField->marshaller->unmarshal(attachment.pField, entry.value->c_str());
      else if (field_desc.default_value) {
        // Descriptor provides a default value, and we do not have a value ourselves in our own
        // local config store.  In this case, we take the default value, and unconditionally overwrite
        // the value currently present on the object.
        entry.value = attachment.configField->marshaller->marshal(attachment.configField->default_value.ptr());
        field_desc.marshaller->copy(attachment.pField, attachment.configField->default_value.ptr());
      }
      ///else
        // In this case, no value stored locally, no default value either.  We can't really do much
        // except to default this entry.

      // Deferred signalling on all watcher collections.  This part causes When handlers to be invoked.
      const std::vector<const metadata_base*>& all_metadata = attachment.bound_metadata->get_list();
      for (const auto& m : all_metadata) {
        auto q = metadata_collection.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(m->id()),
          std::forward_as_tuple(pObj, &desc, &field_desc, m)
        );

        // Here's where we actually figure out what watchers need invocation:
        auto er = watchers.equal_range(m->id());
        for (auto r = er.first; r != er.second; r++)
          deferred.emplace_back(r->second.get(), &q->second);
      }
    }
  }

  for (auto& e : deferred)
    e.first->OnMetadata(*e.second);
}

void ConfigManager::WhenInternal(const std::shared_ptr<WhenWatcher>& watcher) {
  std::vector<config_event*> matched;

  WhenWatcher& w = *watcher;

  {
    // Add the watcher to the colleciton, and also make a list of all existing metadata
    // that will need to be passed to the watcher:
    std::lock_guard<autowiring::spin_lock> lk(m_lock);
    watchers.emplace(watcher->id, watcher);
    auto r = metadata_collection.equal_range(w.id);
    for (auto q = r.first; q != r.second; q++) {
      matched.emplace_back(&q->second);
    }
  }

  for (const config_event* match : matched)
    w.OnMetadata(*match);
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

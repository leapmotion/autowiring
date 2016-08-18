// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ConfigManager.h"
#include "ConfigRegistry.h"
#include "at_exit.h"
#include <mutex>
#include <unordered_set>

using namespace autowiring;

struct entry {};

ConfigManager::ConfigManager(std::shared_ptr<ConfigManager> pParent) :
  m_pParent(pParent)
{
  if (pParent)
    // No defaulting work needed, we can end here
    return;

  // The root configuration manager will prepopulate itself with a total traversal of all elements
  for (const config_descriptor& descriptor : config_registry{})
    for (const auto& field_entry : descriptor.fields) {
      const config_field& field = field_entry.second;

      Entry& entry = m_config[field_entry.first];

      // If there's a default value on this field, try to marshal it, otherwise the field stays blank
      if (field.default_value)
        entry.value = field.marshaller->marshal(field.default_value.ptr());
    }
}

void ConfigManager::Clear(void) {
  // Clear any recursive handlers created by binding to a default value
  for (auto& q : m_config) {
    Entry& entry = q.second;
    entry.reg.clear();
  }

  // Unlink from the parent
  m_pParent = {};
}

void ConfigManager::Register(std::shared_ptr<void> pObj, const config_descriptor& desc) {
  // Deferred call registrations:
  std::vector<std::pair<WhenWatcher*, const config_event*>> deferred;

  // Byte object base, held for convenience
  uint8_t* pObjBase = static_cast<uint8_t*>(pObj.get());

  {
    // This goes through all of the fields in the user-specified descriptor.  These fields describe offsets
    // in pObj, and each one could require us to either populate offsets in pObj with values that we are
    // keeping in m_config, or it could require us to update m_config based on metadata described by these
    // fields.
    for (const auto& field : desc.fields) {
      const config_field& field_desc = field.second;
      void* pField = pObjBase + field_desc.offset;

      // We need to attach this field to the corresponding configuration entry, we perform the attachment by
      // name.  After we have our entry we can fill it or use it to fill other objects as needed.
      Entry& entry = GetEntryUnsafe(field_desc.name);
      metadata_pack_base* pBoundMetadata;
      {
        entry.attached.emplace_back(field_desc, pField);
        pBoundMetadata = entry.attached.back().bound_metadata.get();
      }

      // Register a change handler with this object.  Also hold a reference to the object's shared
      // pointer, we want to prevent the enclosing object's destruction as long as the entry is
      // invoking.
      const marshaller_base* marshaller = field_desc.marshaller;
      entry.onChanged << [pObj, marshaller, pField, &entry] {
        // register the handler
        entry.onChanged += [pObj, marshaller, pField, &entry] {
          marshaller->unmarshal(pField, entry.value.c_str());
        };

        // Take the default value
        marshaller->unmarshal(pField, entry.value.c_str());
      };

      // Deferred signalling on all watcher collections.  This part causes When handlers to be invoked.
      const std::vector<const metadata_base*>& all_metadata = pBoundMetadata->get_list();
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

ConfigManager::Entry& ConfigManager::GetEntryUnsafe(const std::string& name) {
  auto q = m_config.find(name);
  if (q != m_config.end())
    return q->second;

  auto& configEntry = m_config[name];
  if (!m_pParent)
    // No parent, no recursive registration possible
    return configEntry;

  // Need to ensure this value is correctly updated on parent value modifications
  auto& parentEntry = m_pParent->GetEntry(name);

  // Need to register ourselves and take a value in the sequence order of the parent:
  auto self = shared_from_this();
  parentEntry.onChanged << [self, &configEntry, &parentEntry] {
    // Now we can perform a registration while nothing else is going on
    configEntry.reg = parentEntry.onChanged += [self, &configEntry, &parentEntry] {
      // Double-check that we are still registered.  Technically we can become
      // unregistered while a signal is underway, which means that the entity
      // performing the unregistration has kicked off a new call to setValue
      if (!configEntry.reg)
        return;

      // Take a copy to our local total order, notify everyone:
      configEntry.onChanged.invoke(
        [&] (std::string&& valueCopy) {
          configEntry.value = std::move(valueCopy);
        },
        std::string{ parentEntry.value }
      );
      configEntry.onChanged();
    };

    // And we want to take a snapshot of the value and store it in ourselves:
    configEntry.onChanged.invoke(
      [&] (std::string&& valueCopy) {
        configEntry.value = std::move(valueCopy);
      },
      std::string{ parentEntry.value }
    );
  };
  return configEntry;
}

ConfigManager::Entry& ConfigManager::GetEntry(const std::string& name) {
  std::lock_guard<autowiring::spin_lock> lk(m_lock);
  return GetEntryUnsafe(name);
}

std::string ConfigManager::Get(const std::string& name) const {
  const Entry* pEntry;
  {
    std::lock_guard<autowiring::spin_lock> lk(m_lock);
    auto q = m_config.find(name);
    pEntry = q == m_config.end() ? nullptr : &q->second;
  }

  if (!pEntry)
    return m_pParent ? m_pParent->Get(name) : "";

  // Lock the signal to prevent the value from being changed:
  std::lock_guard<signal<void()>> lk(pEntry->onChanged);
  return pEntry->value;
}

void ConfigManager::Set(const std::string& name, std::string value) {
  Entry* pEntry;

  {
    std::lock_guard<autowiring::spin_lock> lk(m_lock);
    auto q = m_config.find(name);
    if (q == m_config.end())
      q = m_config.emplace(name, Entry{}).first;

    pEntry = &q->second;

    // If we are registered on this level, we need to unregister with the parent,
    // because this level is overriding the parent's value
    if (pEntry->reg)
      pEntry->reg.clear();
  }

  // Sequenced value assignment:
  pEntry->onChanged.invoke(
    [pEntry] (std::string&& rhs) {
      pEntry->value = std::move(rhs);
    },
    std::move(value)
  );
  pEntry->onChanged();
}

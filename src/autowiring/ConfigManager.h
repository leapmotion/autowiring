// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_id.h"
#include "config_descriptor.h"
#include "Decompose.h"
#include "optional.h"
#include "spin_lock.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace autowiring {
  namespace internal {

    struct WhenWatcher {
      WhenWatcher(auto_id id) :
        id(id)
      {}

      auto_id id;

      virtual void operator()(const config_field& field, const void* pObj) const = 0;
    };

    template<typename Fn, typename = void>
    struct WhenWatcherT;

    template<typename Fn, typename M>
    struct WhenWatcherT<Fn, void (Fn::*)(const config_field&, const M&) const> :
      WhenWatcher
    {
      WhenWatcherT(Fn&& fn) :
        WhenWatcher(auto_id_t<M>{}),
        fn(std::move(fn))
      {}

      Fn fn;

      void operator()(const config_field& field, const void* pMetadata) const override { fn(field, *static_cast<const M*>(pMetadata)); }
    };

    template<typename Fn, typename M>
    struct WhenWatcherT<Fn, void (Fn::*)(const M&) const> :
      WhenWatcher
    {
      WhenWatcherT(Fn&& fn) :
        WhenWatcher(auto_id_t<M>{}),
        fn(std::move(fn))
      {}

      Fn fn;

      void operator()(const config_field&, const void* pMetadata) const override { fn(*static_cast<const M*>(pMetadata)); }
    };
  }

  class ConfigManager {
  public:
    ConfigManager(void);

  private:
    spin_lock m_lock;

    // Empty string sentry, used when a miss occurs
    const std::string m_empty;

    // A single entry, which has a string representation part paired
    // with a pointer to the value part
    struct Entry {
      Entry(void) = default;
      Entry(const Entry&) = delete;
      Entry(Entry&& rhs) :
        value(std::move(rhs.value)),
        attached(std::move(rhs.attached))
      {}

      // The value held here
      optional<std::string> value;

      struct Attachment {
        Attachment(void) = default;
        Attachment(const config_field& configField, void* pField) :
          configField(&configField),
          pField(pField)
        {
          bound_metadata = configField.metadata->clone();
          bound_metadata->bind(configField, pField);
        }
        Attachment(const Attachment&) = delete;
        Attachment(Attachment&& rhs) :
          configField(rhs.configField),
          bound_metadata(std::move(rhs.bound_metadata)),
          pField(rhs.pField)
        {}

        // Field proper
        const config_field* configField = nullptr;

        // Object proper
        void* pField = nullptr;

        // Metadata copy, taken from the field, required because we bind all of this metadata
        std::unique_ptr<metadata_pack_base> bound_metadata;

        Attachment& operator=(const Attachment& rhs) = delete;
        Attachment& operator=(Attachment&& rhs) {
          configField = rhs.configField;
          bound_metadata = std::move(rhs.bound_metadata);
          pField = rhs.pField;
          return *this;
        }
      };

      // Fields attached on this entry:
      std::vector<Attachment> attached;
    };

    // All configuration values
    std::unordered_map<std::string, Entry> m_config;

    // Cache of metadata values to their fields:
    struct metadata_value {
      metadata_value(const config_field* field, const void* metadata) :
        field(field),
        metadata(metadata)
      {}

      const config_field* field;
      const void* metadata;
    };
    std::unordered_multimap<auto_id, metadata_value> metadata_collection;

    // All watchers for each individual metadata field:
    std::unordered_multimap<auto_id, std::unique_ptr<internal::WhenWatcher>> watchers;

    // Adds a watcher on a specified type
    void WhenInternal(std::unique_ptr<internal::WhenWatcher>&& watcher);

  public:
    /// <summary>
    /// Registers a new configurable object
    /// </summary>
    /// <param name="pObj">The object that may be configured</param>
    /// <param name="desc">A descriptor for the object</param>
    void Register(void* pObj, const config_descriptor& desc);

    /// <summary>
    /// Invokes the specified callback when a piece of metadata is attached to any object
    /// </summary>
    /// <remarks>
    /// Two callable signatures are allowed.  The first is the full context variant, which
    /// provides the configuration filed.  Here, M is the metadata type:
    ///
    ///  [] (const aw::config_field& field, const M& slider)
    ///
    /// The second is the compact variant, for which only the raw metadata is provided:
    ///
    /// [] (const M& slider)
    /// </remarks>
    template<typename Fn>
    void When(Fn&& fn) {
      WhenInternal(
        std::unique_ptr<internal::WhenWatcher> {
          new internal::WhenWatcherT<
            Fn,
            decltype(&Fn::operator())
          >{ std::forward<Fn>(fn) }
        }
      );
    }

    /// <summary>
    /// Gets the current configuration value from the map
    /// </summary>
    const std::string& Get(std::string name) const;

    /// <summary>
    /// Sets the named config value in the map
    /// </summary>
    void Set(std::string&& name, std::string&& value);
  };
}

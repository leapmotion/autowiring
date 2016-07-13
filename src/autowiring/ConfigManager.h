// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_id.h"
#include "config_descriptor.h"
#include "config_event.h"
#include "Decompose.h"
#include "optional.h"
#include "signal.h"
#include "spin_lock.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace autowiring {
  struct WhenWatcher {
    WhenWatcher(auto_id id) :
      id(id)
    {}

    auto_id id;

    virtual void OnMetadata(const config_event& evt) = 0;
  };

  namespace internal {
    template<typename Fn, typename = void>
    struct WhenWatcherT;

    template<typename Fn, typename M>
    struct WhenWatcherT<Fn, void (Fn::*)(const config_event&, const M&) const> :
      WhenWatcher
    {
      WhenWatcherT(Fn&& fn) :
        WhenWatcher(auto_id_t<M>{}),
        fn(std::move(fn))
      {}

      Fn fn;

      void OnMetadata(const config_event& evt) override {
        fn(evt, *static_cast<const M*>(evt.metadata->value()));
      }
    };
  }

  class ConfigManager :
    public std::enable_shared_from_this<ConfigManager>
  {
  public:
    ConfigManager(void) :
      ConfigManager(nullptr)
    {}

    ConfigManager(std::shared_ptr<ConfigManager> pParent);

  private:
    mutable spin_lock m_lock;

    // Parent configuration, if one exists
    std::shared_ptr<ConfigManager> m_pParent;

    // A single entry, which has a string representation part paired
    // with a pointer to the value part
    struct Entry {
      struct Attachment;

      Entry(void) = default;
      Entry(const Entry&) = delete;
      Entry(Entry&& rhs) :
        value(std::move(rhs.value)),
        attached(std::move(rhs.attached))
      {}

      // Signal asserted when the value changes:
      mutable signal<void()> onChanged;

      // Fields attached on this entry:
      std::vector<Attachment> attached;

      // The value held here
      std::string value;

      // If we are taking on our value from the ancestor, this holds the parent
      // registration object.
      registration_t reg;

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
    };

    // All configuration values
    std::unordered_map<std::string, Entry> m_config;

    // Cache of metadata values to event types that would be asserted for those values:
    std::unordered_multimap<auto_id, config_event> metadata_collection;

    // All watchers for each individual metadata field:
    std::unordered_multimap<auto_id, std::shared_ptr<WhenWatcher>> watchers;

    // Adds a watcher on a specified type
    void WhenInternal(const std::shared_ptr<WhenWatcher>& watcher);

    // Unsynchronized version of GetEntry
    Entry& GetEntryUnsafe(const std::string& name);

    // Gets an entry out of the configuration map
    Entry& GetEntry(const std::string& name);

  public:
    /// <summary>
    /// Breaks any link this manager may have with its parent
    /// </summary>
    void Clear(void);

    /// <summary>
    /// Registers a new configurable object
    /// </summary>
    /// <param name="pObj">The object that may be configured</param>
    /// <param name="desc">A descriptor for the object</param>
    void Register(std::shared_ptr<void> pObj, const config_descriptor& desc);

    /// <summary>
    /// Invokes the specified callback when a piece of metadata is attached to any object
    /// </summary>
    /// <returns>A shared pointer containing the added watcher</returns>
    /// <remarks><![CDATA[
    /// Two callable signatures are allowed.  The first is the full context variant, which
    /// provides the configuration filed.  Here, M is the metadata type:
    ///
    ///  [] (const aw::config_field& field, const M& slider)
    ///
    /// The second is the compact variant, for which only the raw metadata is provided:
    ///
    /// [] (const M& slider)
    /// ]]></remarks>
    template<typename Fn>
    std::shared_ptr<WhenWatcher> WhenFn(Fn&& fn) {
      auto retVal = std::make_shared<internal::WhenWatcherT<Fn, decltype(&Fn::operator())>>(std::forward<Fn>(fn));
      WhenInternal(retVal);
      return retVal;
    }

    /// <summary>
    /// Identical to WhenFn, except uses a preconstructed WhenWatcher
    /// </summary>
    void When(const std::shared_ptr<WhenWatcher>& watcher) {
      WhenInternal(watcher);
    }

    /// <summary>
    /// Gets the current configuration value from the map
    /// </summary>
    std::string Get(const std::string& name) const;

    /// <summary>
    /// Sets the named config value in the map
    /// </summary>
    void Set(std::string&& name, std::string&& value);

    /// <summary>
    /// Causes this entry to take on the default value in the parent context
    /// </summary>
    void Default(std::string name);
  };
}

#pragma once
#include "auto_id.h"
#include "once.h"
#include "config_descriptor.h"
#include "ConfigManager.h"

namespace autowiring {
  class ConfigWatcherBase :
    public WhenWatcher
  {
  public:
    ConfigWatcherBase(auto_id id) :
      WhenWatcher{ id }
    {}

  protected:
    virtual void OnMetadata(const config_event& evt, autowiring::once& shutdown) = 0;

  public:
    // Base class overrides:
    void OnMetadata(const config_event& evt) override;
  };

  /// <summary>
  /// Allows users to receive notifications any time configuration metadata is detected in the context
  /// </summary>
  /// <remarks>
  /// Metadata comes in from various
  /// </remarks>
  template<typename T>
  class ConfigWatcher:
    public ConfigWatcherBase
  {
  public:
    ConfigWatcher(void) :
      ConfigWatcherBase{ auto_id_t<T>{} }
    {}

  protected:
    void OnMetadata(const config_event& evt, autowiring::once& shutdown) {
      OnMetadata(evt, *static_cast<const T*>(evt.metadata->value()), shutdown);
    }

    /// <summary>
    /// Invoked when the matching metadata type is detected in the context
    /// </summary>
    /// <param name="evt">Information about the added metadata field</param>
    /// <param name="metadata">The metadata value itself</param>
    /// <param name="shutdown">A signal that will be asserted before the metadata value is destroyed</param>
    /// <remarks>
    /// The current context at the time of this call will be the context in which the member was detected
    /// </remarks>
    virtual void OnMetadata(const config_event& evt, const T& metadata, autowiring::once& shutdown) = 0;
  };
}

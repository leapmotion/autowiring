#pragma once

/// <summary>
/// Allows the deserialization of events from an output stream, in order to replay them in-process
/// </summary>
template<class T>
class EventInputStream
{
public:
  static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot instantiate an event input stream on a non-event type");

private:

public:
  /// <summary>
  /// Enables a new event for firing via its identity, as a member of T, for transmission by this class
  /// </summary>
  template<class MemFn>
  void EnableIdentity(MemFn eventIden) {
  }

  /// <summary>
  /// Interprets and fires a SINGLE EVENT from the passed input buffer
  /// </summary>
  /// <returns>
  /// The number of bytes processed from the input buffer
  /// </returns>
  size_t FireSingle(const void* pData, size_t dataSize) const {
    return 0;
  }
};


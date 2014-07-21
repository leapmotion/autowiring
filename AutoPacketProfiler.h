#pragma once
#include CHRONO_HEADER
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP

/// <summary>
/// Profiling instrumentation for the Autowiring subsystem
/// </summary>
class AutoPacketProfiler
{
public:
  AutoPacketProfiler();
  ~AutoPacketProfiler();

private:
  bool m_shouldProfile;

  /// <summary>
  /// Total amount of time spent in any one subscriber
  /// </summary>
  std::unordered_map<std::type_index, std::chrono::nanoseconds> m_totalTime;

public:
  /// <summary>
  /// Flag, set if we should profile
  /// </summary>
  bool ShouldProfile(void) const { return m_shouldProfile; }

  /// <summary>
  /// Profiling callback, invoked by AutoPacket to indicate subscriber invocation time
  /// </summary>
  /// <param name="subscriber">The type_info of the subscriber</param>
  /// <param name="duration">The total duration of the just-made call</param>
  void AddProfilingInformation(const std::type_info& subscriber, std::chrono::nanoseconds duration);
};


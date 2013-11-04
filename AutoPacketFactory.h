#pragma once
#include "Autowiring/ObjectPool.h"
#include <typeindex>
#include STL_UNORDERED_SET

class AutoPacket;

class AutoPacketFactorySatisfactionBase {
public:
  virtual ~AutoPacketFactorySatisfactionBase(void) {}
};

/// <summary>
/// A satisfaction implements the behavior that results 
/// </summary>
template<class T>
class AutoPacketFactorySatisfaction:
  AutoPacketFactorySatisfactionBase
{
private:
  /// <summary>
  /// The instance to which this satisfaction actually refers
  /// </summary>
  std::shared_ptr<T> m_sat;

public:
  /// <summary>
  /// Method invoked by the AutoPacket when this satisfaction is complete
  /// </summary>
  void OnSatisfied(void) {

  }
};

/// <summary>
/// A configurable factory class for pipeline packets with a built-in object pool
/// </summary>
/// <remarks>
/// Generally, only one packet factory is required per context.
/// </remarks>
class AutoPacketFactory
{
public:
  AutoPacketFactory(void);
  ~AutoPacketFactory(void);

private:
  /// <summary>
  /// An independently maintained object pool just for packets
  /// </summary>
  /// <remarks>
  /// The object pool defined here is provided mainly to allow detection of
  /// pipeline packet expiration in order to support expiration notification
  /// broadcasts.
  /// </remarks>
  ObjectPool<AutoPacket> m_packets;

  // Optimization value.  Stores the total number of all satisfactions.  This
  // is equivalent to the sum of all of the sizes in the m_sats list.
  size_t m_numSats;

  /// <summary>
  /// The packet factory satisfaction graph
  /// </summary>
  std::unordered_set<std::type_index, std::list<AutoPacketFactory>> m_sats;

public:
  /// <summary>
  /// </summary>
};


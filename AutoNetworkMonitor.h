// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _AUTO_NETWORK_MONITOR_H
#define _AUTO_NETWORK_MONITOR_H
#include <boost/icl/interval_map.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <iosfwd>
#include <list>
#include <map>

class AutowirableSlot;
class ContextMember;

/// <summary>
/// Allows a dependency network to be constructed on a per-context basis
/// </summary>
/// <remarks>
/// The autowiring network monitor constructs a graph of contexts and context members,
/// and provides information about the relationships between these components.
///
/// The network monitor generally will not be functional in any mode except debug mode.
/// Callers who are interested in enabling the network monitor in other modes should
/// define ENABLE_NETWORK_MONITOR, and should be advised that this will add overhead
/// to all autowiring or context operations.
///
/// ONLY OBJECTS INHERITING FROM ContextMember CAN BE PROPERLY TRACKED.
/// </remarks>
class AutoNetworkMonitor
{
public:
  AutoNetworkMonitor(void);
  ~AutoNetworkMonitor(void);

  struct Entry
  {
    Entry(std::weak_ptr<AutowirableSlot> slot, std::weak_ptr<ContextMember> member):
      m_slot(slot),
      m_member(member)
    {}

    std::weak_ptr<AutowirableSlot> m_slot;
    std::weak_ptr<ContextMember> m_member;
  };

  struct Allocation
  {
    Allocation(const void* pSpace, size_t nBytes);

    const void* pSpace;
    size_t nBytes;

    bool Enclosed(const void* ptr) const {
      return pSpace < ptr && ptr < (char*)pSpace + nBytes;
    }

    bool operator<(const Allocation& rhs) const {
      return pSpace < rhs.pSpace;
    }
  };

  struct Space
  {
    // This maps a constructed interval.  We require a true map because lower bounds are important.
    typedef std::map<Allocation, std::list<std::weak_ptr<AutowirableSlot>>> t_mpType;
    t_mpType m_mp;

    t_mpType::iterator Find(const void* ptr);
  };

  // Space tracking monitor--global because it's thread-specific:
  static boost::thread_specific_ptr<Space> s_space;

private:
  // The collection of all known associations
  boost::mutex m_lock;
  std::list<Entry> m_knownSlots;

public:
  /// <summary>
  /// Notifies this monitor that a new space has just been constructed
  /// </summary>
  /// <remarks>
  /// This member is typically invoked from an overridden new to indicate a pending space construction.
  /// PreConstruct notifications enable pointers to fields to be resolved to the size of their enclosing
  /// space.
  /// </remarks>
  void PreConstruct(void* pMem, size_t szSpace);
  
  /// <summary>
  /// Notifies this network monitor that a new 
  /// </summary>
  void Notify(const AutowirableSlot& lhs);

  /// <summary>
  /// Notifies this monitor that construction on the specified space is now complete
  /// </summary>
  /// <remarks>
  /// PostConstruct is typically invoked from the CoreContext when a particular pointer has been added.
  /// This indicates that the object at the specified space, and any pointers allocated since it was
  /// created, are no longer being constructed.
  ///
  /// The caller is not required to know about the size of the constructed space.  Instead, a thread-
  /// specific interval tree is used to resolve the space pointer to the enclosing allocation.
  /// </remarks>
  void PostConstruct(void* pMem);

  /// <summary>
  /// Writes a serialized version of the internal dependency graph to the specified output stream
  /// </summary>
  void Serialize(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const AutoNetworkMonitor& rhs);

#endif

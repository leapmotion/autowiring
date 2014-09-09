// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/Bolt.h>
#include <autowiring/DeclareAutoFilter.h>
#include <autowiring/atomic_object.h>
#include <autowiring/AutoPacket.h>
#include <unordered_set>

// TODO: Find a way to propagate target information, and make sure that it is used
// when GetAll or HasAll are called.
// NOTE: This is important, since the gather should be ONLY for data that is piped in.
// NOTE: This means that additional steps are required since data must be piped to
// the merge node.
// IDEA: If this is the goal, then there should be a helper function to declare pipes.

/// <summary>
/// Extracts all instances of a type from a slave-context on each execution of that context.
/// </summary>
/// <remarks>
/// It is expected that AutoMerge is a member of the parent of both master and slave contexts.
/// AutoFilters in child contexts are called by AutoPackets created in the parent context.
/// However, a merge of all instances of a type is undefined until the execution of the slave-context
/// is complete.
/// Therefore, in order to ensure that the termination of execution of the slave context is
/// well defined, it cannot be a child of the master context.
/// Thus, in order to ensure discovery of both master and slave contexts, AutoMerge is
/// expected to be a member of a parent context of both.
/// </remarks>
template<class merge_type, class slave_sigil>
class AutoMerge :
  public Bolt<slave_sigil>,
  public atomic_object<std::unordered_map<std::type_index, std::shared_ptr<merge_type>>>
{
public:
  typedef atomic_object<std::unordered_map<std::type_index, std::shared_ptr<merge_type>>> atomic;
  typedef typename atomic::object object;
  typedef typename atomic::lock lock;
  typedef typename atomic::unlock unlock;
  typedef typename atomic::shared shared;
  typedef BasedAutoFilter<AutoMerge<merge_type, slave_sigil>, void, const AutoPacket&> gather;

protected:
  /// Reference to the slave context
  std::weak_ptr<CoreContext> m_slave_context;

public:
  AutoMerge() {}
  using atomic::operator=;
  operator object(void) {
    // NOTE: This avoids "using" keyword with a cast operator overload
    return *(atomic*)this;
  }

  void ContextCreated() {
    if (!m_slave_context.expired()) {
      throw std::runtime_error("AutoMerge cannot reference multuple slave contexts");
    }

    // Maintain a reference to the slave context
    m_slave_context = AutoCurrentContext();

    // Inject a gather node into the slave context
    DeclareAutoFilter(this, &AutoMerge<merge_type, slave_sigil>::AutoGather);
  }

  /// <summary>Final call in slave context to extract all data of the specified type</summary>
  void AutoGather(const AutoPacket& slave_data) {
    atomic::operator = (slave_data.GetAll<merge_type>());
  }
};

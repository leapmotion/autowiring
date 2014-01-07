// Transient pools are deprecated. Don't include this header.
#error
#pragma once

/// <summary>
/// A marker interface used to indicate a context member which may be removed at any time
/// </summary>
/// <remarks>
/// A transient context member differs from ordinary context members in that a transient
/// member cannot be the subject of an Autowiring.  Contexts will also generally not return
/// a transient context member as the subject of a search, and will not hold references
/// to any transient member.  Transient context members must not inherit from ContextMember,
/// and as a result, must not inherit from CoreThread.  Finally, the autonet will not show
/// relationships involving a transient member.
///
/// The benefit of using a transient member, however, is that a transient member is the only
/// context member type which may be removed from the context.  Transient members are also
/// free to autowire any non-transient members in their current context, and will be properly
/// connected as the sender or recipient of events.
///
/// Transient members must be manually inserted into a context via the Add function, or may
/// be declaratively inserted via AutoTransient.
/// </remarks>
class TransientContextMember
{
public:
  DEPRECATED(TransientContextMember(), "Transient context members are deprecated.  Instead of using transient members, insert them into an anonymous subcontext") {}
  virtual ~TransientContextMember(void) {}
};


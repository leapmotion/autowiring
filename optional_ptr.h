#pragma once

/// <summary>
/// A wrapper type, used to indicate a pointer which cannot be deferred
/// </summary>
/// <remarks>
/// An optional pointer denotes a field which may be omitted during a call to a filter
/// method.  The caller is responsible for testing a field for correctness.
///
/// Consumers are cautioned against using optional_ptr where not necessary.  Users of
/// optional_ptr will only be notified that a field is satisfied until the packet is
/// about to be destroyed, or the field has been explicitly marked as unsatisfiable
/// (for example, due to a failed checkout).
/// </remarks>
template<class T>
class optional_ptr
{
public:
};

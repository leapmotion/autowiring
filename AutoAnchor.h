#pragma once

// Marker base class for anchors
struct AutoAnchorBase {};

/// <summary>
/// Anchors a particular object type or event type to the annotated context sigil
/// </summary>
/// <remarks>
/// The AutoAnchor is a marker type to be applied to a sigil type.  When a member of a subcontext of any
/// annotated sigil class attempts to AutoRequire or AutoFire an instance of any sigil type, instead of
/// creating the corresponding object or obtaining the junction box in that child context, the request
/// will be satisfied instead by the anchor.
/// </remarks>
template<typename... Ts>
struct AutoAnchor:
  AutoAnchorBase
{};

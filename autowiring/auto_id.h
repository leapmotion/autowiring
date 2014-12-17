// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

/// <summary>
/// Identifier sigil structure
/// </summary>
/// <remarks>
/// This structure is used to avoid attempts to obtain the typeid of an incomplete type.
/// It's only used in cases where typeid(T) would have been used alone.
/// </remarks>
template<class T>
struct auto_id {};

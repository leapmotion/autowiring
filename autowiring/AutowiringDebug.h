// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <string>
#include <vector>

struct AutoFilterDescriptor;
class AutoPacket;

namespace autowiring {
namespace dbg {

/// <returns>
/// True if the specified typename looks like it might be the type of a lambda function
/// </returns>
bool IsLambda(const std::type_info& ti);

/// <returns>
/// A string corresponding to the current context name
/// </returns>
std::string ContextName(void);

/// <returns>
/// The current packet under processing
/// </returns>
AutoPacket* CurrentPacket(void);

/// <returns>
/// An AutoFilterDescriptor corresponding to the filter with the specified name
/// </returns>
/// <param name="name">The name of the filter</param>
const AutoFilterDescriptor* DescriptorByName(const char* name);

/// <summary>
/// Attempts to determine why the specified AutoFilter type might not be called
/// </summary>
/// <param name="name">The name of the filter</param>
std::string AutoFilterInfo(const char* name);

/// <summary>
/// Shows decorations that are considered "root decorations" in the current context
/// </summary>
/// <param name="name">The name of the filter</param>
std::vector<std::string> ListRootDecorations(void);

/// <summary>
/// Initializes the Autowiring debug library
/// </summary>
void DebugInit(void);
}
}
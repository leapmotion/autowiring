// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <string>
#include <vector>
#include <ostream>
#include <memory>

struct AutoFilterDescriptor;
class AutoPacket;
class CoreContext;

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

    /// <summary>
    /// Write a string representation of the context hierarchy to 'os'(defaults to std::cout)
    /// </summary>
    void PrintContextTree(void);
    void PrintContextTree(std::ostream& os);
    void PrintContextTree(std::ostream& os, const std::shared_ptr<CoreContext>& ctxt);

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
    /// It will recursively print out the auto filters which provides inputs for the specified
    /// AutoFilter
    /// </summary>
    /// <param name="name">The name of the filter</param>
    std::string AutoFilterInfo(const char* name);

    /// <summary>
    /// Shows decorations that are considered "root decorations" in the current context
    /// </summary>
    /// <param name="name">The name of the filter</param>
    std::vector<std::string> ListRootDecorations(void);

    /// <summary>
    /// Write a DOT file representing the AutoFilter
    /// </summary>
    /// <param name="ctxt">Context of AutoFilter network to output. Defaults to AutoCurrentContext</param>
    /// <param name="os">output stream to write DOT file</param>
    /// <returns> string representation of DOT file, or outputs to 'os' </returns>
    std::string AutoFilterGraphStr(void);
    void WriteAutoFilterGraph(std::ostream& os);
    void WriteAutoFilterGraph(std::ostream& os, const std::shared_ptr<CoreContext>& ctxt);
    void WriteAutoFilterGraph(std::ostream& os, CoreContext& ctxt);

    /// <summary>
    /// Initializes the Autowiring debug library
    /// </summary>
    void DebugInit(void);
  }
}

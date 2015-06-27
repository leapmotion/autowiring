// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutowiringDebug.h"
#include "Autowired.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace autowiring;
using namespace autowiring::dbg;

#ifdef _MSC_VER
// On Windows, lambda functions start with the key string "class <"
bool autowiring::dbg::IsLambda(const std::type_info& ti) {
  return demangle(ti)[0] == '<';
}
#else
// On other platforms, try to find lambda functions by the presence of forbidden characters
bool autowiring::dbg::IsLambda(const std::type_info& ti) {
  auto name = demangle(ti);
  for (auto cur : name)
    if (
      !('a' <= cur && cur <= 'z') &&
      !('A' <= cur && cur <= 'Z') &&
      !('0' <= cur && cur <= '9') &&
      cur != '_' &&
      cur != ':' &&
      cur != '<' &&
      cur != '>' &&
      cur != ',' &&
      cur != ' '
    )
      return true;
  return false;
}
#endif

static std::string DemangleWithAutoID(const std::type_info& ti) {
  auto retVal = demangle(ti);

  // prefix is at the beginning of the string, skip over it
  static const char prefix [] = "auto_id<";

  if (retVal.compare(0, sizeof(prefix) - 1, prefix) == 0) {
    size_t off = sizeof(prefix) - 1;
    size_t end = retVal.find_last_not_of(' ', retVal.find_last_of('>') - 1);
    retVal = retVal.substr(off, end - off + 1);
  }
  return retVal;
}

std::string autowiring::dbg::ContextName(void) {
  AutoCurrentContext ctxt;
  return autowiring::demangle(ctxt->GetSigilType());
}

void PrintContextTreeRecursive(std::ostream& os, std::shared_ptr<CoreContext> ctxt) {
  // Initialize this only once
  AutoCurrentContext curCtxt;
  
  for (; ctxt; ctxt = ctxt->NextSibling()) {
    
    // Create of vector of contexts from child of global to 'ctxt'
    std::deque<std::shared_ptr<CoreContext>> path;
    for (auto c = ctxt; !c->IsGlobalContext(); c = c->GetParentContext()) {
      path.push_front(c);
    }

    // Print indentation the shows 'ctxt's path in the tree
    for (auto it = path.begin(); it != path.end(); ++it)
      if (std::next(it) == path.end()) // Check if last element
        if ((*it)->NextSibling())
          os << "├── ";
        else
          os << "└── ";
      else if ((*it)->NextSibling()) // If not the last child
        os << "│   ";
      else
        os << "    ";

    // Print context name
    os << autowiring::demangle(ctxt->GetSigilType());
    if (ctxt == curCtxt) {
      os << "(Current Context)";
    }
    os << std::endl;
    PrintContextTreeRecursive(os, ctxt->FirstChild());
  }
}

void autowiring::dbg::PrintContextTree(std::ostream& os) {
  PrintContextTreeRecursive(os, AutoGlobalContext());
}

void autowiring::dbg::PrintContextTree(void) {
  PrintContextTreeRecursive(std::cout, AutoGlobalContext());
}

AutoPacket* autowiring::dbg::CurrentPacket(void) {
  Autowired<AutoPacketFactory> factory;
  if (!factory)
    return nullptr;
  return factory->CurrentPacket().get();
}

static const AutoFilterDescriptor* DescriptorByName(const char* name, const std::vector<AutoFilterDescriptor>& filters) {
  for (const auto& filter : filters) {
    auto type = filter.GetType();
    if (!type)
      continue;

    auto curName = demangle(type);
    if (!curName.compare(name))
      return &filter;
  }
  return nullptr;
}

const AutoFilterDescriptor* autowiring::dbg::DescriptorByName(const char* name) {
  Autowired<AutoPacketFactory> factory;
  if (!factory)
    return nullptr;

  return DescriptorByName(name, factory->GetAutoFilters());
}

std::string autowiring::dbg::AutoFilterInfo(const char* name) {
  Autowired<AutoPacketFactory> factory;
  if (!factory)
    return "No factory detected in this context";

  // Obtain all descriptors:
  const std::vector<AutoFilterDescriptor> descs = factory->GetAutoFilters();

  // Need the root descriptor first
  const AutoFilterDescriptor* desc = DescriptorByName(name, descs);
  if (!desc)
    return "Filter not found";

  std::ostringstream os;
  std::function<void(const AutoFilterDescriptor&, size_t)> fnCall = [&](const AutoFilterDescriptor& desc, size_t nLevels) {
    auto args = desc.GetAutoFilterArguments();

    for (size_t i = 0; i < desc.GetArity(); i++) {
      // Argument must be an input to the filter we want to know more about
      if (!args[i].is_input)
        continue;

      // Who provides this input?
      for (const auto& providerDesc : descs) {
        auto providerArg = providerDesc.GetArgumentType(args[i].ti);
        if (providerArg && providerArg->is_output) {
          // Need to print information about this provider:
          os << demangle(*args[i].ti) << ' ' << std::string(nLevels, ' ') << demangle(*providerDesc.GetType()) << std::endl;

          // The current descriptor provides an input to the parent, recurse
          fnCall(providerDesc, nLevels + 1);
        }
      }
    }
  };

  // Root typename print first:
  os << demangle(desc->GetType()) << std::endl;
  fnCall(*desc, 1);

  auto retVal = os.str();
  return retVal;
}

std::vector<std::string> autowiring::dbg::ListRootDecorations(void) {
  Autowired<AutoPacketFactory> factory;
  if (!factory)
    return std::vector<std::string>{};

  // Obtain all descriptors:
  const std::vector<AutoFilterDescriptor> descs = factory->GetAutoFilters();

  // Get all baseline descriptor types, figure out what isn't satisfied:
  std::unordered_map<std::type_index, const std::type_info*> outputs;
  std::unordered_map<std::type_index, const std::type_info*> inputs;

  for (const auto& desc : descs) {
    auto args = desc.GetAutoFilterArguments();
    for (size_t i = 0; i < desc.GetArity(); i++)
      (args[i].is_output ? outputs : inputs)[*args[i].ti] = args[i].ti;
  }

  // Remove all inputs that exist in the outputs set:
  for (auto& output : outputs)
    inputs.erase(*output.second);

  // Any remaining inputs are unsatisfied
  std::vector<std::string> retVal;
  for (auto& input : inputs)
    if (input.second)
      retVal.push_back(DemangleWithAutoID(*input.second));
  return retVal;
}

void autowiring::dbg::WriteAutoFilterGraph(std::ostream& os) {
  WriteAutoFilterGraph(os, AutoCurrentContext());
}

std::string autowiring::dbg::AutoFilterGraphStr(void) {
  std::stringstream ss;
  WriteAutoFilterGraph(ss);
  return ss.str();
}

void autowiring::dbg::WriteAutoFilterGraph(std::ostream& os, const std::shared_ptr<CoreContext>& ctxt) {
  CurrentContextPusher pshr(ctxt);
  Autowired<AutoPacketFactory> factory;

  // Write opening and closing parts of file. Now, we only need to write edges
  os << "digraph " << autowiring::demangle(ctxt->GetSigilType()) << " {" << std::endl;
  auto exit = MakeAtExit([&os]{
    os << "}" << std::endl;
  });

  // If no factory, this is an empty network
  if (!factory) {
    return;
  }

  // Obtain all descriptors:
  const std::vector<AutoFilterDescriptor> descs = factory->GetAutoFilters();

  std::unordered_map<std::string, int> decorations; // demangled name to ID
  std::unordered_map<std::string, int> filters; // demangled name to ID

  // give each node a unique id
  int idCounter = 0;

  for (const auto& desc : descs) {
    std::string filter = autowiring::demangle(desc.GetType());

    // Assign each filter an ID
    if (filters.find(filter) == filters.end()) {
      filters[filter] = idCounter++;
    }

    auto args = desc.GetAutoFilterArguments();
    for (size_t i = 0; i < desc.GetArity(); i++) {
      const AutoFilterArgument& arg = args[i];

      std::string decoration = DemangleWithAutoID(*arg.ti);

      // Assign each decoration and ID
      if (decorations.find(decoration) == decorations.end()) {
        decorations[decoration] = idCounter++;
      }

      // Add edge
      if (arg.is_input) {
        os << decorations[decoration] << " -> " << filters[filter];
      } else {
        os << filters[filter] << " -> " << decorations[decoration];
      }

      // Label time shifted edges
      if (arg.tshift) {
        os << " [style=dotted label=\"prev=" << arg.tshift << "\"]";
      }

      os << std::endl;
    }
  }

  // Label each filter with its demangled name
  for (const auto& filter : filters) {
    const std::string& name = filter.first;
    int id = filter.second;

    os << id << " [shape=box label=\""<< name << "\"];" << std::endl;
  }

  // Label each decoration with its demangled name
  for (const auto& decoration : decorations) {
    const std::string& name = decoration.first;
    int id = decoration.second;

    os << id << " [shape=oval label=\""<< name << "\"];" << std::endl;
  }
}

void autowiring::dbg::DebugInit(void) {
  static const void* p [] = {
    (void*) AutoFilterGraphStr
  };
  (void)p;
}
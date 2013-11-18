#pragma once
#include "Bolt.h"
#include "Autowired.h"

/// <summary>
/// Utility class, defined at file level, which registers a type as a member of a context
/// </summary>
/// <remarks>
/// A microbolt is an extremely simple bolt which will, on the creation of a context of some type,
/// do one of three possible things:
///
/// 1)  If T provides a static method of the form "ContextCreated(const char*)", invoke this
///     method with the specified ContextName
/// 2)  Otherwise, follow the AutoRequired behavior for T on the current context
///
/// </remarks>
template<class T, const char* contextName>
class MicroBolt:
  public Bolt<contextName>
{
public:
  void ContextCreated(void) override {
    AutoRequired<T>();
  }
};

#define BOLT_TO(T, contextName) \
  extern const char s_microBolt##T##contextName##str[] = #contextName; \
  static AutoRequired<MicroBolt<T, s_microBolt##T##contextName##str>> s_microBolt##T##contextName;


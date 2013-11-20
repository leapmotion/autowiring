#pragma once
#include "Bolt.h"
#include "Autowired.h"
#include <string>

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
/// </remarks>
class MicroBoltBase:
  public EventReceiver
{
public:
  virtual void ListenForAnAbsolutePath(const std::string& absoluteContextPath) = 0;
};

template<class T, const char* absolutepath>
class MicroBolt:
  public MicroBoltBase
{
public:
  void ListenForAnAbsolutePath(const std::string& absoluteContextPath) override {
    if(!absoluteContextPath.compare(absolutePath))
      AutoRequired<T>();
  }
};

#define BOLT_TO(type, absolutePath) \
  namespace { \
    class SpecificBolt: \
      public MicroBoltBase \
    { \
      void ListenForAnAbsolutePath(const std::string& absoluteContextPath) override { \
        if(!absoluteContextPath.compare(absolutePath)) \
          AutoRequired<type>(); \
      } \
    }; \
    static AutoRequired<SpecificBolt> s_bolt; \
  }
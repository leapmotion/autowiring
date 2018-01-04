// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
/// @page AutoFilterTutorial_1_0 AutoFilter Tutorial 1.0
/// This tutorial will introduce the reader to the different kinds of `AutoFilter` input parameter semantics.
///
/// The central theme to the Autowiring implementation of the `AutoFilter` concept is that parameters are only
/// distinguished by their types, modulo the type semantics of `AutoFilter` parameters, and that there can only
/// be at most one instance of a type in the execution of a filter network at any given time (with certain caveats
/// which will be explained later).
///
/// Regarding distinguishing `AutoFilter` input parameter types, for a given C++ type `T`, there are several related types
/// which are considered equivalent in the at-most-one-instance sense described above.  They are:
/// - `T`
/// - `const T`
/// - `const T &`
/// - `std::shared_ptr<const T>`
/// - `const std::shared_ptr<const T>`
/// - `const std::shared_ptr<const T> &`
///
#include <autowiring/autowiring.h> // Needed for Autowiring classes.
#include <iostream>               // Needed for std::cout.
#include <string>                 // Needed for std::string.
#include <unordered_set>          // Needed for std::unordered_set.

/// We will now define several structures having AutoFilter methods.  Note that classes and structs are essentially equivalent
/// in C++ (besides the default access specifiers), so using struct here doesn't indicate anything special, it just allows us
/// to avoid having to explicitly write the `public:` access specifier.
///
/// For each type in the list above, we define a struct having an AutoFilter having that parameter type.  The method will print
/// a reflective message indicating the struct and the type, and then store the input value.  The input values that we will
/// send will indicate the type used to decorate the packet used in the execution.  This way, we can verify that the expected
/// inputs were received by the expected context members.
struct Hippo1 {
  void AutoFilter (std::string input) {
    std::cout << "Hippo1::AutoFilter(std::string) was called with value: \"" << input << "\"\n";
    m_received_input_decoration_types.insert(input);
  }
  std::unordered_set<std::string> m_received_input_decoration_types;
};

struct Hippo2 {
  void AutoFilter (const std::string input) {
    std::cout << "Hippo2::AutoFilter(const std::string) was called with value: \"" << input << "\"\n";
    m_received_input_decoration_types.insert(input);
  }
  std::unordered_set<std::string> m_received_input_decoration_types;
};

struct Hippo3 {
  void AutoFilter (const std::string &input) {
    std::cout << "Hippo3::AutoFilter(const std::string &) was called with value: \"" << input << "\"\n";
    m_received_input_decoration_types.insert(input);
  }
  std::unordered_set<std::string> m_received_input_decoration_types;
};

struct Hippo4 {
  void AutoFilter (std::shared_ptr<const std::string> input) {
    std::cout << "Hippo4::AutoFilter(std::shared_ptr<const std::string>) was called with (dereferenced) value: \"" << *input << "\"\n";
    m_received_input_decoration_types.insert(*input);
  }
  std::unordered_set<std::string> m_received_input_decoration_types;
};

struct Hippo5 {
  void AutoFilter (const std::shared_ptr<const std::string> input) {
    std::cout << "Hippo5::AutoFilter(const std::shared_ptr<const std::string>) was called with (dereferenced) value: \"" << *input << "\"\n";
    m_received_input_decoration_types.insert(*input);
  }
  std::unordered_set<std::string> m_received_input_decoration_types;
};

struct Hippo6 {
  void AutoFilter (const std::shared_ptr<const std::string> &input) {
    std::cout << "Hippo6::AutoFilter(const std::shared_ptr<const std::string> &) was called with (dereferenced) value: \"" << *input << "\"\n";
    m_received_input_decoration_types.insert(*input);
  }
  std::unordered_set<std::string> m_received_input_decoration_types;
};

int main () {
  // Initialization of context, injection of context members, obtaining access to the factory.
  AutoCurrentContext()->Initiate();
  AutoRequired<Hippo1> hippo1;
  AutoRequired<Hippo2> hippo2;
  AutoRequired<Hippo3> hippo3;
  AutoRequired<Hippo4> hippo4;
  AutoRequired<Hippo5> hippo5;
  AutoRequired<Hippo6> hippo6;
  Autowired<AutoPacketFactory> factory;
  // Declare a packet to use in the following code blocks.
  std::shared_ptr<AutoPacket> packet;

/// At this point we will execute the filter network a number of times, each with a different type related to `std::string`,
/// in order to observe which filters are executed.  The first six executions demonstrate the types that are equivalent to
/// `std::string` as input parameters (in which we expect only `Hippo#::AutoFilter` to be called). 
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `std::string`:\n";
    std::string s("std::string");
    packet->Decorate(s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `const std::string`:\n";
    const std::string s("const std::string");
    packet->Decorate(s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `std::shared_ptr<std::string>`:\n";
    std::shared_ptr<std::string> s = std::make_shared<std::string>("std::shared_ptr<std::string>");
    packet->Decorate(s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `const std::shared_ptr<std::string>`:\n";
    const std::shared_ptr<std::string> s = std::make_shared<std::string>("const std::shared_ptr<std::string>");
    packet->Decorate(s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `std::shared_ptr<const std::string>`:\n";
    std::shared_ptr<const std::string> s = std::make_shared<const std::string>("std::shared_ptr<const std::string>");
    packet->Decorate(s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `const std::shared_ptr<const std::string>`:\n";
    const std::shared_ptr<const std::string> s = std::make_shared<const std::string>("const std::shared_ptr<const std::string>");
    packet->Decorate(s);
    std::cout << '\n';
  }

/// Verify that the accumulated values in the `m_received_input_decoration_types` for each context member are what we expect.
  {
    std::unordered_set<std::string> expected_hippo_inputs({
      "std::string",
      "const std::string",
      "std::shared_ptr<std::string>",
      "const std::shared_ptr<std::string>",
      "std::shared_ptr<const std::string>",
      "const std::shared_ptr<const std::string>"
    });
    if (hippo1->m_received_input_decoration_types != expected_hippo_inputs)
      throw std::runtime_error("Hippo1 did not receive the expected inputs.");
    if (hippo1->m_received_input_decoration_types != expected_hippo_inputs)
      throw std::runtime_error("Hippo2 did not receive the expected inputs.");
    if (hippo1->m_received_input_decoration_types != expected_hippo_inputs)
      throw std::runtime_error("Hippo3 did not receive the expected inputs.");
    if (hippo1->m_received_input_decoration_types != expected_hippo_inputs)
      throw std::runtime_error("Hippo4 did not receive the expected inputs.");
    if (hippo1->m_received_input_decoration_types != expected_hippo_inputs)
      throw std::runtime_error("Hippo5 did not receive the expected inputs.");
    if (hippo1->m_received_input_decoration_types != expected_hippo_inputs)
      throw std::runtime_error("Hippo6 did not receive the expected inputs.");
    std::cout << "All Hippo# context members received the expected inputs.\n";
  }
  std::cout << "All verifications passed.\n";

  return 0; // Return with no error.
}

/// The output of this program should be (with possibly a different ordering of `AutoFilter` calls):
///
///     Decorating packet with instance of `std::string`:
///     Hippo3::AutoFilter(const std::string &) was called with value: "std::string"
///     Hippo2::AutoFilter(const std::string) was called with value: "std::string"
///     Hippo1::AutoFilter(std::string) was called with value: "std::string"
///     Hippo6::AutoFilter(const std::shared_ptr<const std::string> &) was called with (dereferenced) value: "std::string"
///     Hippo5::AutoFilter(const std::shared_ptr<const std::string>) was called with (dereferenced) value: "std::string"
///     Hippo4::AutoFilter(std::shared_ptr<const std::string>) was called with (dereferenced) value: "std::string"
///     
///     Decorating packet with instance of `const std::string`:
///     Hippo3::AutoFilter(const std::string &) was called with value: "const std::string"
///     Hippo2::AutoFilter(const std::string) was called with value: "const std::string"
///     Hippo1::AutoFilter(std::string) was called with value: "const std::string"
///     Hippo6::AutoFilter(const std::shared_ptr<const std::string> &) was called with (dereferenced) value: "const std::string"
///     Hippo5::AutoFilter(const std::shared_ptr<const std::string>) was called with (dereferenced) value: "const std::string"
///     Hippo4::AutoFilter(std::shared_ptr<const std::string>) was called with (dereferenced) value: "const std::string"
///     
///     Decorating packet with instance of `std::shared_ptr<std::string>`:
///     Hippo3::AutoFilter(const std::string &) was called with value: "std::shared_ptr<std::string>"
///     Hippo2::AutoFilter(const std::string) was called with value: "std::shared_ptr<std::string>"
///     Hippo1::AutoFilter(std::string) was called with value: "std::shared_ptr<std::string>"
///     Hippo6::AutoFilter(const std::shared_ptr<const std::string> &) was called with (dereferenced) value: "std::shared_ptr<std::string>"
///     Hippo5::AutoFilter(const std::shared_ptr<const std::string>) was called with (dereferenced) value: "std::shared_ptr<std::string>"
///     Hippo4::AutoFilter(std::shared_ptr<const std::string>) was called with (dereferenced) value: "std::shared_ptr<std::string>"
///     
///     Decorating packet with instance of `const std::shared_ptr<std::string>`:
///     Hippo3::AutoFilter(const std::string &) was called with value: "const std::shared_ptr<std::string>"
///     Hippo2::AutoFilter(const std::string) was called with value: "const std::shared_ptr<std::string>"
///     Hippo1::AutoFilter(std::string) was called with value: "const std::shared_ptr<std::string>"
///     Hippo6::AutoFilter(const std::shared_ptr<const std::string> &) was called with (dereferenced) value: "const std::shared_ptr<std::string>"
///     Hippo5::AutoFilter(const std::shared_ptr<const std::string>) was called with (dereferenced) value: "const std::shared_ptr<std::string>"
///     Hippo4::AutoFilter(std::shared_ptr<const std::string>) was called with (dereferenced) value: "const std::shared_ptr<std::string>"
///     
///     Decorating packet with instance of `std::shared_ptr<const std::string>`:
///     Hippo3::AutoFilter(const std::string &) was called with value: "std::shared_ptr<const std::string>"
///     Hippo2::AutoFilter(const std::string) was called with value: "std::shared_ptr<const std::string>"
///     Hippo1::AutoFilter(std::string) was called with value: "std::shared_ptr<const std::string>"
///     Hippo6::AutoFilter(const std::shared_ptr<const std::string> &) was called with (dereferenced) value: "std::shared_ptr<const std::string>"
///     Hippo5::AutoFilter(const std::shared_ptr<const std::string>) was called with (dereferenced) value: "std::shared_ptr<const std::string>"
///     Hippo4::AutoFilter(std::shared_ptr<const std::string>) was called with (dereferenced) value: "std::shared_ptr<const std::string>"
///     
///     Decorating packet with instance of `const std::shared_ptr<const std::string>`:
///     Hippo3::AutoFilter(const std::string &) was called with value: "const std::shared_ptr<const std::string>"
///     Hippo2::AutoFilter(const std::string) was called with value: "const std::shared_ptr<const std::string>"
///     Hippo1::AutoFilter(std::string) was called with value: "const std::shared_ptr<const std::string>"
///     Hippo6::AutoFilter(const std::shared_ptr<const std::string> &) was called with (dereferenced) value: "const std::shared_ptr<const std::string>"
///     Hippo5::AutoFilter(const std::shared_ptr<const std::string>) was called with (dereferenced) value: "const std::shared_ptr<const std::string>"
///     Hippo4::AutoFilter(std::shared_ptr<const std::string>) was called with (dereferenced) value: "const std::shared_ptr<const std::string>"
///     
///     All Hippo# context members received the expected inputs.
///     All verifications passed.
///
/// One thing to note is that the order that the `AutoFilter` methods are called is not necessarily the order in which
/// the context members (the `Hippo#` instances) were injected.

// AutoFilterTutorial01.cpp created by Victor Dods on 2015/06/10
// Copyright (C) Leap Motion, Inc. All rights reserved.
/// @page AutoFilterTutorial01 AutoFilter Tutorial 01
/// This tutorial will introduce the reader to the different kinds of AutoFilter input parameter semantics.
///
/// The central theme to the Autowiring implementation of the AutoFilter concept is that parameters are only
/// distinguished by their types, modulo the type semantics of AutoFilter parameters, and that there can only
/// be at most one instance of a type in the execution of a filter network at any given time (with certain caveats
/// which will be explained later).
///
/// Regarding distinguishing AutoFilter input parameter types, for a given C++ type T, there are several related types
/// which are considered equivalent in the at-most-one-instance sense described above.  They are:
/// - T
/// - const T
/// - const T &
/// - std::shared_ptr<const T>
/// - const std::shared_ptr<const T>
/// - const std::shared_ptr<const T> &
///
/// @code{.cpp}
#include <autowiring/Autowired.h> // Needed for Autowiring classes.
#include <iostream>               // Needed for std::cout.
/// @endcode
/// We will now define several structures having AutoFilter methods.  Note that classes and structs are essentially equivalent
/// in C++ (besides the default access specifiers), so using struct here doesn't indicate anything special, it just allows us
/// to avoid having to explicitly write the `public:` access specifier.
///
/// For each type in the list above, we define a struct having an AutoFilter having that parameter type.  The method will simply print
/// a reflective message indicating the struct and the type.
/// @code
struct Hippo1 {
  void AutoFilter (std::string input) { std::cout << "Hippo1::AutoFilter(std::string) was called with value: \"" << input << "\"\n"; }
};

struct Hippo2 {
  void AutoFilter (const std::string input) { std::cout << "Hippo2::AutoFilter(const std::string) was called with value: \"" << input << "\"\n"; }
};

struct Hippo3 {
  void AutoFilter (const std::string &input) { std::cout << "Hippo3::AutoFilter(const std::string &) was called with value: \"" << input << "\"\n"; }
};

struct Hippo4 {
  void AutoFilter (std::shared_ptr<const std::string> input) { std::cout << "Hippo4::AutoFilter(std::shared_ptr<const std::string>) was called with (dereferenced) value: \"" << *input << "\"\n"; }
};

struct Hippo5 {
  void AutoFilter (const std::shared_ptr<const std::string> input) { std::cout << "Hippo5::AutoFilter(const std::shared_ptr<const std::string>) was called with (dereferenced) value: \"" << *input << "\"\n"; }
};

struct Hippo6 {
  void AutoFilter (const std::shared_ptr<const std::string> &input) { std::cout << "Hippo6::AutoFilter(const std::shared_ptr<const std::string> &) was called with (dereferenced) value: \"" << *input << "\"\n"; }
};
/// @endcode
/// We also define two more structs having AutoFilter methods whose parameter types are analogous C++ pointer values.  These will serve
/// to demonstrate which filters are called or not called at what times.
/// @code
struct Hippo7 {
  void AutoFilter (std::string *input) { std::cout << "Hippo7::AutoFilter(std::string *) was called with (dereferenced) value: \"" << *input << "\"\n"; }
};

struct Hippo8 {
  void AutoFilter (const std::string *input) { std::cout << "Hippo8::AutoFilter(const std::string *) was called with (dereferenced) value: \"" << *input << "\"\n"; }
};

int main () {
  // Initialization of context, injection of context members, obtaining access to the factory.
  AutoCurrentContext()->Initiate();
  AutoRequired<Hippo1>();
  AutoRequired<Hippo2>();
  AutoRequired<Hippo3>();
  AutoRequired<Hippo4>();
  AutoRequired<Hippo5>();
  AutoRequired<Hippo6>();
  AutoRequired<Hippo7>();
  AutoRequired<Hippo8>();
  Autowired<AutoPacketFactory> factory;
  // Declare a packet to use in the following code blocks.
  std::shared_ptr<AutoPacket> packet;
/// @endcode
/// TODO: start here
/// @code  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `std::string`:\n";
    std::string s("OSTRICH!");
    packet->Decorate(s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `const std::string`:\n";
    const std::string s("OSTRICH!");
    packet->Decorate(s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `std::shared_ptr<std::string>`:\n";
    std::shared_ptr<std::string> s = std::make_shared<std::string>("OSTRICH!");
    packet->Decorate(s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `const std::shared_ptr<std::string>`:\n";
    const std::shared_ptr<std::string> s = std::make_shared<std::string>("OSTRICH!");
    packet->Decorate(s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `std::shared_ptr<const std::string>`:\n";
    std::shared_ptr<const std::string> s = std::make_shared<const std::string>("OSTRICH!");
    packet->Decorate(s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `const std::shared_ptr<const std::string>`:\n";
    const std::shared_ptr<const std::string> s = std::make_shared<const std::string>("OSTRICH!");
    packet->Decorate(s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `std::string *`:\n";
    std::string s = std::string("OSTRICH!");
    packet->Decorate(&s);
    std::cout << '\n';
  }
  
  {
    packet = factory->NewPacket();
    std::cout << "Decorating packet with instance of `const std::string *`:\n";
    const std::string s = std::string("OSTRICH!");
    packet->Decorate(&s);
    std::cout << '\n';
  }
  
  return 0; // Return with no error.
}
/// @endcode
/// The output of this program is:
///
///     XXX

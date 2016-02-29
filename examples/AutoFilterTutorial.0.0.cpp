// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
/// @page AutoFilterTutorial_0_0 AutoFilter Tutorial 0.0
/// This tutorial will introduce the reader to the AutoFilter concept and how to implement a rudimentary
/// AutoFilter network.  It is recommended that the reader become familiar with the concept of a context
/// (see XXX TODO context tutorial) before reading this tutorial.
///
/// A filter network can be thought of as a directed acyclic graph, where the nodes of the graph represent
/// data structures and the collection of arrows pointing at each graph node represent a function which
/// populates the data structure which that node represents.  That collection of arrows will be refered to
/// as a filter.
/// 
/// In Autowiring, a filter is implemented as context members having an AutoFilter method.  Its parameters
/// are what define the inputs and outputs.  The inputs and outputs of these AutoFilter methods are what
/// define the nodes of the filter network.
///
/// Particular C++ idioms are used to differentiate the input and output parameters for an AutoFilter method,
/// as well as certain usage semantics.  These will be fully covered in later tutorials.  For now we will
/// deal only with ordinary input and output.
///
/// This tutorial will define and instantiate a simple filter network.
#include <autowiring/autowiring.h> // Needed for Autowiring classes.
#include <cmath>                  // Needed for std::round.
#include <cstdlib>                // Needed for std::atof.
#include <iostream>               // Needed for std::cout.
/// We will define several filters connecting several data structures (really they can be any C++ type), rendering
/// a linear sequence of processing.
///
/// This filter takes a std::string as input and produces a double as output.  Notice that the input is passed
/// to the AutoFilter method by value (other, more efficient semantics will be introduced later), and that the
/// output is passed by reference.  These type semantics are what define what the inputs and outputs of the
/// filter are.  Instead of returning a value, the AutoFilter method need only assign to its output parameters.
///
/// In particular, this filter parses the string as a decimal value and stores it in double-precision floating
/// point format in the output parameter.
class StringToDouble {
public:
  void AutoFilter (std::string input, double &output) {
    output = std::atof(input.c_str());
    std::cout << "StringToDouble received std::string value \"" << input << "\" and has set its output param to double value " << output << ".\n";
  }
};

/// This filter takes a double as input and produces an int as output.  In particular, the output is the rounded
/// value of the input.
class DoubleRounder {
public:
  void AutoFilter (double input, int &output) {
    output = static_cast<int>(std::round(input));
    std::cout << "DoubleRounder received double value " << input << " and has set its output param to int value " << output << ".\n";
  }
};

/// This filter takes an int as input and has no output.  It simply prints the value of the input.
class IntPrinter {
public:
  void AutoFilter (int value) {
    std::cout << "IntPrinter received int value " << value << ".\n";
  }
};

/// To demonstrate this filter network, we will perform the necessary initialization and context member injection
/// within the main function.
int main () {

/// A global context is created by default, and is the default current context.  We must initialize that context
/// before proceeding.
  AutoCurrentContext()->Initiate();

/// Each of the filters must be injected into the context in which they'll operate.  In this case, we're only working
/// with the global context.  The `AutoRequired` method is what accomplishes that injection.
  AutoRequired<StringToDouble>();
  AutoRequired<DoubleRounder>();
  AutoRequired<IntPrinter>();

/// If a context has any members, then the context automatically includes an AutoPacketFactory type. This
/// can be used to create new packets in the AutoFilter network.
  Autowired<AutoPacketFactory> factory;

/// `AutoPacket` is the mechanism which runs filter networks.  An instance of AutoPacket corresponds with one execution
/// of the corresponding filter network.  The packet can be 'decorated' with a value of a particular type.  This means
/// that that type is present during this execution of the filter network as an input parameter to whatever AutoFilter
/// methods are present in the filter network.  Only one value of any given type can be present in a packet at a time.
/// The AutoFilter method of a context member will only be called once all of its inputs have been decorated onto the
/// packet.
///
/// Using the factory, create a new AutoPacket so that we may run the filter network.
  std::shared_ptr<AutoPacket> packet = factory->NewPacket();

/// Now decorate the packet with an `int`.  This will cause the `AutoFilter` methods which only require a single `int`
/// input to be called.  We should expect to see "IntPrinter received int value 42." printed to std::cout at this point.
  packet->Decorate(42);
  std::cout << '\n'; // To make the separation between packets' executions clear in the console output.

/// Create a new packet so that we may run the filter network again.  Note that `packet` is a `std::shared_ptr<AutoPacket>`,
/// and so this assignment deletes the old instance.  Decorate the packet again, but this time with a `double` value, thereby
/// calling the `DoubleRounder` filter, which in turn outputs an `int`, which calls the `IntPrinter` filter, generating output
/// to std::cout.  This demonstrates that a filter network doesn't have a fixed input interface -- inputs can be provided at
/// any point.  Of course, an `AutoFilter` method will be called only when all of its inputs are present on a packet.
  packet = factory->NewPacket();
  packet->Decorate(101.1);
  std::cout << '\n';

/// Repeat the process, but decorate the packet with a value whose rounded value is different.
  packet = factory->NewPacket();
  packet->Decorate(101.9);
  std::cout << '\n';

/// Repeat the process, but decorate the packet with a std::string value.
  packet = factory->NewPacket();
  packet->Decorate(std::string("45954.1"));
  
  return 0; // Return with no error.
}

/// The output of this program is:
///
///     IntPrinter received int value 42.
/// 
///     DoubleRounder received double value 101.1 and has set its output param to int value 101.
///     IntPrinter received int value 101.
/// 
///     DoubleRounder received double value 101.9 and has set its output param to int value 102.
///     IntPrinter received int value 102.
/// 
///     StringToDouble received std::string value "45954.1" and has set its output param to double value 45954.1.
///     DoubleRounder received double value 45954.1 and has set its output param to int value 45954.
///     IntPrinter received int value 45954.

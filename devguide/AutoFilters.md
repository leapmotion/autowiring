Autofilters
=========================

AutoFilters make it easier to construct and extend complex processing flows. While it may be straightforward enough to create a filter graph for a well-defined network in the conventional way, consider the problem of adding to or changing the network. If you add a new step to the process or change the design such that data available early in the process must also be available later in the process, you could have significant replumbing to do. With autowiring's' inversion of control, filters in the network declare what kinds of data they take in and what kinds of data they put out. Autowiring handles the connections. 

The vessel that carries data from one filter to the next is the AutoPacket. An AutoPacket begins life as an “empty” object. The packet is subsequently "decorated" with data. AutoFilters in the network consume the packet by declaring a special function that specifies the input data types the filter requires to do its job. A filter can decorate the packet with additional data, which lets other filters which require that data to process the packet.

The packet lifespan ends when all current AutoFilter functions have returned and have not added decorations that trigger other filters in the network. (Deferred auto filters are an exception to this — the packet will persist at least until the deferred filter is run.)  Technically, the AutoPacket is held as a std::shared_ptr, once all references to the pointer are released, the packet is destroyed.

The context forms the boundaries of a filter network. There are two ways around this. You can snoop and you can use a lambda function.

# Rules of a packet filter network:

* Each filter gets called at most once for a packet.
* A type can only define one AutoFilter method.
* Decorations cannot be removed from a packet.
* A packet can only be decorated with a specific type once.

# Defining an AutoFilter Function:

You can create a filter with any object by implementing a function named AutoFilter. The arguments of the function declares both the input types of the filter and the output types. Const references and arguments passed by value are inputs. Non-const references are output types. The following example creates a class implementing an AutoFilter that takes two inputs and has one output:

\include snippets/AutoFilter_BasicAutoFilter.txt

The function is called when instances of types T and Q are added to the packet. On exit from the function, the type W is added to the packet.

## Arguments

Autowiring uses the form of the argument to determine whether it is an input or an output. Pass-by-value and const arguments are inputs. Pass-by-reference arguments are outputs.

### Input arguments

* ``T`` — type T is an input passed by value
  \include snippets/AutoFilter_InType_ByValue.txt
* ``const T&`` — T is an input, passed by reference.
  \include snippets/AutoFilter_InType_ByRef.txt
* ``shared_ptr<const T> p`` — p is a shared_ptr to input of type T. You can keep p as long as necessary; the shared object won’t be released until after you release p.
  \include snippets/AutoFilter_InType_shared_ptr.txt
* ``T*[]`` — A null-terminated array of pointers to type T. A pointer to an instance of T is added for each filter that adds an instance of T to the packet. The filter taking this input is called when all filters outputting T have run (or no more can run).
  \include snippets/AutoFilter_Type_array.txt
* ``T&&`` — A synchronizing decoration. The previous value is removed from the packet. The filter function can assign a new value. Use for inplace modification of a data item in the packet.
  \include snippets/AutoFilter_OutType_Rvalue.txt

### Output argumets

* ``T&`` — T is a filter output. An instance of T is constructed with its default constructor and passed to the function where it can be modified. The type is automatically added as a packet decoration after the function exits.
  \include snippets/AutoFilter_OutType_ByRef.txt
* ``shared_ptr<T> &p`` — p is a pointer to an output type that this AutoFilter function must construct. This form is useful when type T doesn’t have a default constructor, or there are other reasons not to call it.
  \include snippets/AutoFilter_OutType_shared_ptr.txt

### Special arguments

* ``AutoPacket &packet`` — provides a reference to the packet itself. This argument is not treated as an input or output; other arguments determine when the filter function is called.
  \include snippets/AutoFilter_Type_packet.txt

.. * ``AutoPrev &previous`` --

Note that a filter function with no input arguments is called when a new, empty packet is called.

## Return types:
* ``void`` -- AutoFilter functions do not return a value. Instead, data is passed out of the function using output arguments.
  \include snippets/AutoFilter_ReturnType_void.txt
* ``Deferred`` — Using the special Deferred return type creates a deferred AutoFilter. The class declaring this function must inherit the CoreThread class. The function is added to the CoreThread’s dispatch queue when its input arguments are satisfied instead of being invoked immediately. Return ``Deferred(this)`` from the function.
  \include snippets/AutoFilter_ReturnType_Deferred.txt

# Creating a filter graph

To create a filter graph network:

1. Implement the desired filter classes, each with an AutoFilter function.
2. Autowire these types into a context.
3. Initiate the context.
4. Create a packet using the context's AutoPacketFactory.
5. Decorate the packet with a data type (unless one of your filter functions takes an empty packet as input).
 
The following example shows a hypothetical filter graph that takes a camera image, runs a blur filter, and then line and circle feature finding filters:

\include snippets/AutoFilter_ImageNet.txt

All filters in the same context are part of the same network. You can design things so that parts of the graph never intersect, but the pontential for type collisions and hence unintended interaction exists.

# Decorating packets

AutoFilter functions implicitly decorate packets by implementing an output argument. You can also explicitly decorate a packet with the Decorate() and DecorateImmediate() functions.

The Decorate() function works the same as an AutoFilter output. DecorateImmediate() is for time-critical data, such as items that have a very short lifespan. When you call DecorateImmediate(T), every AutoFilter function that takes the type T and whose other inputs are satisfied is invoked before DecorateImmediate() returns. After that, type T is marked as unsatisfiable and no other functions that take type T are invoked for the current packet.


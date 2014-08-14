// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

//IMPORTANT: These modifiers should apply until destroyed.
//IMPORTANT: This logic must be applicable to contexts.

/// <summary>
/// Declares a pipe of data from transmitter to receiver.
/// </summary>
/// <remarks>
/// The existence of a pipe does not preclude the use of other
/// communication channels.
/// Declaring a pipe has no effect unless either the source or receiver
/// have been declared to be in a non-broadcasting mode.
/// </remarks>
template<class transmitter_type, class receiver_type, class data_type>
class AutoFilterPipe {
public:
  int stub;
  AutoFilterPipe() : stub(0) {}
};

/// <summary>
/// Declares that the specified type of data from the source will not be broadcast.
/// </summary>
/// <remarks>
/// After declaring
/// </remarks>
template <class source_type, class data_type>
class AutoTransmitPipe {
public:
  int stub;
  AutoTransmitPipe() : stub(0) {}
};

/// <summary>
/// Declares that the specified type of data can be received only via a pipe.
/// </summary>
/// <remarks>
/// After declaring
/// </remarks>
template <class receiver_type, class data_type>
class AutoReceivePipe {
public:
  int stub;
  AutoReceivePipe() : stub(0) {}
};

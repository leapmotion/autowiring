#pragma once

/// <summary>
/// A utility type to positively declare an output to an AutoFilter
/// </summary>
/// <remarks>
/// The auto_output wrapper is a convenience alternative to accepting an AutoPacket
/// and then requesting a checkout.  It provides the additional benefit that the
/// caller may declaratively specify whether the auto_output should be satisfied by
/// default when the function returns, or if it must be manually satisfied with a
/// call to 
/// </remarks>
template<class T, bool satisfy = false>
class auto_output {

};
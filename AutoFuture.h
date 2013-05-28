#pragma once

/// <summary>
/// Signifier type used to mark event receivers which must be deferred
/// </summary>
/// <remarks>
/// </remarks>
class AutoFuture
{
public:
  AutoFuture(void);
  ~AutoFuture(void);
};


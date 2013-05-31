#ifndef _I_FACTORY_H
#define _I_FACTORY_H

template<class T>
class IFactory
{
  /// <summary>
  /// Constructs an instance of type T
  /// </summary>
  /// <remarks>
  /// This method puts the return type in the signature so that multiple overloads of this
  /// type can be supported in the same context
  /// </remarks>
  virtual void operator()(T*& obj) = 0;
};

#endif

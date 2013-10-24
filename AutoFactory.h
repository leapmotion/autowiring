#pragma once
#include <typeinfo>
#include TYPE_TRAITS_HEADER

template<typename T>
struct has_static_new
{
  template<class Fn, Fn>
  struct unnamed_constant;

  template<class U>
  static int select(unnamed_constant<U* (*)(), &U::New>*);

  template<class U>
  static char select(...);

  static const bool value = sizeof(select<T>(nullptr)) == sizeof(int);
};

template<typename T, bool isAbstract = std::is_abstract<T>::value>
struct has_simple_constructor
{
  template<class U>
  static int select(decltype(U())*);

  template<class U>
  static char select(...);

  static const bool value = sizeof(select<T>(nullptr)) == sizeof(int);
};

template<typename T>
struct has_simple_constructor<T, true>:
  public std::false_type
{};

class AutoFactoryBase
{
public:
  typedef AutoFactoryBase ground;

  virtual ~AutoFactoryBase(void) {}

  /// <summary>
  /// Returns type information for the proposed 
  /// </summary>
  virtual const std::type_info& GetType(void) const = 0;
};

/// <summary>
/// This is a marker wrapper for classes which must be constructed by a factory
/// </summary>
/// <remarks>
/// A context may contain, at most, one factory for a particular Product.  Introduction of
/// a factory into a context enables the construction of the Product type, and the
/// product's construction must have been impossible without the factory's presence.  This
/// can be done by making the product an abstract type, or by eliminating the trivial
/// constructor from the product.
///
///
/// </remarks>
template<class Product>
class AutoFactory:
  public AutoFactoryBase
{
public:
  virtual const std::type_info& GetType(void) const override {return typeid(Product);}
  virtual Product* New(void) = 0;
};

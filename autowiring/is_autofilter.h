// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include FUNCTIONAL_HEADER
#include TYPE_TRAITS_HEADER

class Deferred;
template<class T, bool auto_ready> class auto_out;
template<class T> class optional_ptr;

/// <summary>
/// Determines whether Arg is an instance of auto_out
/// </summary>
template<class Arg>
struct is_auto_out :
std::false_type
{
  typedef Arg type;
  static const bool ready = false;
};
template<class Arg, bool auto_ready>
struct is_auto_out<auto_out<Arg, auto_ready>> :
std::true_type
{
  typedef Arg type;
  static const bool ready = auto_ready;
};

/// <summary>
/// Determines whether Arg is an instance of optional_ptr
/// </summary>
template<class Arg>
struct is_optional_ptr :
  std::false_type
{
  typedef Arg type;
};
template<class Arg>
struct is_optional_ptr<optional_ptr<Arg>> :
  std::true_type
{
  typedef Arg type;
};

/*
 AutoFilter argument orientations:
 const T& -> REQUIRED INPUT
 T& -> REQUIRED OUTPUT
 const T*& -> OPTIONAL INPUT (nullptr initial state indicates absence)
 T*& -> OPTIONAL OUTPUT (initialized to nullptr, nullptr final state indicates absence)

 TODO: It is desireable to have the optional output drawn from an object pool.
 Therefore it is necessary create something like a const_shared_ptr that inherits from
 shared_ptr but provides only const references.
 const_shared_ptr<T>& -> OPTIONAL INPUT (nullptr initial state indicates absence)
 shared_ptr<T>& -> OPTIONAL OUTPUT (initialized to nullptr, nullptr final state indicates absence)

 Extensions to Dependent Types are always satisfied by the current AutoPacket:
 const AutoPacket& -> Called as first step of finalize, if other arguments are present
 AutoPacket& -> Called initially, or when other arguments are present

 Extensions to Split & Merge arguments are satisfied by dependent packets:
 array_type<shared_ptr<AutoPacket, MoveToConstArray>>& -> Split Node = OUTPUT (called with empty array)
 const array_type<shared_ptr<AutoPacket, MoveToObjectPool>>& -> Merge Node = INPUT (called only when array is full)
 Destruction of packets in the split array moves them to the merge array,
 and destruction of the merge array returns them to the object pool.
 */

template<class Arg>
struct is_required_input :
  std::integral_constant<bool,
    std::is_reference<Arg>::value &&
    std::is_const<typename std::remove_reference<Arg>::type>::value
  >
{
  typedef typename std::remove_const<std::remove_reference<Arg>>::type type;
};
template<class Arg>
struct is_required_output :
  std::integral_constant<bool,
    std::is_reference<Arg>::value &&
    !std::is_const<typename std::remove_reference<Arg>::type>::value
  >
{
  typedef typename std::remove_reference<Arg>::type type;
};
template<class Arg>
struct is_optional_input :
  std::integral_constant<bool,
    std::is_reference<Arg>::value &&
    std::is_pointer<typename std::remove_reference<Arg>::type>::value &&
    std::is_const<typename std::remove_pointer<typename std::remove_reference<Arg>::type>::type>::value
  >
{
  typedef typename std::remove_const<typename std::remove_pointer<typename std::remove_reference<Arg>::type>::type>::type type;
};
template<class Arg>
struct is_optional_output :
  std::integral_constant<bool,
    std::is_reference<Arg>::value &&
    std::is_pointer<typename std::remove_reference<Arg>::type>::value &&
    !std::is_const<typename std::remove_pointer<typename std::remove_reference<Arg>::type>::type>::value
  >
{
  typedef typename std::remove_pointer<typename std::remove_reference<Arg>::type>::type type;
};

/// <summary>
/// Determines whether Arg is an accepted argument of AutoFilter
/// </summary>
/// <remarks>
/// is_autofilter::type is the base type with const, reference, and point removed.
/// </remarks>
template<class Arg>
struct is_autofilter_arg :
  std::integral_constant<bool,
    is_required_input<Arg>::value ||
    is_required_output<Arg>::value ||
    is_optional_input<Arg>::value ||
    is_optional_output<Arg>::value ||
    is_optional_ptr<Arg>::value ||
    is_auto_out<Arg>::value
  >
{
  typedef
  typename std::conditional<is_required_input<Arg>::value, typename is_required_input<Arg>::type,
  typename std::conditional<is_required_output<Arg>::value, typename is_required_output<Arg>::type,
  typename std::conditional<is_optional_input<Arg>::value, typename is_optional_input<Arg>::type,
  typename std::conditional<is_required_output<Arg>::value, typename is_required_output<Arg>::type,
  typename std::conditional<is_optional_ptr<Arg>::value, typename is_optional_ptr<Arg>::type,
  typename std::conditional<is_auto_out<Arg>::value, typename is_auto_out<Arg>::type,
  Arg
  >::type
  >::type
  >::type
  >::type
  >::type
  >::type
  type;

  static const bool is_input = is_required_input<Arg>::value || is_optional_input<Arg>::value || is_optional_ptr<Arg>::value;
  static const bool is_output = is_required_output<Arg>::value || is_optional_output<Arg>::value || is_auto_out<Arg>::value;
  static const bool is_required = is_required_input<Arg>::value || is_required_output<Arg>::value || (is_auto_out<Arg>::value && is_auto_out<Arg>::ready);
  static const bool is_optional = is_optional_input<Arg>::value || is_optional_output<Arg>::value || is_optional_ptr<Arg>::value || (is_auto_out<Arg>::value && !is_auto_out<Arg>::ready);
};

/// <summary>
/// Determines whether Arg is an allowed AutoFilter argument list.
/// An empty argument list is not allowed, and will return false.
/// </summary>
/// <remarks>
/// Only strict declaration types are supported.
/// At least one argument is required in order for the function to be valid.
/// If calling on instantiation is desired declare "const AutoPacket&"
/// as the only argument.
/// </remarks>
template<class... Args>
struct all_autofilter_args :
  std::false_type
{};
template<class Head, class... Tail>
struct all_autofilter_args<Head, Tail...> :
  std::integral_constant<bool, is_autofilter_arg<Head>::value && all_autofilter_args<Tail...>::value>
{};
template<class Head>
struct all_autofilter_args<Head> :
  is_autofilter_arg<Head>
{};

/// <summary>
/// Determines whether all arguments are inputs.
/// </summary>
/// <remarks>
/// This enables static validation of final-call AutoFilter functions.
/// </remarks>
template<class... Args>
struct all_autofilter_inputs :
  std::false_type
{};
template<class Head, class... Tail>
struct all_autofilter_inputs<Head, Tail...> :
  std::integral_constant<bool, is_autofilter_arg<Head>::is_input && all_autofilter_inputs<Tail...>::value>
{};

/// <summary>
/// Determines whether all arguments are inputs.
/// </summary>
/// <remarks>
/// This enables static validation of issue-call AutoFilter functions.
/// </remarks>
template<class... Args>
struct all_autofilter_outputs :
  std::false_type
{};
template<class Head, class... Tail>
struct all_autofilter_outputs<Head, Tail...> :
  std::integral_constant<bool, is_autofilter_arg<Head>::is_output && all_autofilter_outputs<Tail...>::value>
{};

/// <summary>
/// Determines whether the return value of a function is allowed for an AutoFilter:
/// - void
/// - Deferred
/// </summary>
template<class Ret>
struct is_autofilter_return :
  std::integral_constant<bool,
    std::is_same<void, Ret>::value ||
    std::is_same<Deferred, Ret>::value
  >
{};

///<summary>
/// Determines whether T has the type of an autofilter method
///</summary>
template <class T>
struct is_autofilter :
  std::false_type
{};
template <class Ret, class... Args>
struct is_autofilter<std::function<Ret(Args...)>> :
  std::integral_constant<bool, is_autofilter_return<Ret>::value && all_autofilter_args<Args...>::value>
{};

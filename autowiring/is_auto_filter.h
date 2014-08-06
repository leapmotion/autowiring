// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <functional>
#include TYPE_TRAITS_HEADER

class Deferred;
template<class T, bool auto_ready> class auto_out;

/// <summary>
/// Determines whether Arg is an instance of auto_out
/// </summary>
template<class Arg>
struct is_auto_out :
  std::false_type
{};
template<class Arg, bool auto_ready>
struct is_auto_out<auto_out<Arg, auto_ready>> :
  std::true_type
{};

/// <summary>
/// Determines whether Arg is an allowed AutoFilter argument:
/// - const input& output
/// - auto_out<ouput>&
/// </summary>
/// <remarks>
/// Only strict declaration types are supported.
/// The argument type "const AutoPacket&" is valid but "AutoPacket&" is not.
/// </remarks>
template<class Arg>
struct is_auto_filter_arg :
  std::integral_constant<bool,
    (std::is_reference<Arg>{} && std::is_const<typename std::remove_reference<Arg>::type>{}) ||
     is_auto_out<typename std::remove_reference<Arg>::type>{}
  >
{};

/// <summary>
/// Determines whether Arg is an allowed AutoFilter argument list.
/// </summary>
/// <remarks>
/// Only strict declaration types are supported.
/// At least one argument is required in order for the function to be valid.
/// If calling on instantiation is desired declare "const AutoPacket&"
/// as the only argument.
/// </remarks>
template<class... Args>
struct all_auto_filter_args :
  std::false_type
{};
template<class Head, class... Tail>
struct all_auto_filter_args<Head, Tail...> :
  std::integral_constant<bool, is_auto_filter_arg<Head>{} && all_auto_filter_args<Tail...>{}>
{};
template<class Head>
struct all_auto_filter_args<Head> :
  is_auto_filter_arg<Head>
{};

/// <summary>
/// Determines whether the return value of a function is allowed for an AutoFilter:
/// - void
/// - Deferred
/// </summary>
template<class Ret>
struct is_auto_filter_return :
  std::integral_constant<bool,
    std::is_same<void, Ret>{} ||
    std::is_same<Deferred, Ret>{}
  >
{};

///<summary>
/// Determines whether T has the type of an autofilter method
///</summary>
template <class T>
struct is_auto_filter :
  std::false_type
{};
template <class Ret, class... Args>
struct is_auto_filter<std::function<Ret(Args...)>> :
  std::integral_constant<bool, is_auto_filter_return<Ret>{} && all_auto_filter_args<Args...>{}>
{};

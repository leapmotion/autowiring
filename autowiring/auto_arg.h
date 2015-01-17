// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_in.h"
#include "auto_out.h"
#include "auto_prev.h"

/*
 The auto_arg<T> classes are used to generate of auto_in and auto_out types
 based on the argument T.
 The core functionality is that all argument types are treated as standard
 types, and can be re-derived from those standard types.
 It is expected that every parent of auto_arg will define:
 - id_type (an unqualified T)
 - base_type (a T qualified by const and/or &)
 - shared_type (the pointer T from which the parent inherits)
 Furthermore, by classifying arguments in terms of orientation and fundamental
 T, auto_arg enables the specification of a principal T to any class
 implementing an AutoFilter method.
 */

/// <summary>
/// Reinterpret copied argument as input
/// </summary>
template<class T>
class auto_arg
{
public:
  typedef auto_in<T> type;
  typedef auto_id<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_shared = false;
  static const int tshift = 0;
};

/// <summary>
/// Reinterpret copied argument as input
/// </summary>
template<class T>
class auto_arg<const T>:
  public auto_arg<T>
{};

/// <summary>
/// Specialization for "const T&" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<const T&> :
  public auto_arg<T>
{};

/// <summary>
/// Specialization for "std::shared_ptr<const T>" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<const T>>
{
public:
  typedef auto_in<const T> type;
  typedef auto_id<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_shared = true;
  static const int tshift = 0;
};

/// <summary>
/// Specialization for equivalent T auto_in<T>
/// </summary>
template<class T>
class auto_arg<auto_in<T>>:
  public auto_arg<T>
{};

/// <summary>
/// Specialization for "T&" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<T&> :
  public auto_out<T>
{
public:
  typedef auto_out<T> type;
  typedef auto_id<T> id_type;
  static const bool is_input = false;
  static const bool is_output = true;
  static const bool is_shared = false;
  static const int tshift = 0;
};

/// <summary>
/// Specialization for "std::shared_ptr<T>&" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<T>&>:
  public auto_arg<T&>
{};

/// <summary>
/// Forbidden input T
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<T>> {
  static_assert(
    std::is_same<T, AutoPacket>::value,
    "std::shared_ptr<AutoPacket> is the only T that may be supplied as a non-const shared input"
  );
};

/// <summary>
/// Specialization for equivalent T auto_out<T>
/// </summary>
template<class T>
class auto_arg<auto_out<T>>:
  public auto_arg<T&>
{};

template<class T, int N>
class auto_arg<auto_prev<T, N>>
{
public:
  typedef auto_prev<T, N> type;
  typedef auto_id<T> id_type;

  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_shared = false;
  static const int tshift = N;
};

/// <summary>
/// AutoPacket specialization
/// </summary>
/// <remarks>
/// This type is treated as an input type because it supports concurrent modification
/// </remarks>
template<>
class auto_arg<AutoPacket&>
{
public:
  typedef auto_in<AutoPacket> type;
  typedef AutoPacket id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_shared = false;
  static const int tshift = 0;
};

template<class T>
class auto_arg<const T*[]> :
  public auto_in<const T*[]>
{};

#pragma once

template<class T>
struct is_shared_ptr:
  public std::false_type
{};

template<class T>
struct is_shared_ptr<T&> :
  is_shared_ptr<T>
{};

template<class T>
struct is_shared_ptr<const T> :
  is_shared_ptr<T>
{};

template<class T>
struct is_shared_ptr<std::shared_ptr<T>> :
  public std::true_type
{};
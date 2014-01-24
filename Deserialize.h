#pragma once
#include TYPE_TRAITS_HEADER
#include <typeinfo>

/// <summary>
/// Support class for deserializing byte-streams input as std::string into a variety of formats
/// </summary>

namespace Auto{

template <typename T>
const std::string * deser(std::string & str){
  const std::string * ret(&str);
    return ret;
  }
  
}
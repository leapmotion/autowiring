#pragma once
#include TYPE_TRAITS_HEADER
#include <typeinfo>

/// <summary>
/// Support class for deserializing byte-streams input as std::string into a variety of formats
/// </summary>


namespace Auto{

  struct RefactorMeMarshal{

    virtual std::string AutoSerialize(void) = 0;
    virtual void AutoDeserialize(std::string data) = 0;

  };      

  template <typename T, typename Tag = T>
  struct deser;

  template <typename T>
  struct deser<T>{
    static const std::string * deserialize(std::string & str){
      assert(false);
      //This should never be called
      const std::string * ret(&str);
      return ret;
    }
  };

  template <>
  struct deser<const std::string *>{
    static const std::string * deserialize(std::string & str){
      const std::string * ret(&str);
      return ret;
    }
  };

  template <typename T>
  struct deser<T, typename std::enable_if<!std::is_base_of<Auto::RefactorMeMarshal, T>::value>::type>{
    static const std::string * deserialize(std::string & str){
      const std::string * ret(&str);
      return ret;
    }
  };
  
}
#pragma once
#include TYPE_TRAITS_HEADER
#include <typeinfo>

/// <summary>
/// Support class for deserializing byte-streams input as std::string into a variety of formats
/// </summary>


namespace Auto{

  struct SerializableSigil{};

  struct Serialize : public SerializableSigil{

    virtual std::string AutoSerialize(void) = 0;
    virtual void AutoDeserialize(std::string data) = 0;

  };

  template <typename T, typename Tag = T>
  struct deser;

  template <typename T>
  struct deser<T>{
    
    /// <summary>
    /// const std::string * Deserialization
    /// </summary>
    template <class X = T>
    static typename std::enable_if<std::is_same<const std::string *, X>::value, const std::string *>::type
    deserialize(std::string & str){
      const std::string * ret(&str);
      return ret;
    }

    /// <summary>
    /// Deserialization for types that do that thingAutoDeserialize
    /// </summary>
    template <class X = T>
    static typename std::enable_if<std::is_base_of<Serialize, X>::value, T>::type
    deserialize(std::string & str){
      T t;
      t.AutoDeserialize(str);
      return t;
    }
  };
  
  /// in
  /// A type that represents a parameter pack of zero or more integers.
  template<unsigned... Indices>
  struct index_tuple
  {
    /// Generate an index_tuple with an additional element.
    template<unsigned N>
    using append = index_tuple<Indices..., N>;
  };
  
  /// Unary metafunction that generates an index_tuple containing [0, Size)
  template<unsigned Size>
  struct make_index_tuple
  {
    typedef typename make_index_tuple<Size-1>::type::template append<Size-1>
    type;
  };
  
  // Terminal case of the recursive metafunction.
  template<>
  struct make_index_tuple<0u>
  {
    typedef index_tuple<> type;
  };
  
  template<typename... Types>
  using to_index_tuple = typename make_index_tuple<sizeof...(Types)>::type;

}
#pragma once
#include TYPE_TRAITS_HEADER

/// <summary>
/// Support class for deserializing byte-streams input as std::string into a variety of formats
/// </summary>
namespace Auto {
  struct SerializableSigil{};

  struct Serialize:
    public SerializableSigil
  {
    virtual std::string AutoSerialize(void) = 0;
    virtual void AutoDeserialize(std::string data) = 0;
  };

  template <typename T, typename Tag = T>
  struct deser;

  template<typename T>
  struct deser<T> {
    /// <summary>
    /// const std::string * Deserialization
    /// </summary>
    template <class X = T>
    static typename std::enable_if<std::is_same<const std::string *, X>::value, const std::string *>::type
    deserialize(std::string & str){
      return &str;
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
  
  
  /// <summary>
  /// Utility type which enables the composition of a sequence [0, sizeof...(Ts))
  /// </summary>
  template<unsigned ...Indicies>
  struct index_tuple {
    
    /// Generate an index_tuple with an additional element.
    template<unsigned N>
    struct append {
      typedef index_tuple<Indicies..., N> type;
    };
  };
  
  template<typename ...Members>
  struct make_index_tuple; //So linux doesn't bitch
  
  /// Unary metafunction that generates an index_tuple containing [0, Size)
  template<typename Head, typename ...Tail>
  struct make_index_tuple<Head, Tail...>{
    typedef typename make_index_tuple<Tail...>::type::template append<sizeof...(Tail)>::type type;
  };
  
  // Terminal case of the recursive metafunction.
  template<>
  struct make_index_tuple<>{
    typedef index_tuple<> type;
  };
  
  static_assert(
    std::is_same<
      make_index_tuple<>::type,
      index_tuple<>
    >::value,
    "Index tuple base case was not correctly specialized"
  );

  static_assert(
    std::is_same<
      make_index_tuple<int, int, int>::type,
      index_tuple<0, 1, 2>
    >::value,
    "Index tuple conversion function did not properly index a sample type"
  );
}
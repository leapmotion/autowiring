// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include <atomic>
#include <initializer_list>
#include <cmath>
#include <limits>
#include <stdexcept>

#include <string>
#include <string.h>
#include <stdlib.h>
#include "double-conversion/double-conversion.h"
#include TYPE_TRAITS_HEADER

namespace autowiring {
  struct invalid_marshal_base {};

  template<typename T>
  struct marshaller;

  class ConfigManager;

  /// <summary>
  /// The interface all marshallers must support
  /// </summary>
  struct marshaller_base {
    /// <summary>
    /// Marshals the underlying type to a string value
    /// </summary>
    virtual std::string marshal(const void* ptr) const { return ""; }

    /// <summary>
    /// Converts the specified string value to the output type
    /// </summary>
    /// <param name="szValue">The string value to be converted</param>
    /// <param name="ptr">A pointer to the memory region where the output will be stored</param>
    virtual void unmarshal(void* ptr, const char* szValue) const = 0;

    /// <summary>
    /// Copies the value on the right-hand side to the left-hand side without translation.
    /// </summary>
    /// <param name="lhs">The destination for the assignment</param>
    /// <param name="rhs">The source of the copy</param>
    /// <remarks>
    /// As with all other methods on this interface, this operation assumes that both objects
    /// are fully initialized and are pointers to the correct type.
    ///
    /// Implementations of this method make use of the assignment operator to perform the copy.
    /// </remarks>
    virtual void copy(void* lhs, const void* rhs) const = 0;

    /// <summary>
    /// Notification passed by the ConfigManager when a field has been attached
    /// </summary>
    /// <remarks>
    /// When a configurable object is added to a context, each field in the object is processed one
    /// at a time and then bound to the context's ConfigManager.  After being bound, each field's
    /// marshaller is given the opportunity to consider the newly created field together with the
    /// ConfigManager.
    ///
    /// One use case for this is allowing config fields to back-propagate changes on themselves to
    /// the owning ConfigManager.
    /// </remarks>
    virtual void attach(ConfigManager& manager, void* pField) const {};
  };

  /// <summary>
  /// Built-in marshaller, declared separately because it's a little more complicated
  /// </summary>
  template<typename T, typename>
  struct builtin_marshaller :
    invalid_marshal_base
  {};

  template<>
  struct builtin_marshaller<bool, void> :
    marshaller_base
  {
    typedef bool type;

    std::string marshal(const void* ptr) const override {
      return *static_cast<const bool*>(ptr) ? "true" : "false";
    }

    void unmarshal(void* ptr, const char* szValue) const override {
      if (!strcmp("true", szValue))
        *static_cast<bool*>(ptr) = true;
      else if (!strcmp("false", szValue))
        *static_cast<bool*>(ptr) = false;
      else
        throw std::invalid_argument("Boolean unmarshaller expects true or false keyword");
    }

    void copy(void* lhs, const void* rhs) const override {
      *static_cast<bool*>(lhs) = *static_cast<const bool*>(rhs);
    }
  };

  template<>
  struct builtin_marshaller<volatile bool, void> :
    builtin_marshaller<bool, void>
  {};

  // Needed to avoid annoying signed/unsigned warnings due to strtoull
  template<typename T, typename = void>
  struct integral_converter;

  template<typename T>
  struct integral_converter<T, typename std::enable_if<std::is_signed<T>::value>::type> {
    static T convert(const char* p) {
      char* end = nullptr;
      return static_cast<T>(strtoll(p, &end, 10));
    }
  };

  template<typename T>
  struct integral_converter<T, typename std::enable_if<std::is_unsigned<T>::value>::type> {
    static T convert(const char* p) {
      // Trim
      while (isspace(*p))
        p++;

      // This is the reason we're trimming--no signed numbers in unsigned deserialization
      if (*p == '-')
        throw std::range_error("Underflow error, attempted to set a signed value in an unsigned field");

      char* end = nullptr;
      return static_cast<T>(strtoull(p, &end, 10));
    }
  };

  template<typename T>
  struct builtin_marshaller<T, typename std::enable_if<std::is_integral<T>::value>::type> :
    marshaller_base
  {
    typedef typename std::remove_volatile<T>::type type;

    std::string marshal(const void* ptr) const override {
      type val = *static_cast<const type*>(ptr);
      return std::to_string(val);
    }

    void unmarshal(void* ptr, const char* szValue) const override {
      const auto value = integral_converter<typename std::decay<T>::type>::convert(szValue);
      if (value > std::numeric_limits<type>::max() || value < std::numeric_limits<type>::min())
        throw std::range_error("Overflow error, value is outside the range representable by this type.");
      *static_cast<type*>(ptr) = value;
    }

    void copy(void* lhs, const void* rhs) const override {
      *static_cast<T*>(lhs) = *static_cast<const T*>(rhs);
    }
  };

  template<typename T>
  struct float_converter;

  template<>
  struct float_converter<float> {
    static float convert(const char* szValue) {
      double_conversion::StringToDoubleConverter c(
        double_conversion::StringToDoubleConverter::ALLOW_LEADING_SPACES |
        double_conversion::StringToDoubleConverter::ALLOW_TRAILING_SPACES |
        double_conversion::StringToDoubleConverter::ALLOW_SPACES_AFTER_SIGN, 0, 0, "Infinity", "NaN");
      int count;
      return c.StringToFloat(szValue, static_cast<int>(strlen(szValue)), &count);
    }
  };

  template<>
  struct float_converter<double> {
    static double convert(const char* szValue) {
      double_conversion::StringToDoubleConverter c(
        double_conversion::StringToDoubleConverter::ALLOW_LEADING_SPACES |
        double_conversion::StringToDoubleConverter::ALLOW_TRAILING_SPACES |
        double_conversion::StringToDoubleConverter::ALLOW_SPACES_AFTER_SIGN, 0, 0, "Infinity", "NaN");
      int count;
      return c.StringToDouble(szValue, static_cast<int>(strlen(szValue)), &count);
    }
  };

  template<typename T>
  struct builtin_marshaller<T, typename std::enable_if<std::is_floating_point<T>::value>::type> :
    marshaller_base
  {
    typedef typename std::remove_volatile<T>::type type;

    std::string marshal(const void* ptr) const override {
      type value = *static_cast<const type*>(ptr);
      auto& dc = double_conversion::DoubleToStringConverter::EcmaScriptConverter();
      char buffer[128];
      double_conversion::StringBuilder builder(buffer, sizeof(buffer));
      dc.ToShortest(value, &builder);
      builder.Finalize();

      return std::string(buffer, builder.size());
    }

    void unmarshal(void* ptr, const char* szValue) const override {
      T& value = *static_cast<type*>(ptr);
      value = float_converter<type>::convert(szValue);
    }

    void copy(void* lhs, const void* rhs) const override {
      *static_cast<T*>(lhs) = *static_cast<const T*>(rhs);
    }
  };

  /// <summary>
  /// Default marshaller, a point of specialization for external users
  /// </summary>
  template<typename T>
  struct marshaller :
    builtin_marshaller<T, void>
  {};

  ///
  /// Nonprimitive or full specializations follow from here.  Use these as examples when devising
  /// your own marshallers.
  ///

  template<typename T>
  struct marshaller<std::atomic<T>> :
    marshaller_base
  {
    typedef std::atomic<T> type;

    // Marshaller for the interior type
    marshaller<T> interior;

    std::string marshal(const void* ptr) const override {
      T value = static_cast<const type*>(ptr)->load();
      return interior.marshal(&value);
    }

    void unmarshal(void* ptr, const char* szValue) const override {
      T value;
      interior.unmarshal(&value, szValue);
      *static_cast<type*>(ptr) = std::move(value);
    }

    void copy(void* lhs, const void* rhs) const override {
      *static_cast<std::atomic<T>*>(lhs) = static_cast<const std::atomic<T>*>(rhs)->load();
    }
  };

  template<>
  struct marshaller<std::string> :
    marshaller_base
  {
    typedef std::string type;

    std::string marshal(const void* ptr) const override {
      return *static_cast<const type*>(ptr);
    }

    void unmarshal(void* ptr, const char* szValue) const override {
      *static_cast<std::string*>(ptr) = szValue;
    }

    void copy(void* lhs, const void* rhs) const override {
      *static_cast<std::string*>(lhs) = *static_cast<const std::string*>(rhs);
    }
  };
}

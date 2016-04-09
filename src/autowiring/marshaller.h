// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <atomic>
#include <initializer_list>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <string.h>
#include TYPE_TRAITS_HEADER

namespace autowiring {
  struct invalid_marshal_base {};

  template<typename T>
  struct marshaller;

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
  };

  template<>
  struct builtin_marshaller<volatile bool, void> :
    builtin_marshaller<bool, void>
  {};

  template<typename T>
  struct builtin_marshaller<T, typename std::enable_if<std::is_integral<T>::value>::type> :
    marshaller_base
  {
    typedef typename std::remove_volatile<T>::type type;

    std::string marshal(const void* ptr) const override {
      std::string retVal;
      type val = *static_cast<const type*>(ptr);
      bool pos = 0 < val;
      for (; val; val /= 10) {
        retVal.push_back(static_cast<char>(val % 10 + '0'));
      }
      if (!pos)
        retVal.push_back('-');

      for (
        auto first = retVal.begin(), last = retVal.end();
        (first != last) && (first != --last);
        ++first
      )
        std::swap(*first, *last);
      return retVal;
    }

    void unmarshal(void* ptr, const char* szValue) const override {
      type& value = *static_cast<type*>(ptr);
      bool negative = *szValue == '-';
      if(negative) {
        // Skip over the sign, verify we aren't assigning to the wrong field type
        szValue++;
        if (std::is_unsigned<type>::value)
          throw std::range_error("Attempted to set a signed value on an unsigned calibration field");
      }

      for (value = 0; *szValue; szValue++) {
        if (*szValue < '0' || '9' < *szValue)
          throw std::invalid_argument("String value is not an integer");
        value = *szValue - '0' + value * 10;
      }
      if(negative)
        value *= -1;
    }
  };

  template<typename T>
  struct builtin_marshaller<T, typename std::enable_if<std::is_floating_point<T>::value>::type> :
    marshaller_base
  {
    typedef typename std::remove_volatile<T>::type type;

    std::string marshal(const void* ptr) const override {
      std::string retVal;
      type value = *static_cast<const type*>(ptr);
      if (value == 0.0f)
        return "0";

      bool neg = value < 0.0f;
      if (neg)
        value *= -1.0f;

      // Convert input value to scientific notation
      int power = static_cast<int>(std::log10(value));

      // We will be assembling the number backwards, need to keep track of the
      // index of the current digit in the reassembled number.
      int curDigit = std::numeric_limits<type>::digits10 - power;

      // We only want a certain number of digits, this digit count will fit in
      // a large integer and elimintes the loss of precision we experience when
      // using floating point math to try to do digit shifts
      uint64_t digits = static_cast<uint64_t>(value * std::pow(10.0, curDigit) + 0.5);

      // Trailing zero introduction for integer multiples of 10
      if (power > std::numeric_limits<type>::digits10)
        retVal.append(power - std::numeric_limits<type>::digits10, '0');

      // Trailing zero omission
      while (0 < curDigit && 0 == (digits % 10)) {
        digits /= 10;
        curDigit--;
      }

      for(; digits; digits /= 10, curDigit--) {
        char digit = static_cast<char>(digits % 10);

        // String conversion step, straightforward mapping
        retVal.push_back('0' + digit);
        if (curDigit == 1)
          retVal.push_back('.');

        if (!digits)
          // Short-circuit for precise representations
          break;
      }

      // Zeroes before the decimal:
      if (power < 0) {
        retVal.append(-power, '0');
        retVal.append(".0");
      }

      if (neg)
        retVal.push_back('-');
      for (
        auto first = retVal.begin(), last = retVal.end();
        (first != last) && (first != --last);
        ++first
      )
        std::swap(*first, *last);
      return retVal;
    }

    void unmarshal(void* ptr, const char* szValue) const override {
      T& value = *static_cast<type*>(ptr);
      bool negative = *szValue == '-';
      if (negative)
        szValue++;

      uint64_t whole = 0;
      for (; *szValue; szValue++) {
        // Detect the decimal marker, switch to fractional part
        if (*szValue == '.') {
          szValue++;
          break;
        }
        if (*szValue < '0' || '9' < *szValue)
          throw std::invalid_argument("String value is not a decimal number");
        whole = whole * 10 + *szValue - '0';
      }

      uint64_t fractional = 0;
      size_t n = 0;
      for (; szValue[n]; n++) {
        if (szValue[n] < '0' || '9' < szValue[n])
          throw std::invalid_argument("String value is not a decimal number");
        fractional = fractional * 10 + szValue[n] - '0';
      }
      value = static_cast<type>(fractional);
      while(n--)
        value /= 10.0f;
      value += static_cast<type>(whole);
      if (negative)
        value *= -1.0f;
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
  };
}

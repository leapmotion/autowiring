#pragma once
#include <atomic>
#include <type_traits>

namespace autowiring {
  template<typename T>
  struct tweak {
    static_assert(std::is_arithmetic<T>::value, "Cannot define a tweakable value that is not arithmetic");

    tweak(void) = default;
    tweak(const tweak& rhs) = default;
    tweak(T value) :
      value(value),
      last_observed(value)
    {}

  private:
    // Current value, advertised both outside and updated
    volatile T value = T{};

    // Last observed value
    volatile T last_observed = T{};

  public:
    // Accessor methods:
    T get(void) const volatile { return value; }
    volatile T& get(void) volatile { return value; }

    /// <returns>
    /// True if the entry is dirty
    /// </returns>
    bool is_dirty(void) const volatile {
      return value != last_observed;
    }

    /// <summary>
    /// Updates the assignment version, clearing the dirty flag
    /// </summary>
    /// <returns>
    /// True if this instance was dirty
    /// </returns>
    bool clear_dirty(void) volatile {
      if(value == last_observed)
        return false;
      last_observed = value;
      return true;
    }

    // Operator overloads:
    volatile tweak& operator=(T rhs) volatile {
      value = rhs;
      return *this;
    }
    volatile tweak& operator=(const tweak& rhs) volatile {
      value = rhs.value;
      return *this;
    }
    operator T(void) volatile const {
      return value;
    }
  };

  template<typename T>
  struct marshaller<tweak<T>> :
    marshaller_base
  {
    typedef tweak<T> type;

    // Marshaller for the interior type
    marshaller<T> interior;

    std::string marshal(const void* ptr) const override {
      T value = static_cast<const type*>(ptr)->get();
      return interior.marshal(&value);
    }

    void unmarshal(void* ptr, const char* szValue) const override {
      T value;
      interior.unmarshal(&value, szValue);
      *static_cast<type*>(ptr) = std::move(value);
    }
  };
}

// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

// This is a hexadecimal mapping table, represented as a string.  It's based at 0x30, the charcode for '0'
// This table helps convert hex strings into their numeric representation at compile time.
#define UUID_MAPPING_TABLE "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09_______\x0a\x0b\x0c\x0d\x0e\x0f__________________________\x0a\x0b\x0c\x0d\x0e\x0f"

// Arrangement makers:
#define MAKE_1_INDIRECT(tab, name, base, shift) ((unsigned long long)(unsigned char&)tab[name[0 + base] - '0'] << ((shift) * 4))
#define MAKE_2_INDIRECT(tab, name, base, shift) MAKE_1_INDIRECT(tab, name, base, shift + 1) | MAKE_1_INDIRECT(tab, name, base + 1, shift)
#define MAKE_4_INDIRECT(tab, name, base, shift) MAKE_2_INDIRECT(tab, name, base, shift + 2) | MAKE_2_INDIRECT(tab, name, base + 2, shift)
#define MAKE_8_INDIRECT(tab, name, base, shift) MAKE_4_INDIRECT(tab, name, base, shift + 4) | MAKE_4_INDIRECT(tab, name, base + 4, shift)

#define MAKE_16_INDIRECT(tab, name, base) \
  MAKE_2_INDIRECT(tab, name, base +  0,  0) | MAKE_2_INDIRECT(tab, name, base + 2,  2) | \
  MAKE_2_INDIRECT(tab, name, base +  5,  4) | MAKE_2_INDIRECT(tab, name, base + 7,  6) | \
  MAKE_2_INDIRECT(tab, name, base +  9,  8) | MAKE_2_INDIRECT(tab, name, base + 11, 10) | \
  MAKE_2_INDIRECT(tab, name, base + 13, 12) | MAKE_2_INDIRECT(tab, name, base + 15, 14)

/// <summary>
/// Composes a uuid structure from a uuid string, forming four comma-separated expressions
/// </summary>
/// <remarks>
/// The expected uuid is of the following form:
///
///  01234567-89AB-CDEF-fedc-ba9876543210
///
///  It is parsed out as
///  [01234567]L-[89AB]S-[CDEF]S- [fedc ba9876543210]LL
/// Hexidecimal characters may be uppercase or lowercase.  Dashes are required, and there must not be any offsets.
/// </remarks>
#define COMPOSE_UUID(name, base) \
    MAKE_8_INDIRECT(UUID_MAPPING_TABLE, name, base + 0, 0), \
    MAKE_4_INDIRECT(UUID_MAPPING_TABLE, name, base + 9, 0), \
    MAKE_4_INDIRECT(UUID_MAPPING_TABLE, name, base + 14, 0), \
    MAKE_16_INDIRECT(UUID_MAPPING_TABLE, name, base + 19) \

// TODO:  Replace this with defined(_MSC_VER) when the linux implementation has been tested
#if 0
#define DECL_UUID(id) __declspec(uuid(id))
#define PLATFORM_UUID_OF __uuidof
#else
#define DECL_UUID(id)
#define PLATFORM_UUID_OF(clazz) uuid_of<clazz>::Uuid()
#endif

/// <summary>
/// Provides the uuid of a specific type as a constant expression
/// </summary>
#define UUID_OF(clazz) PLATFORM_UUID_OF(clazz)

/// <summary>
/// Provides the uuid of a specific type as a string literal
/// </summary>
#define UUID_OF_STR(clazz) uuid_of<clazz>::UuidStr()

/// <summary>
/// Evaluates to "true" if the specified class has a UUID defined
/// </summary>
#define UUID_KNOWN(clazz) uuid_of<clazz>::value

#ifndef _MSC_VER
struct UUID {
  unsigned long  Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned long long Data4;
};
#endif

// TODO:  The UUID structure is the type of the result of the __uuidof keyword, so we have to use it
// (or something like it) here.  It might be helpful to find a solution that doesn't require the use
// of this name, as not all consumers of this header will be including Windows.h
struct uuid
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned long long Data4;
  /// <summary>
  /// Trivial constructor
  /// </summary>
  uuid(void) {}

  /// <summary>
  /// Equality Operators [stubbed for unit-testing, need better implementations]
  /// </summary>

  bool friend operator== (const uuid& lhs, const uuid& rhs){
    return  (lhs.Data1 == rhs.Data1) && 
            (lhs.Data2 == rhs.Data2) &&
            (lhs.Data3 == rhs.Data3) && 
            (lhs.Data4 == rhs.Data4);
  }
  bool friend operator!= (const uuid& lhs, const uuid& rhs){return !(lhs == rhs);}

  /// <summary>
  /// Convenience constructor:
  /// </summary>
  uuid(unsigned long Data1,unsigned short Data2,unsigned short Data3, unsigned long long Data4)
  {
    this->Data1 = Data1;
    this->Data2 = Data2;
    this->Data3 = Data3;
    (long long&)this->Data4 = Data4;
  }

  /// <summary>
  /// Initializes a UUID from a string literal value
  /// </summary>
  /// <remarks>
  /// Initializes UUIDs of the form:
  ///  58910B93-C20B-42C2-AD8E-B80587D89D87
  ///</remarks>
  uuid(const char (&val)[37]) {
    *this = uuid(COMPOSE_UUID(val, 0));
  }

  /// <summary>
  /// Initializes a UUID from a string literal value
  /// </summary>
  /// <remarks>
  /// Initializes UUIDs of the form:
  ///  {58910B93-C20B-42C2-AD8E-B80587D89D87}
  ///</remarks>
  uuid(const char (&val)[39]) {
    *this = uuid(COMPOSE_UUID(val, 1));
  }
};

/// <summary>
/// Macro defines a template specialization of uuid_of
/// Allows the association of a uuid with a class
/// </summary>
#define DECLARE_UUID(clazz, id) \
  class clazz; \
  template<> \
  struct uuid_of<clazz> \
  { \
    static const bool value = true; \
    static const char* UuidStr(void) {return id;} \
    static const uuid Uuid(void) { return uuid(id); }; \
  }; \
  class DECL_UUID(id) clazz

/// <summary>
/// Non-macro defined fall-thrrough case for uuid_of. Gets called on types
/// which were not previously declared with DECLARE_UUID
/// Allows the association of a uuid with a class
/// </summary>
template<class T>
struct uuid_of
{
  static const bool value = false;
  static const char* UuidStr(void) {return nullptr;}

  static uuid Uuid(void) {
    return uuid(0, 0, 0, 0);
  }
};

/// <summary>
/// Functor datatype which can be used for unordered maps
/// </summary>
struct uuid_hash {
  bool operator()(const uuid& lh, const uuid& rh) const {
    return
      (const long long&)lh.Data1 == (const long long&)rh.Data1 &&
      (const long long&)lh.Data4 == (const long long&)rh.Data4;
  };

  long long operator()(const uuid& val) const {
    return (long long&)val.Data4;
  }
};

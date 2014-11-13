#pragma once
#include <iosfwd>

namespace autowiring {
/// <summary>
/// Expectation type, used in utility processing
/// </summary>
template<class ch>
class _expect {
public:
  typedef std::basic_istream<ch, std::char_traits<ch>> stream;

  _expect(const ch* expected) :
    m_expected(expected)
  {
  }

private:
  const ch* m_expected;

public:
  stream& operator()(stream& lhs) const {
    ch c;
    for(size_t i = 0; m_expected[i] && (lhs >> c); i++)
      if(toupper(m_expected[i]) != toupper(c))
        lhs.setstate(std::ios::failbit);
    return lhs;
  }
};

template<class ch>
static typename _expect<ch>::stream& operator>>(typename _expect<ch>::stream& lhs, const _expect<ch>& rhs) {
  return rhs(lhs);
}

template<class ch>
_expect<ch> expect(const ch* str) {
  return _expect<ch>(str);
}
}
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_text_iprimitive.ipp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // size_t, NULL
#include <limits> // NULL

#include <autoboost/config.hpp>
#if defined(AUTOBOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif

#include <autoboost/serialization/throw_exception.hpp>

#include <autoboost/archive/basic_text_iprimitive.hpp>

#include <autoboost/archive/iterators/remove_whitespace.hpp>
#include <autoboost/archive/iterators/istream_iterator.hpp>
#include <autoboost/archive/iterators/binary_from_base64.hpp>
#include <autoboost/archive/iterators/transform_width.hpp>

namespace autoboost {
namespace archive {

namespace detail {
    template<class CharType>
    static inline bool is_whitespace(CharType c);

    template<>
    inline bool is_whitespace(char t){
        return 0 != std::isspace(t);
    }

    #ifndef AUTOBOOST_NO_CWCHAR
    template<>
    inline bool is_whitespace(wchar_t t){
        return 0 != std::iswspace(t);
    }
    #endif
} // detail

// translate base64 text into binary and copy into buffer
// until buffer is full.
template<class IStream>
AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL void
basic_text_iprimitive<IStream>::load_binary(
    void *address,
    std::size_t count
){
    typedef typename IStream::char_type CharType;

    if(0 == count)
        return;

    AUTOBOOST_ASSERT(
        static_cast<std::size_t>((std::numeric_limits<std::streamsize>::max)())
        > (count + sizeof(CharType) - 1)/sizeof(CharType)
    );

    if(is.fail())
        autoboost::serialization::throw_exception(
            archive_exception(archive_exception::input_stream_error)
        );
    // convert from base64 to binary
    typedef typename
        iterators::transform_width<
            iterators::binary_from_base64<
                iterators::remove_whitespace<
                    iterators::istream_iterator<CharType>
                >
                ,typename IStream::int_type
            >
            ,8
            ,6
            ,CharType
        >
        binary;

    binary i = binary(iterators::istream_iterator<CharType>(is));

    char * caddr = static_cast<char *>(address);

    // take care that we don't increment anymore than necessary
    while(count-- > 0){
        *caddr++ = static_cast<char>(*i++);
    }

    // skip over any excess input
    for(;;){
        typename IStream::int_type r;
        r = is.get();
        if(is.eof())
            break;
        if(detail::is_whitespace(static_cast<CharType>(r)))
            break;
    }
}

template<class IStream>
AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL
basic_text_iprimitive<IStream>::basic_text_iprimitive(
    IStream  &is_,
    bool no_codecvt
) :
    is(is_),
    flags_saver(is_),
    precision_saver(is_),
#ifndef AUTOBOOST_NO_STD_LOCALE
    codecvt_null_facet(1),
    archive_locale(is.getloc(), & codecvt_null_facet),
    locale_saver(is)
{
    if(! no_codecvt){
        is_.sync();
        is_.imbue(archive_locale);
    }
    is_ >> std::noboolalpha;
}
#else
{}
#endif

template<class IStream>
AUTOBOOST_ARCHIVE_OR_WARCHIVE_DECL
basic_text_iprimitive<IStream>::~basic_text_iprimitive(){
}

} // namespace archive
} // namespace autoboost

/* autoboost random/detail/seed.hpp header file
 *
 * Copyright Steven Watanabe 2009
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id$
 */

#ifndef AUTOBOOST_RANDOM_DETAIL_SEED_HPP
#define AUTOBOOST_RANDOM_DETAIL_SEED_HPP

#include <autoboost/config.hpp>

// Sun seems to have trouble with the use of SFINAE for the
// templated constructor.  So does Borland.
#if !defined(AUTOBOOST_NO_SFINAE) && !defined(__SUNPRO_CC) && !defined(__BORLANDC__)

#include <autoboost/utility/enable_if.hpp>
#include <autoboost/type_traits/is_arithmetic.hpp>

namespace autoboost {
namespace random {
namespace detail {

template<class T>
struct disable_seed : autoboost::disable_if<autoboost::is_arithmetic<T> > {};

template<class Engine, class T>
struct disable_constructor : disable_seed<T> {};

template<class Engine>
struct disable_constructor<Engine, Engine> {};

#define AUTOBOOST_RANDOM_DETAIL_GENERATOR_CONSTRUCTOR(Self, Generator, gen) \
    template<class Generator>                                           \
    explicit Self(Generator& gen, typename ::autoboost::random::detail::disable_constructor<Self, Generator>::type* = 0)

#define AUTOBOOST_RANDOM_DETAIL_GENERATOR_SEED(Self, Generator, gen)    \
    template<class Generator>                                       \
    void seed(Generator& gen, typename ::autoboost::random::detail::disable_seed<Generator>::type* = 0)

#define AUTOBOOST_RANDOM_DETAIL_SEED_SEQ_CONSTRUCTOR(Self, SeedSeq, seq)    \
    template<class SeedSeq>                                             \
    explicit Self(SeedSeq& seq, typename ::autoboost::random::detail::disable_constructor<Self, SeedSeq>::type* = 0)

#define AUTOBOOST_RANDOM_DETAIL_SEED_SEQ_SEED(Self, SeedSeq, seq)   \
    template<class SeedSeq>                                     \
    void seed(SeedSeq& seq, typename ::autoboost::random::detail::disable_seed<SeedSeq>::type* = 0)

#define AUTOBOOST_RANDOM_DETAIL_ARITHMETIC_CONSTRUCTOR(Self, T, x)  \
    explicit Self(const T& x)

#define AUTOBOOST_RANDOM_DETAIL_ARITHMETIC_SEED(Self, T, x) \
    void seed(const T& x)
}
}
}

#else

#include <autoboost/type_traits/is_arithmetic.hpp>
#include <autoboost/mpl/bool.hpp>

#define AUTOBOOST_RANDOM_DETAIL_GENERATOR_CONSTRUCTOR(Self, Generator, gen) \
    Self(Self& other) { *this = other; }                                \
    Self(const Self& other) { *this = other; }                          \
    template<class Generator>                                           \
    explicit Self(Generator& gen) {                                     \
        autoboost_random_constructor_impl(gen, ::autoboost::is_arithmetic<Generator>());\
    }                                                                   \
    template<class Generator>                                           \
    void autoboost_random_constructor_impl(Generator& gen, ::autoboost::mpl::false_)

#define AUTOBOOST_RANDOM_DETAIL_GENERATOR_SEED(Self, Generator, gen)    \
    template<class Generator>                                       \
    void seed(Generator& gen) {                                     \
        autoboost_random_seed_impl(gen, ::autoboost::is_arithmetic<Generator>());\
    }\
    template<class Generator>\
    void autoboost_random_seed_impl(Generator& gen, ::autoboost::mpl::false_)

#define AUTOBOOST_RANDOM_DETAIL_SEED_SEQ_CONSTRUCTOR(Self, SeedSeq, seq)    \
    Self(Self& other) { *this = other; }                                \
    Self(const Self& other) { *this = other; }                          \
    template<class SeedSeq>                                             \
    explicit Self(SeedSeq& seq) {                                       \
        autoboost_random_constructor_impl(seq, ::autoboost::is_arithmetic<SeedSeq>());\
    }                                                                   \
    template<class SeedSeq>                                             \
    void autoboost_random_constructor_impl(SeedSeq& seq, ::autoboost::mpl::false_)

#define AUTOBOOST_RANDOM_DETAIL_SEED_SEQ_SEED(Self, SeedSeq, seq)           \
    template<class SeedSeq>                                             \
    void seed(SeedSeq& seq) {                                           \
        autoboost_random_seed_impl(seq, ::autoboost::is_arithmetic<SeedSeq>()); \
    }                                                                   \
    template<class SeedSeq>                                             \
    void autoboost_random_seed_impl(SeedSeq& seq, ::autoboost::mpl::false_)

#define AUTOBOOST_RANDOM_DETAIL_ARITHMETIC_CONSTRUCTOR(Self, T, x)  \
    explicit Self(const T& x) { autoboost_random_constructor_impl(x, ::autoboost::mpl::true_()); }\
    void autoboost_random_constructor_impl(const T& x, ::autoboost::mpl::true_)

#define AUTOBOOST_RANDOM_DETAIL_ARITHMETIC_SEED(Self, T, x) \
    void seed(const T& x) { autoboost_random_seed_impl(x, ::autoboost::mpl::true_()); }\
    void autoboost_random_seed_impl(const T& x, ::autoboost::mpl::true_)

#endif

#endif

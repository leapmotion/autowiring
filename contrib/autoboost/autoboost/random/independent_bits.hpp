/* autoboost random/independent_bits.hpp header file
 *
 * Copyright Steven Watanabe 2011
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id$
 *
 */

#ifndef AUTOBOOST_RANDOM_INDEPENDENT_BITS_HPP
#define AUTOBOOST_RANDOM_INDEPENDENT_BITS_HPP

#include <istream>
#include <iosfwd>
#include <autoboost/assert.hpp>
#include <autoboost/limits.hpp>
#include <autoboost/config.hpp>
#include <autoboost/cstdint.hpp>
#include <autoboost/integer/integer_mask.hpp>
#include <autoboost/random/traits.hpp>
#include <autoboost/random/detail/config.hpp>
#include <autoboost/random/detail/integer_log2.hpp>
#include <autoboost/random/detail/operators.hpp>
#include <autoboost/random/detail/seed.hpp>
#include <autoboost/random/detail/seed_impl.hpp>
#include <autoboost/random/detail/signed_unsigned_tools.hpp>

namespace autoboost {
namespace random {

/**
 * An instantiation of class template @c independent_bits_engine
 * model a \pseudo_random_number_generator.  It generates random
 * numbers distributed between [0, 2^w) by combining one or
 * more invocations of the base engine.
 *
 * Requires: 0 < w <= std::numeric_limits<UIntType>::digits
 */
template<class Engine, std::size_t w, class UIntType>
class independent_bits_engine
{
public:
    typedef Engine base_type;
    typedef UIntType result_type;
    typedef typename Engine::result_type base_result_type;

    // Required by old Boost.Random concept
    AUTOBOOST_STATIC_CONSTANT(bool, has_fixed_range = false);

    /** Returns the smallest value that the generator can produce. */
    static result_type min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION ()
    { return 0; }
    /** Returns the largest value that the generator can produce. */
    static result_type max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION ()
    { return max_imp(autoboost::is_integral<UIntType>()); }

    /**
     * Constructs an @c independent_bits_engine using the
     * default constructor of the base generator.
     */
    independent_bits_engine() { }

    /**
     * Constructs an @c independent_bits_engine, using seed as
     * the constructor argument for both base generators.
     */
    AUTOBOOST_RANDOM_DETAIL_ARITHMETIC_CONSTRUCTOR(independent_bits_engine,
        base_result_type, seed_arg)
    {
        _base.seed(seed_arg);
    }

    /**
     * Constructs an @c independent_bits_engine, using seq as
     * the constructor argument for the base generator.
     */
    AUTOBOOST_RANDOM_DETAIL_SEED_SEQ_CONSTRUCTOR(independent_bits_engine,
        SeedSeq, seq)
    { _base.seed(seq); }

    /** Constructs an @c independent_bits_engine by copying @c base. */
    independent_bits_engine(const base_type& base_arg) : _base(base_arg) {}

    /**
     * Contructs an @c independent_bits_engine with
     * values from the range defined by the input iterators first
     * and last.  first will be modified to point to the element
     * after the last one used.
     *
     * Throws: @c std::invalid_argument if the input range is too small.
     *
     * Exception Safety: Basic
     */
    template<class It>
    independent_bits_engine(It& first, It last) : _base(first, last) { }

    /**
     * Seeds an @c independent_bits_engine using the default
     * seed of the base generator.
     */
    void seed() { _base.seed(); }

    /**
     * Seeds an @c independent_bits_engine, using @c seed as the
     * seed for the base generator.
     */
    AUTOBOOST_RANDOM_DETAIL_ARITHMETIC_SEED(independent_bits_engine,
        base_result_type, seed_arg)
    { _base.seed(seed_arg); }

    /**
     * Seeds an @c independent_bits_engine, using @c seq to
     * seed the base generator.
     */
    AUTOBOOST_RANDOM_DETAIL_SEED_SEQ_SEED(independent_bits_engine,
        SeedSeq, seq)
    { _base.seed(seq); }

    /**
     * Seeds an @c independent_bits_engine with
     * values from the range defined by the input iterators first
     * and last.  first will be modified to point to the element
     * after the last one used.
     *
     * Throws: @c std::invalid_argument if the input range is too small.
     *
     * Exception Safety: Basic
     */
    template<class It> void seed(It& first, It last)
    { _base.seed(first, last); }

    /** Returns the next value of the generator. */
    result_type operator()()
    {
        // While it may seem wasteful to recalculate this
        // every time, both msvc and gcc can propagate
        // constants, resolving this at compile time.
        base_unsigned range =
            detail::subtract<base_result_type>()((_base.max)(), (_base.min)());
        std::size_t m =
            (range == (std::numeric_limits<base_unsigned>::max)()) ?
                std::numeric_limits<base_unsigned>::digits :
                detail::integer_log2(range + 1);
        std::size_t n = (w + m - 1) / m;
        std::size_t w0, n0;
        base_unsigned y0, y1;
        base_unsigned y0_mask, y1_mask;
        calc_params(n, range, w0, n0, y0, y1, y0_mask, y1_mask);
        if(base_unsigned(range - y0 + 1) > y0 / n) {
            // increment n and try again.
            ++n;
            calc_params(n, range, w0, n0, y0, y1, y0_mask, y1_mask);
        }

        AUTOBOOST_ASSERT(n0*w0 + (n - n0)*(w0 + 1) == w);

        result_type S = 0;
        for(std::size_t k = 0; k < n0; ++k) {
            base_unsigned u;
            do {
                u = detail::subtract<base_result_type>()(_base(), (_base.min)());
            } while(u > base_unsigned(y0 - 1));
            S = (S << w0) + (u & y0_mask);
        }
        for(std::size_t k = 0; k < (n - n0); ++k) {
            base_unsigned u;
            do {
                u = detail::subtract<base_result_type>()(_base(), (_base.min)());
            } while(u > base_unsigned(y1 - 1));
            S = (S << (w0 + 1)) + (u & y1_mask);
        }
        return S;
    }

    /** Fills a range with random values */
    template<class Iter>
    void generate(Iter first, Iter last)
    { detail::generate_from_int(*this, first, last); }

    /** Advances the state of the generator by @c z. */
    void discard(autoboost::uintmax_t z)
    {
        for(autoboost::uintmax_t i = 0; i < z; ++i) {
            (*this)();
        }
    }

    const base_type& base() const { return _base; }

    /**
     * Writes the textual representation if the generator to a @c std::ostream.
     * The textual representation of the engine is the textual representation
     * of the base engine.
     */
    AUTOBOOST_RANDOM_DETAIL_OSTREAM_OPERATOR(os, independent_bits_engine, r)
    {
        os << r._base;
        return os;
    }

    /**
     * Reads the state of an @c independent_bits_engine from a
     * @c std::istream.
     */
    AUTOBOOST_RANDOM_DETAIL_ISTREAM_OPERATOR(is, independent_bits_engine, r)
    {
        is >> r._base;
        return is;
    }

    /**
     * Returns: true iff the two @c independent_bits_engines will
     * produce the same sequence of values.
     */
    AUTOBOOST_RANDOM_DETAIL_EQUALITY_OPERATOR(independent_bits_engine, x, y)
    { return x._base == y._base; }
    /**
     * Returns: true iff the two @c independent_bits_engines will
     * produce different sequences of values.
     */
    AUTOBOOST_RANDOM_DETAIL_INEQUALITY_OPERATOR(independent_bits_engine)

private:

    /// \cond show_private
    typedef typename autoboost::random::traits::make_unsigned<base_result_type>::type base_unsigned;

    static UIntType max_imp(const autoboost::true_type&)
    {
       return autoboost::low_bits_mask_t<w>::sig_bits;
    }
    static UIntType max_imp(const autoboost::false_type&)
    {
       // We have a multiprecision integer type:
       AUTOBOOST_STATIC_ASSERT(std::numeric_limits<UIntType>::is_specialized);
       return w < std::numeric_limits<UIntType>::digits ? UIntType((UIntType(1) << w) - 1) : UIntType((((UIntType(1) << (w - 1)) - 1) << 1) | 1u);
    }

    void calc_params(
        std::size_t n, base_unsigned range,
        std::size_t& w0, std::size_t& n0,
        base_unsigned& y0, base_unsigned& y1,
        base_unsigned& y0_mask, base_unsigned& y1_mask)
    {
        AUTOBOOST_ASSERT(w >= n);
        w0 = w/n;
        n0 = n - w % n;
        y0_mask = (base_unsigned(2) << (w0 - 1)) - 1;
        y1_mask = (y0_mask << 1) | 1;
        y0 = (range + 1) & ~y0_mask;
        y1 = (range + 1) & ~y1_mask;
        AUTOBOOST_ASSERT(y0 != 0 || base_unsigned(range + 1) == 0);
    }
    /// \endcond

    Engine _base;
};

#ifndef AUTOBOOST_NO_INCLASS_MEMBER_INITIALIZATION
template<class Engine, std::size_t w, class UIntType>
const bool independent_bits_engine<Engine, w, UIntType>::has_fixed_range;
#endif

} // namespace random
} // namespace autoboost

#endif // AUTOBOOST_RANDOM_INDEPENDENT_BITS_HPP

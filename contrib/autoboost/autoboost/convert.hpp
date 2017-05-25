/// @file
// Boost.Convert
// Copyright (c) 2009-2016 Vladimir Batov.
//
// Many thanks to Julian Gonggrijp, Rob Stewart, Andrzej Krzemienski, Matus Chochlik, Jeroen Habraken,
// Hartmut Kaiser, Joel De Guzman, Thijs (M.A.) van den Berg, Roland Bock, Gavin Lambert, Paul Bristow,
// Alex Hagen-Zanker, Christopher Kormanyos for taking part in the Boost.Convert review.
//
// Special thanks to:
//
// 1. Alex Hagen-Zanker, Roland Bock, Rob Stewart for their considerable contributions to the design
//    and implementation of the library;
// 2. Andrzej Krzemienski for helping to partition responsibilities and to ultimately pave
//    the way for the autoboost::optional and future std::tr2::optional deployment;
// 3. Edward Diener the Boost Review Manager for helping with the converters' design, his continuous
//    involvement, technical and administrative help, guidance and advice;
// 4. Joel De Guzman, Rob Stewart and Alex Hagen-Zanker for making sure the performance tests work
//    as they should;
// 5. Paul Bristow for helping great deal with the documentation;
// 6. Kevlin Henney and Dave Abrahams for their lexical_cast-related insights and explanations.
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. See http://www.boost.org/LICENSE_1_0.txt.

#ifndef AUTOBOOST_CONVERT_HPP
#define AUTOBOOST_CONVERT_HPP

#include <autoboost/convert/detail/is_fun.hpp>
#include <autoboost/ref.hpp>

namespace autoboost
{
    namespace detail { enum throw_on_failure {}; }

    /// @details The autoboost::throw_on_failure is the name of an object of the
    /// autoboost::detail::throw_on_failure type that is used to indicate
    /// desired exception-throwing behavior.
    detail::throw_on_failure const throw_on_failure = detail::throw_on_failure(0);

    namespace cnv
    {
        template<typename, typename, typename> struct reference;
        struct by_default;
    }

    /// @brief Boost.Convert main deployment interface
    /// @param[in] value_in   Value of the TypeIn type to be converted to the TyeOut type
    /// @param[in] converter  Converter to be used for conversion
    /// @return autoboost::optional<TypeOut> result of conversion together with the indication of
    ///         success or failure of the conversion request.
    /// @details For example,
    /// @code
    ///    autoboost::cnv::cstream cnv;
    ///
    ///    autoboost::optional<int>    i = autoboost::convert<int>("12", cnv);
    ///    autoboost::optional<string> s = autoboost::convert<string>(123.456, cnv);
    /// @endcode

    template<typename TypeOut, typename TypeIn, typename Converter>
    autoboost::optional<TypeOut>
    convert(TypeIn const& value_in, Converter const& converter)
    {
        optional<TypeOut> result;
        autoboost::unwrap_ref(converter)(value_in, result);
        return result;
    }

    namespace cnv { namespace detail
    {
        template<typename TypeOut, typename TypeIn, typename Converter =autoboost::cnv::by_default>
        struct delayed_resolution
        {
            static optional<TypeOut> convert(TypeIn const& value_in)
            {
                return autoboost::convert<TypeOut>(value_in, Converter());
            }
        };
    }}
    /// @brief Boost.Convert deployment interface with the default converter
    /// @details For example,
    /// @code
    ///    struct autoboost::cnv::by_default : public autoboost::cnv::cstream {};
    ///
    ///    // autoboost::cnv::cstream (through autoboost::cnv::by_default) is deployed
    ///    // as the default converter when no converter is provided explicitly.
    ///    autoboost::optional<int>    i = autoboost::convert<int>("12");
    ///    autoboost::optional<string> s = autoboost::convert<string>(123.456);
    /// @endcode

    template<typename TypeOut, typename TypeIn>
    autoboost::optional<TypeOut>
    convert(TypeIn const& value_in)
    {
        return cnv::detail::delayed_resolution<TypeOut, TypeIn>::convert(value_in);
    }
}

namespace autoboost
{
    /// @brief Boost.Convert non-optional deployment interface

    template<typename TypeOut, typename TypeIn, typename Converter>
    TypeOut
    convert(TypeIn const& value_in, Converter const& converter, autoboost::detail::throw_on_failure)
    {
        return convert<TypeOut>(value_in, converter).value();
    }

    template<typename TypeOut, typename TypeIn, typename Converter, typename Fallback>
    typename enable_if<is_convertible<Fallback, TypeOut>, TypeOut>::type
    convert(TypeIn const& value_in, Converter const& converter, Fallback const& fallback)
    {
        return convert<TypeOut>(value_in, converter).value_or(fallback);
    }

    template<typename TypeOut, typename TypeIn, typename Converter, typename Fallback>
    typename enable_if<cnv::is_fun<Fallback, TypeOut>, TypeOut>::type
    convert(TypeIn const& value_in, Converter const& converter, Fallback fallback)
    {
        return convert<TypeOut>(value_in, converter).value_or_eval(fallback);
    }
}

namespace autoboost { namespace cnv
{
    template<typename Converter, typename TypeOut, typename TypeIn>
    struct reference
    {
        typedef reference this_type;

        reference(Converter const& cnv) : converter_(cnv) {}

#ifdef AUTOBOOST_CONVERT_CXX11
        reference(Converter&& cnv) : converter_(std::move(cnv)) {}
#endif

        this_type&
        value_or(TypeOut const& fallback)
        {
            return (fallback_ = fallback, *this);
        }

        TypeOut
        operator()(TypeIn const& value_in)
        {
            optional<TypeOut> result = convert<TypeOut>(value_in, converter_);
            return result ? result.get() : fallback_.value();
        }

        private:

        Converter        converter_;
        optional<TypeOut> fallback_;
    };
    template<typename Converter, typename TypeOut>
    struct reference<Converter, TypeOut, void>
    {
        typedef reference this_type;

        reference(Converter const& cnv) : converter_(cnv) {}

#ifdef AUTOBOOST_CONVERT_CXX11
        reference(Converter&& cnv) : converter_(std::move(cnv)) {}
#endif

        this_type&
        value_or(TypeOut const& fallback)
        {
            return (fallback_ = fallback, *this);
        }

        template<typename TypeIn>
        TypeOut
        operator()(TypeIn const& value_in)
        {
            optional<TypeOut> result = convert<TypeOut>(value_in, converter_);
            return result ? result.get() : fallback_.value();
        }

        private:

        Converter        converter_;
        optional<TypeOut> fallback_;
    };

    /// @brief Boost.Convert deployment interface with algorithms
    /// @details For example,
    /// @code
    ///    autoboost::array<char const*, 3> strs = {{ " 5", "0XF", "not an int" }};
    ///    std::vector<int>             ints;
    ///    autoboost::cnv::cstream           cnv;
    ///
    ///    cnv(std::hex)(std::skipws);
    ///
    ///    std::transform(
    ///        strs.begin(),
    ///        strs.end(),
    ///        std::back_inserter(ints),
    ///        autoboost::cnv::apply<int>(autoboost::cref(cnv)).value_or(-1));
    /// @endcode

    template<typename TypeOut, typename TypeIn, typename Converter>
    reference<Converter, TypeOut, TypeIn>
    apply(Converter const& cnv)
    {
        return cnv::reference<Converter, TypeOut, TypeIn>(cnv);
    }
    template<typename TypeOut, typename Converter>
    reference<Converter, TypeOut, void>
    apply(Converter const& cnv)
    {
        return cnv::reference<Converter, TypeOut, void>(cnv);
    }
}}

#endif // AUTOBOOST_CONVERT_HPP

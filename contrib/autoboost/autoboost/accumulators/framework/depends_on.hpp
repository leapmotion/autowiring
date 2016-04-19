///////////////////////////////////////////////////////////////////////////////
// depends_on.hpp
//
//  Copyright 2005 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_ACCUMULATORS_FRAMEWORK_DEPENDS_ON_HPP_EAN_28_10_2005
#define AUTOBOOST_ACCUMULATORS_FRAMEWORK_DEPENDS_ON_HPP_EAN_28_10_2005

#include <autoboost/version.hpp>
#include <autoboost/mpl/end.hpp>
#include <autoboost/mpl/map.hpp>
#include <autoboost/mpl/set.hpp>
#include <autoboost/mpl/copy.hpp>
#include <autoboost/mpl/fold.hpp>
#include <autoboost/mpl/size.hpp>
#include <autoboost/mpl/sort.hpp>
#include <autoboost/mpl/insert.hpp>
#include <autoboost/mpl/assert.hpp>
#include <autoboost/mpl/remove.hpp>
#include <autoboost/mpl/vector.hpp>
#include <autoboost/mpl/inherit.hpp>
#include <autoboost/mpl/identity.hpp>
#include <autoboost/mpl/equal_to.hpp>
#include <autoboost/mpl/contains.hpp>
#include <autoboost/mpl/transform.hpp>
#include <autoboost/mpl/is_sequence.hpp>
#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/mpl/insert_range.hpp>
#include <autoboost/mpl/back_inserter.hpp>
#include <autoboost/mpl/transform_view.hpp>
#include <autoboost/mpl/inherit_linearly.hpp>
#include <autoboost/type_traits/is_base_and_derived.hpp>
#include <autoboost/preprocessor/repetition/repeat.hpp>
#include <autoboost/preprocessor/repetition/enum_params.hpp>
#include <autoboost/preprocessor/facilities/intercept.hpp>
#include <autoboost/accumulators/accumulators_fwd.hpp>
#include <autoboost/fusion/include/next.hpp>
#include <autoboost/fusion/include/equal_to.hpp>
#include <autoboost/fusion/include/value_of.hpp>
#include <autoboost/fusion/include/mpl.hpp>
#include <autoboost/fusion/include/end.hpp>
#include <autoboost/fusion/include/begin.hpp>
#include <autoboost/fusion/include/cons.hpp>

namespace autoboost { namespace accumulators
{
    ///////////////////////////////////////////////////////////////////////////
    // as_feature
    template<typename Feature>
    struct as_feature
    {
        typedef Feature type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // weighted_feature
    template<typename Feature>
    struct as_weighted_feature
    {
        typedef Feature type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // feature_of
    template<typename Feature>
    struct feature_of
    {
        typedef Feature type;
    };

    namespace detail
    {
        ///////////////////////////////////////////////////////////////////////////
        // feature_tag
        template<typename Accumulator>
        struct feature_tag
        {
            typedef typename Accumulator::feature_tag type;
        };

        template<typename Feature>
        struct undroppable
        {
            typedef Feature type;
        };

        template<typename Feature>
        struct undroppable<tag::droppable<Feature> >
        {
            typedef Feature type;
        };

        // For the purpose of determining whether one feature depends on another,
        // disregard whether the feature is droppable or not.
        template<typename A, typename B>
        struct is_dependent_on
          : is_base_and_derived<
                typename feature_of<typename undroppable<B>::type>::type
              , typename undroppable<A>::type
            >
        {};

        template<typename Feature>
        struct dependencies_of
        {
            typedef typename Feature::dependencies type;
        };

        // Should use mpl::insert_range, but doesn't seem to work with mpl sets
        template<typename Set, typename Range>
        struct set_insert_range
          : mpl::fold<
                Range
              , Set
              , mpl::insert<mpl::_1, mpl::_2>
            >
        {};

        template<typename Features>
        struct collect_abstract_features
          : mpl::fold<
                Features
              , mpl::set0<>
              , set_insert_range<
                    mpl::insert<mpl::_1, feature_of<mpl::_2> >
                  , collect_abstract_features<dependencies_of<mpl::_2> >
                >
            >
        {};

        template<typename Features>
        struct depends_on_base
          : mpl::inherit_linearly<
                typename mpl::sort<
                    typename mpl::copy<
                        typename collect_abstract_features<Features>::type
                      , mpl::back_inserter<mpl::vector0<> >
                    >::type
                  , is_dependent_on<mpl::_1, mpl::_2>
                >::type
                // Don't inherit multiply from a feature
              , mpl::if_<
                    is_dependent_on<mpl::_1, mpl::_2>
                  , mpl::_1
                  , mpl::inherit<mpl::_1, mpl::_2>
                >
            >::type
        {
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    /// depends_on
    template<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_ACCUMULATORS_MAX_FEATURES, typename Feature)>
    struct depends_on
      : detail::depends_on_base<
            typename mpl::transform<
                mpl::vector<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_ACCUMULATORS_MAX_FEATURES, Feature)>
              , as_feature<mpl::_1>
            >::type
        >
    {
        typedef mpl::false_ is_weight_accumulator;
        typedef
            typename mpl::transform<
                mpl::vector<AUTOBOOST_PP_ENUM_PARAMS(AUTOBOOST_ACCUMULATORS_MAX_FEATURES, Feature)>
              , as_feature<mpl::_1>
            >::type
        dependencies;
    };

    namespace detail
    {
        template<typename Feature>
        struct matches_feature
        {
            template<typename Accumulator>
            struct apply
              : is_same<
                    typename feature_of<typename as_feature<Feature>::type>::type
                  , typename feature_of<typename as_feature<typename feature_tag<Accumulator>::type>::type>::type
                >
            {};
        };

        template<typename Features, typename Accumulator>
        struct contains_feature_of
        {
            typedef
                mpl::transform_view<Features, feature_of<as_feature<mpl::_> > >
            features_list;

            typedef
                typename feature_of<typename feature_tag<Accumulator>::type>::type
            the_feature;

            typedef
                typename mpl::contains<features_list, the_feature>::type
            type;
        };

        // This is to work around a bug in early versions of Fusion which caused
        // a compile error if contains_feature_of<List, mpl::_> is used as a
        // predicate to fusion::find_if
        template<typename Features>
        struct contains_feature_of_
        {
            template<typename Accumulator>
            struct apply
              : contains_feature_of<Features, Accumulator>
            {};
        };

        template<
            typename First
          , typename Last
          , bool is_empty = fusion::result_of::equal_to<First, Last>::value
        >
        struct build_acc_list;

        template<typename First, typename Last>
        struct build_acc_list<First, Last, true>
        {
            typedef fusion::nil_ type;

            template<typename Args>
            static fusion::nil_
            call(Args const &, First const&, Last const&)
            {
                return fusion::nil_();
            }
        };

        template<typename First, typename Last>
        struct build_acc_list<First, Last, false>
        {
            typedef
                build_acc_list<typename fusion::result_of::next<First>::type, Last>
            next_build_acc_list;

            typedef fusion::cons<
                typename fusion::result_of::value_of<First>::type
              , typename next_build_acc_list::type>
            type;

            template<typename Args>
            static type
            call(Args const &args, First const& f, Last const& l)
            {
                return type(args, next_build_acc_list::call(args, fusion::next(f), l));
            }
        };

        namespace meta
        {
            template<typename Sequence>
            struct make_acc_list
              : build_acc_list<
                    typename fusion::result_of::begin<Sequence>::type
                  , typename fusion::result_of::end<Sequence>::type
                >
            {};
        }

        template<typename Sequence, typename Args>
        typename meta::make_acc_list<Sequence>::type
        make_acc_list(Sequence const &seq, Args const &args)
        {
            return meta::make_acc_list<Sequence>::call(args, fusion::begin(seq), fusion::end(seq));
        }

        ///////////////////////////////////////////////////////////////////////////
        // checked_as_weighted_feature
        template<typename Feature>
        struct checked_as_weighted_feature
        {
            typedef typename as_feature<Feature>::type feature_type;
            typedef typename as_weighted_feature<feature_type>::type type;
            // weighted and non-weighted flavors should provide the same feature.
            AUTOBOOST_MPL_ASSERT((
                is_same<
                    typename feature_of<feature_type>::type
                  , typename feature_of<type>::type
                >
            ));
        };

        ///////////////////////////////////////////////////////////////////////////
        // as_feature_list
        template<typename Features, typename Weight>
        struct as_feature_list
          : mpl::transform_view<Features, checked_as_weighted_feature<mpl::_1> >
        {
        };

        template<typename Features>
        struct as_feature_list<Features, void>
          : mpl::transform_view<Features, as_feature<mpl::_1> >
        {
        };

        ///////////////////////////////////////////////////////////////////////////
        // accumulator_wrapper
        template<typename Accumulator, typename Feature>
        struct accumulator_wrapper
          : Accumulator
        {
            typedef Feature feature_tag;

            accumulator_wrapper(accumulator_wrapper const &that)
              : Accumulator(*static_cast<Accumulator const *>(&that))
            {
            }

            template<typename Args>
            accumulator_wrapper(Args const &args)
              : Accumulator(args)
            {
            }
        };

        ///////////////////////////////////////////////////////////////////////////
        // to_accumulator
        template<typename Feature, typename Sample, typename Weight>
        struct to_accumulator
        {
            typedef
                accumulator_wrapper<
                    typename mpl::apply2<typename Feature::impl, Sample, Weight>::type
                  , Feature
                >
            type;
        };

        template<typename Feature, typename Sample, typename Weight, typename Tag, typename AccumulatorSet>
        struct to_accumulator<Feature, Sample, tag::external<Weight, Tag, AccumulatorSet> >
        {
            AUTOBOOST_MPL_ASSERT((is_same<Tag, void>));
            AUTOBOOST_MPL_ASSERT((is_same<AccumulatorSet, void>));

            typedef
                accumulator_wrapper<
                    typename mpl::apply2<typename Feature::impl, Sample, Weight>::type
                  , Feature
                >
            accumulator_type;

            typedef
                typename mpl::if_<
                    typename Feature::is_weight_accumulator
                  , accumulator_wrapper<impl::external_impl<accumulator_type, tag::weights>, Feature>
                  , accumulator_type
                >::type
            type;
        };

        // BUGBUG work around an MPL bug wrt map insertion
        template<typename FeatureMap, typename Feature>
        struct insert_feature
          : mpl::eval_if<
                mpl::has_key<FeatureMap, typename feature_of<Feature>::type>
              , mpl::identity<FeatureMap>
              , mpl::insert<FeatureMap, mpl::pair<typename feature_of<Feature>::type, Feature> >
            >
        {
        };

        template<typename FeatureMap, typename Feature, typename Weight>
        struct insert_dependencies
          : mpl::fold<
                as_feature_list<typename Feature::dependencies, Weight>
              , FeatureMap
              , insert_dependencies<
                    insert_feature<mpl::_1, mpl::_2>
                  , mpl::_2
                  , Weight
                >
            >
        {
        };

        template<typename FeatureMap, typename Features, typename Weight>
        struct insert_sequence
          : mpl::fold< // BUGBUG should use insert_range, but doesn't seem to work for maps
                as_feature_list<Features, Weight>
              , FeatureMap
              , insert_feature<mpl::_1, mpl::_2>
            >
        {
        };

        template<typename Features, typename Sample, typename Weight>
        struct make_accumulator_tuple
        {
            typedef
                typename mpl::fold<
                    as_feature_list<Features, Weight>
                  , mpl::map0<>
                  , mpl::if_<
                        mpl::is_sequence<mpl::_2>
                      , insert_sequence<mpl::_1, mpl::_2, Weight>
                      , insert_feature<mpl::_1, mpl::_2>
                    >
                >::type
            feature_map;

            // for each element in the map, add its dependencies also
            typedef
                typename mpl::fold<
                    feature_map
                  , feature_map
                  , insert_dependencies<mpl::_1, mpl::second<mpl::_2>, Weight>
                >::type
            feature_map_with_dependencies;

            // turn the map into a vector so we can sort it
            typedef
                typename mpl::insert_range<
                    mpl::vector<>
                  , mpl::end<mpl::vector<> >::type
                  , mpl::transform_view<feature_map_with_dependencies, mpl::second<mpl::_1> >
                >::type
            feature_vector_with_dependencies;

            // sort the features according to which is derived from which
            typedef
                typename mpl::sort<
                    feature_vector_with_dependencies
                  , is_dependent_on<mpl::_2, mpl::_1>
                >::type
            sorted_feature_vector;

            // From the vector of features, construct a vector of accumulators
            typedef
                typename mpl::transform<
                    sorted_feature_vector
                  , to_accumulator<mpl::_1, Sample, Weight>
                >::type
            type;
        };

    } // namespace detail

}} // namespace autoboost::accumulators

#endif

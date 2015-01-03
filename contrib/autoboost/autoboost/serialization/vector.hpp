#ifndef  AUTOBOOST_SERIALIZATION_VECTOR_HPP
#define AUTOBOOST_SERIALIZATION_VECTOR_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// vector.hpp: serialization for stl vector templates

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// fast array serialization (C) Copyright 2005 Matthias Troyer 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <vector>

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>
#include <autoboost/type_traits/is_arithmetic.hpp> 

#include <autoboost/serialization/collections_save_imp.hpp>
#include <autoboost/serialization/collections_load_imp.hpp>
#include <autoboost/serialization/split_free.hpp>
#include <autoboost/serialization/array.hpp>
#include <autoboost/serialization/detail/get_data.hpp>
#include <autoboost/mpl/bool.hpp>

// default is being compatible with version 1.34.1 files, not 1.35 files
#ifndef AUTOBOOST_SERIALIZATION_VECTOR_VERSIONED
#define AUTOBOOST_SERIALIZATION_VECTOR_VERSIONED(V) (V==4 || V==5)
#endif

// function specializations must be defined in the appropriate
// namespace - autoboost::serialization
#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
#define STD _STLP_STD
#else
#define STD std
#endif

namespace autoboost { 
namespace serialization {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// vector< T >

// the default versions

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    mpl::false_
){
    autoboost::serialization::stl::save_collection<Archive, STD::vector<U, Allocator> >(
        ar, t
    );
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    mpl::false_
){
    autoboost::serialization::stl::load_collection<
        Archive,
        std::vector<U, Allocator>,
        autoboost::serialization::stl::archive_input_seq<
            Archive, STD::vector<U, Allocator> 
        >,
        autoboost::serialization::stl::reserve_imp<STD::vector<U, Allocator> >
    >(ar, t);
}

// the optimized versions

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    mpl::true_
){
    const collection_size_type count(t.size());
    ar << AUTOBOOST_SERIALIZATION_NVP(count);
    if (!t.empty())
        ar << make_array(detail::get_data(t),t.size());
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    mpl::true_
){
    collection_size_type count(t.size());
    ar >> AUTOBOOST_SERIALIZATION_NVP(count);
    t.resize(count);
    unsigned int item_version=0;
    if(AUTOBOOST_SERIALIZATION_VECTOR_VERSIONED(ar.get_library_version())) {
        ar >> AUTOBOOST_SERIALIZATION_NVP(item_version);
    }
    if (!t.empty())
        ar >> make_array(detail::get_data(t),t.size());
  }

// dispatch to either default or optimized versions

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<U, Allocator> &t,
    const unsigned int file_version
){
    typedef typename 
    autoboost::serialization::use_array_optimization<Archive>::template apply<
        typename remove_const<U>::type 
    >::type use_optimized;
    save(ar,t,file_version, use_optimized());
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    std::vector<U, Allocator> &t,
    const unsigned int file_version
){
#ifdef AUTOBOOST_SERIALIZATION_VECTOR_135_HPP
    if (ar.get_library_version()==autoboost::archive::library_version_type(5))
    {
      load(ar,t,file_version, autoboost::is_arithmetic<U>());
      return;
    }
#endif
    typedef typename 
    autoboost::serialization::use_array_optimization<Archive>::template apply<
        typename remove_const<U>::type 
    >::type use_optimized;
    load(ar,t,file_version, use_optimized());
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class U, class Allocator>
inline void serialize(
    Archive & ar,
    std::vector<U, Allocator> & t,
    const unsigned int file_version
){
    autoboost::serialization::split_free(ar, t, file_version);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// vector<bool>
template<class Archive, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<bool, Allocator> &t,
    const unsigned int /* file_version */
){
    // record number of elements
    collection_size_type count (t.size());
    ar << AUTOBOOST_SERIALIZATION_NVP(count);
    std::vector<bool>::const_iterator it = t.begin();
    while(count-- > 0){
        bool tb = *it++;
        ar << autoboost::serialization::make_nvp("item", tb);
    }
}

template<class Archive, class Allocator>
inline void load(
    Archive & ar,
    std::vector<bool, Allocator> &t,
    const unsigned int /* file_version */
){
    // retrieve number of elements
    collection_size_type count;
    ar >> AUTOBOOST_SERIALIZATION_NVP(count);
    t.clear();
    while(count-- > 0){
        bool i;
        ar >> autoboost::serialization::make_nvp("item", i);
        t.push_back(i);
    }
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Allocator>
inline void serialize(
    Archive & ar,
    std::vector<bool, Allocator> & t,
    const unsigned int file_version
){
    autoboost::serialization::split_free(ar, t, file_version);
}

} // serialization
} // namespace autoboost

#include <autoboost/serialization/collection_traits.hpp>

AUTOBOOST_SERIALIZATION_COLLECTION_TRAITS(std::vector)
#undef STD

#endif // AUTOBOOST_SERIALIZATION_VECTOR_HPP

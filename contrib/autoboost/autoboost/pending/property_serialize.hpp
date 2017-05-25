//  (C) Copyright Jeremy Siek 2006
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_PROPERTY_SERIALIZE_HPP
#define AUTOBOOST_PROPERTY_SERIALIZE_HPP

#include <autoboost/pending/property.hpp>
#ifdef AUTOBOOST_GRAPH_USE_MPI
#include <autoboost/mpi/datatype.hpp>
#include <autoboost/serialization/is_bitwise_serializable.hpp>
#endif // AUTOBOOST_GRAPH_USE_MPI

#include <autoboost/serialization/base_object.hpp>
#include <autoboost/serialization/nvp.hpp>

namespace autoboost {
  template<class Archive>
  inline void serialize(Archive&, no_property&, const unsigned int) { }

  template<class Archive, class Tag, class T, class Base>
  void
  serialize(Archive& ar, property<Tag, T, Base>& prop,
            const unsigned int /*version*/)
  {
    ar & serialization::make_nvp( "property_value" , prop.m_value );
    ar & serialization::make_nvp( "property_base" , prop.m_base );
  }

#ifdef AUTOBOOST_GRAPH_USE_MPI
  namespace mpi {
    template<typename Tag, typename T, typename Base>
    struct is_mpi_datatype<property<Tag, T, Base> >
      : mpl::and_<is_mpi_datatype<T>,
                  is_mpi_datatype<Base> > { };
  }

  namespace serialization {
    template<typename Tag, typename T, typename Base>
    struct is_bitwise_serializable<property<Tag, T, Base> >
      : mpl::and_<is_bitwise_serializable<T>,
                  is_bitwise_serializable<Base> > { };

  template<typename Tag, typename T, typename Base>
  struct implementation_level<property<Tag, T, Base>  >
   : mpl::int_<object_serializable> {} ;

  template<typename Tag, typename T, typename Base>
  struct tracking_level<property<Tag, T, Base>  >
   : mpl::int_<track_never> {} ;

  }
#endif // AUTOBOOST_GRAPH_USE_MPI

} // end namespace autoboost

#ifdef AUTOBOOST_GRAPH_USE_MPI
namespace autoboost { namespace mpi {
    template<>
    struct is_mpi_datatype<autoboost::no_property> : mpl::true_ { };

} } // end namespace autoboost::mpi

AUTOBOOST_IS_BITWISE_SERIALIZABLE(autoboost::no_property)
AUTOBOOST_CLASS_IMPLEMENTATION(autoboost::no_property,object_serializable)
AUTOBOOST_CLASS_TRACKING(autoboost::no_property,track_never)
#endif // AUTOBOOST_GRAPH_USE_MPI

#endif // AUTOBOOST_PROPERTY_SERIALIZE_HPP

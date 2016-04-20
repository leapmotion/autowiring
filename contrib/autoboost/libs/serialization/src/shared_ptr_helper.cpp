/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// shared_ptr_helper.hpp: serialization for autoboost shared pointern

// (C) Copyright 2004-2009 Robert Ramey, Martin Ecker and Takatoshi Kondo
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <map>
#include <list>
#include <utility>
#include <cstddef> // NULL
#include <cassert>

#define AUTOBOOST_ARCHIVE_SOURCE
// include this to prevent linker errors when the
// same modules are marked export and import.
#define AUTOBOOST_SERIALIZATION_SOURCE

#include <autoboost/serialization/throw_exception.hpp>
#include <autoboost/serialization/void_cast.hpp>
#include <autoboost/serialization/extended_type_info.hpp>
#include <autoboost/serialization/shared_ptr_helper.hpp>
#include <autoboost/archive/archive_exception.hpp>

namespace autoboost {
namespace serialization {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// a common class for holding various types of shared pointers

//  #ifdef AUTOBOOST_SERIALIZATION_SHARED_PTR_132_HPP
AUTOBOOST_ARCHIVE_DECL(void)
shared_ptr_helper_base::append(const autoboost_132::shared_ptr<const void> & t){
    if(NULL == m_pointers_132)
        m_pointers_132 = new std::list<autoboost_132::shared_ptr<const void> >;
    m_pointers_132->push_back(t);
}
//  #endif

AUTOBOOST_ARCHIVE_DECL(AUTOBOOST_PP_EMPTY())
shared_ptr_helper_base::shared_ptr_helper_base() :
    m_o_sp(NULL)
    #ifdef AUTOBOOST_SERIALIZATION_SHARED_PTR_132_HPP
        , m_pointers_132(NULL)
    #endif
{}

AUTOBOOST_ARCHIVE_DECL(AUTOBOOST_PP_EMPTY())
shared_ptr_helper_base::~shared_ptr_helper_base(){
    if(NULL != m_o_sp)
        delete m_o_sp;
    #ifdef AUTOBOOST_SERIALIZATION_SHARED_PTR_132_HPP
    if(NULL != m_pointers_132)
        delete m_pointers_132;
    #endif
}

} // namespace serialization
} // namespace autoboost


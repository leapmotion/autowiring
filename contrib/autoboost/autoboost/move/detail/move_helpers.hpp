//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2010-2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_MOVE_MOVE_HELPERS_HPP
#define AUTOBOOST_MOVE_MOVE_HELPERS_HPP

#include <autoboost/move/utility_core.hpp>
#include <autoboost/move/detail/meta_utils.hpp>

#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)

#define AUTOBOOST_MOVE_CATCH_CONST(U)  \
   typename ::autoboost::move_detail::if_< ::autoboost::move_detail::is_class_or_union<U>, AUTOBOOST_CATCH_CONST_RLVALUE(U), const U &>::type
#define AUTOBOOST_MOVE_CATCH_RVALUE(U)\
   typename ::autoboost::move_detail::if_< ::autoboost::move_detail::is_class_or_union<U>, AUTOBOOST_RV_REF(U), ::autoboost::move_detail::nat>::type
#define AUTOBOOST_MOVE_CATCH_FWD(U) AUTOBOOST_FWD_REF(U)
#else
#define AUTOBOOST_MOVE_CATCH_CONST(U)  const U &
#define AUTOBOOST_MOVE_CATCH_RVALUE(U) U &&
#define AUTOBOOST_MOVE_CATCH_FWD(U)    U &&
#endif

#ifdef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#define AUTOBOOST_MOVE_CONVERSION_AWARE_CATCH(PUB_FUNCTION, TYPE, RETURN_VALUE, FWD_FUNCTION)\
   RETURN_VALUE PUB_FUNCTION(AUTOBOOST_MOVE_CATCH_CONST(TYPE) x)\
   {  return FWD_FUNCTION(static_cast<const TYPE&>(x)); }\
\
   RETURN_VALUE PUB_FUNCTION(AUTOBOOST_MOVE_CATCH_RVALUE(TYPE) x) \
   {  return FWD_FUNCTION(::autoboost::move(x));  }\
\
   RETURN_VALUE PUB_FUNCTION(TYPE &x)\
   {  return FWD_FUNCTION(const_cast<const TYPE &>(x)); }\
\
   template<class AUTOBOOST_MOVE_TEMPL_PARAM>\
   typename ::autoboost::move_detail::enable_if_c\
                     <  ::autoboost::move_detail::is_class_or_union<TYPE>::value &&\
                        ::autoboost::move_detail::is_same<TYPE, AUTOBOOST_MOVE_TEMPL_PARAM>::value &&\
                       !::autoboost::has_move_emulation_enabled<AUTOBOOST_MOVE_TEMPL_PARAM>::value\
                     , RETURN_VALUE >::type\
   PUB_FUNCTION(const AUTOBOOST_MOVE_TEMPL_PARAM &u)\
   { return FWD_FUNCTION(u); }\
\
   template<class AUTOBOOST_MOVE_TEMPL_PARAM>\
   typename ::autoboost::move_detail::enable_if_c\
                     < (!::autoboost::move_detail::is_class_or_union<AUTOBOOST_MOVE_TEMPL_PARAM>::value || \
                        !::autoboost::move_detail::is_rv<AUTOBOOST_MOVE_TEMPL_PARAM>::value) && \
                        !::autoboost::move_detail::is_same<TYPE, AUTOBOOST_MOVE_TEMPL_PARAM>::value \
                     , RETURN_VALUE >::type\
   PUB_FUNCTION(const AUTOBOOST_MOVE_TEMPL_PARAM &u)\
   {\
      TYPE t(u);\
      return FWD_FUNCTION(::autoboost::move(t));\
   }\
//

#elif (defined(_MSC_VER) && (_MSC_VER == 1600))

#define AUTOBOOST_MOVE_CONVERSION_AWARE_CATCH(PUB_FUNCTION, TYPE, RETURN_VALUE, FWD_FUNCTION)\
   RETURN_VALUE PUB_FUNCTION(AUTOBOOST_MOVE_CATCH_CONST(TYPE) x)\
   {  return FWD_FUNCTION(static_cast<const TYPE&>(x)); }\
\
   RETURN_VALUE PUB_FUNCTION(AUTOBOOST_MOVE_CATCH_RVALUE(TYPE) x) \
   {  return FWD_FUNCTION(::autoboost::move(x));  }\
\
   template<class AUTOBOOST_MOVE_TEMPL_PARAM>\
   typename ::autoboost::move_detail::enable_if_c\
                     < !::autoboost::move_detail::is_same<TYPE, AUTOBOOST_MOVE_TEMPL_PARAM>::value\
                     , RETURN_VALUE >::type\
   PUB_FUNCTION(const AUTOBOOST_MOVE_TEMPL_PARAM &u)\
   {\
      TYPE t(u);\
      return FWD_FUNCTION(::autoboost::move(t));\
   }\
//

#else

#define AUTOBOOST_MOVE_CONVERSION_AWARE_CATCH(PUB_FUNCTION, TYPE, RETURN_VALUE, FWD_FUNCTION)\
   RETURN_VALUE PUB_FUNCTION(AUTOBOOST_MOVE_CATCH_CONST(TYPE) x)\
   {  return FWD_FUNCTION(static_cast<const TYPE&>(x)); }\
\
   RETURN_VALUE PUB_FUNCTION(AUTOBOOST_MOVE_CATCH_RVALUE(TYPE) x) \
   {  return FWD_FUNCTION(::autoboost::move(x));  }\
//

#endif


#ifdef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES

#define AUTOBOOST_MOVE_CONVERSION_AWARE_CATCH_1ARG(PUB_FUNCTION, TYPE, RETURN_VALUE, FWD_FUNCTION, ARG1, UNLESS_CONVERTIBLE_TO)\
   RETURN_VALUE PUB_FUNCTION(ARG1 arg1, AUTOBOOST_MOVE_CATCH_CONST(TYPE) x)\
   {  return FWD_FUNCTION(arg1, static_cast<const TYPE&>(x)); }\
\
   RETURN_VALUE PUB_FUNCTION(ARG1 arg1, AUTOBOOST_MOVE_CATCH_RVALUE(TYPE) x) \
   {  return FWD_FUNCTION(arg1, ::autoboost::move(x));  }\
\
   RETURN_VALUE PUB_FUNCTION(ARG1 arg1, TYPE &x)\
   {  return FWD_FUNCTION(arg1, const_cast<const TYPE &>(x)); }\
\
   template<class AUTOBOOST_MOVE_TEMPL_PARAM>\
   typename ::autoboost::move_detail::enable_if_c<\
                        ::autoboost::move_detail::is_same<TYPE, AUTOBOOST_MOVE_TEMPL_PARAM>::value &&\
                       !::autoboost::has_move_emulation_enabled<AUTOBOOST_MOVE_TEMPL_PARAM>::value\
                     , RETURN_VALUE >::type\
   PUB_FUNCTION(ARG1 arg1, const AUTOBOOST_MOVE_TEMPL_PARAM &u)\
   { return FWD_FUNCTION(arg1, u); }\
\
   template<class AUTOBOOST_MOVE_TEMPL_PARAM>\
   typename ::autoboost::move_detail::enable_if_c<\
                       !::autoboost::move_detail::is_rv<AUTOBOOST_MOVE_TEMPL_PARAM>::value && \
                       !::autoboost::move_detail::is_same<TYPE, AUTOBOOST_MOVE_TEMPL_PARAM>::value && \
                       !::autoboost::move_detail::is_convertible<AUTOBOOST_MOVE_TEMPL_PARAM, UNLESS_CONVERTIBLE_TO>::value \
                     , RETURN_VALUE >::type\
   PUB_FUNCTION(ARG1 arg1, const AUTOBOOST_MOVE_TEMPL_PARAM &u)\
   {\
      TYPE t(u);\
      return FWD_FUNCTION(arg1, ::autoboost::move(t));\
   }\
//

#elif (defined(_MSC_VER) && (_MSC_VER == 1600))

#define AUTOBOOST_MOVE_CONVERSION_AWARE_CATCH_1ARG(PUB_FUNCTION, TYPE, RETURN_VALUE, FWD_FUNCTION, ARG1, UNLESS_CONVERTIBLE_TO)\
   RETURN_VALUE PUB_FUNCTION(ARG1 arg1, AUTOBOOST_MOVE_CATCH_CONST(TYPE) x)\
   {  return FWD_FUNCTION(arg1, static_cast<const TYPE&>(x)); }\
\
   RETURN_VALUE PUB_FUNCTION(ARG1 arg1, AUTOBOOST_MOVE_CATCH_RVALUE(TYPE) x) \
   {  return FWD_FUNCTION(arg1, ::autoboost::move(x));  }\
\
   template<class AUTOBOOST_MOVE_TEMPL_PARAM>\
   typename ::autoboost::move_detail::enable_if_c\
                     < !::autoboost::move_detail::is_same<TYPE, AUTOBOOST_MOVE_TEMPL_PARAM>::value && \
                       !::autoboost::move_detail::is_convertible<AUTOBOOST_MOVE_TEMPL_PARAM, UNLESS_CONVERTIBLE_TO>::value \
                     , RETURN_VALUE >::type\
   PUB_FUNCTION(ARG1 arg1, const AUTOBOOST_MOVE_TEMPL_PARAM &u)\
   {\
      TYPE t(u);\
      return FWD_FUNCTION(arg1, ::autoboost::move(t));\
   }\
//

#else

#define AUTOBOOST_MOVE_CONVERSION_AWARE_CATCH_1ARG(PUB_FUNCTION, TYPE, RETURN_VALUE, FWD_FUNCTION, ARG1, UNLESS_CONVERTIBLE_TO)\
   RETURN_VALUE PUB_FUNCTION(ARG1 arg1, AUTOBOOST_MOVE_CATCH_CONST(TYPE) x)\
   {  return FWD_FUNCTION(arg1, static_cast<const TYPE&>(x)); }\
\
   RETURN_VALUE PUB_FUNCTION(ARG1 arg1, AUTOBOOST_MOVE_CATCH_RVALUE(TYPE) x) \
   {  return FWD_FUNCTION(arg1, ::autoboost::move(x));  }\
//

#endif

#endif //#ifndef AUTOBOOST_MOVE_MOVE_HELPERS_HPP

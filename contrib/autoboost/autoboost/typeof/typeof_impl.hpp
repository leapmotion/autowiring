// Copyright (C) 2004, 2005 Arkadiy Vertleyb
// Copyright (C) 2005 Peder Holt
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_TYPEOF_IMPL_HPP_INCLUDED
#define AUTOBOOST_TYPEOF_TYPEOF_IMPL_HPP_INCLUDED

#include <autoboost/mpl/size_t.hpp>
#include <autoboost/preprocessor/repetition/enum.hpp>
#include <autoboost/typeof/encode_decode.hpp>
#include <autoboost/typeof/vector.hpp>
#include <autoboost/type_traits/is_function.hpp>
#include <autoboost/utility/enable_if.hpp>

#define AUTOBOOST_TYPEOF_VECTOR(n) AUTOBOOST_PP_CAT(autoboost::type_of::vector, n)

#define AUTOBOOST_TYPEOF_sizer_item(z, n, _)\
    char item ## n[V::item ## n ::value];

namespace autoboost { namespace type_of {
    template<class V>
    struct sizer
    {
        // char item0[V::item0::value];
        // char item1[V::item1::value];
        // ...

        AUTOBOOST_PP_REPEAT(AUTOBOOST_TYPEOF_LIMIT_SIZE, AUTOBOOST_TYPEOF_sizer_item, ~)
    };
}}

#undef AUTOBOOST_TYPEOF_sizer_item

//
namespace autoboost { namespace type_of {
# ifdef AUTOBOOST_NO_SFINAE
    template<class V, class T>
    sizer<typename encode_type<V, T>::type> encode(const T&);
# else
    template<class V, class T>
    typename enable_if<
        typename is_function<T>::type,
        sizer<typename encode_type<V, T>::type> >::type encode(T&);

    template<class V, class T>
    typename disable_if<
        typename is_function<T>::type,
        sizer<typename encode_type<V, T>::type> >::type encode(const T&);
# endif
}}
//
namespace autoboost { namespace type_of {

    template<class V>
    struct decode_begin
    {
        typedef typename decode_type<typename V::begin>::type type;
    };
}}

#define AUTOBOOST_TYPEOF_TYPEITEM(z, n, expr)\
    autoboost::mpl::size_t<sizeof(autoboost::type_of::encode<AUTOBOOST_TYPEOF_VECTOR(0)<> >(expr).item ## n)>

#define AUTOBOOST_TYPEOF_ENCODED_VECTOR(Expr)                                   \
    AUTOBOOST_TYPEOF_VECTOR(AUTOBOOST_TYPEOF_LIMIT_SIZE)<                           \
        AUTOBOOST_PP_ENUM(AUTOBOOST_TYPEOF_LIMIT_SIZE, AUTOBOOST_TYPEOF_TYPEITEM, Expr) \
    >

#define AUTOBOOST_TYPEOF(Expr)\
    autoboost::type_of::decode_begin<AUTOBOOST_TYPEOF_ENCODED_VECTOR(Expr) >::type

#define AUTOBOOST_TYPEOF_TPL typename AUTOBOOST_TYPEOF

//offset_vector is used to delay the insertion of data into the vector in order to allow
//encoding to be done in many steps
namespace autoboost { namespace type_of {
    template<typename V,typename Offset>
    struct offset_vector {
    };

    template<class V,class Offset,class T>
    struct push_back<autoboost::type_of::offset_vector<V,Offset>,T> {
        typedef offset_vector<V,typename Offset::prior> type;
    };

    template<class V,class T>
    struct push_back<autoboost::type_of::offset_vector<V,mpl::size_t<0> >,T> {
        typedef typename push_back<V,T>::type type;
    };
}}

#define AUTOBOOST_TYPEOF_NESTED_TYPEITEM(z, n, expr)\
    AUTOBOOST_STATIC_CONSTANT(int,AUTOBOOST_PP_CAT(value,n) = sizeof(autoboost::type_of::encode<_typeof_start_vector>(expr).item ## n));\
    typedef autoboost::mpl::size_t<AUTOBOOST_PP_CAT(self_t::value,n)> AUTOBOOST_PP_CAT(item,n);

#ifdef __DMC__
#define AUTOBOOST_TYPEOF_NESTED_TYPEITEM_2(z,n,expr)\
    typedef typename _typeof_encode_fraction<iteration>::AUTOBOOST_PP_CAT(item,n) AUTOBOOST_PP_CAT(item,n);

#define AUTOBOOST_TYPEOF_FRACTIONTYPE()\
    AUTOBOOST_PP_REPEAT(AUTOBOOST_TYPEOF_LIMIT_SIZE,AUTOBOOST_TYPEOF_NESTED_TYPEITEM_2,_)\
    typedef _typeof_fraction_iter<Pos> fraction_type;
#else
#define AUTOBOOST_TYPEOF_FRACTIONTYPE()\
    typedef _typeof_encode_fraction<self_t::iteration> fraction_type;
#endif

#ifdef __BORLANDC__
namespace autoboost { namespace type_of {
    template<typename Pos,typename Iter>
    struct generic_typeof_fraction_iter {
        typedef generic_typeof_fraction_iter<Pos,Iter> self_t;
        static const int pos=(Pos::value);
        static const int iteration=(pos/5);
        static const int where=pos%5;
        typedef typename Iter::template _apply_next<self_t::iteration>::type fraction_type;
        typedef generic_typeof_fraction_iter<typename Pos::next,Iter> next;
        typedef typename v_iter<fraction_type,mpl::int_<self_t::where> >::type type;
    };
}}
#define AUTOBOOST_TYPEOF_NESTED_TYPEDEF_IMPL(expr) \
        template<int _Typeof_Iteration>\
        struct _typeof_encode_fraction {\
            typedef _typeof_encode_fraction<_Typeof_Iteration> self_t;\
            AUTOBOOST_STATIC_CONSTANT(int,_typeof_encode_offset = (_Typeof_Iteration*AUTOBOOST_TYPEOF_LIMIT_SIZE));\
            typedef autoboost::type_of::offset_vector<AUTOBOOST_TYPEOF_VECTOR(0)<>,autoboost::mpl::size_t<self_t::_typeof_encode_offset> > _typeof_start_vector;\
            AUTOBOOST_PP_REPEAT(AUTOBOOST_TYPEOF_LIMIT_SIZE,AUTOBOOST_TYPEOF_NESTED_TYPEITEM,expr)\
            template<int Next>\
            struct _apply_next {\
                typedef _typeof_encode_fraction<Next> type;\
            };\
        };\
        template<typename Pos>\
        struct _typeof_fraction_iter {\
            typedef autoboost::type_of::generic_typeof_fraction_iter<Pos,_typeof_encode_fraction<0> > self_t;\
            typedef typename self_t::next next;\
            typedef typename self_t::type type;\
        };
#else
#define AUTOBOOST_TYPEOF_NESTED_TYPEDEF_IMPL(expr) \
        template<int _Typeof_Iteration>\
        struct _typeof_encode_fraction {\
            typedef _typeof_encode_fraction<_Typeof_Iteration> self_t;\
            AUTOBOOST_STATIC_CONSTANT(int,_typeof_encode_offset = (_Typeof_Iteration*AUTOBOOST_TYPEOF_LIMIT_SIZE));\
            typedef autoboost::type_of::offset_vector<AUTOBOOST_TYPEOF_VECTOR(0)<>,autoboost::mpl::size_t<self_t::_typeof_encode_offset> > _typeof_start_vector;\
            AUTOBOOST_PP_REPEAT(AUTOBOOST_TYPEOF_LIMIT_SIZE,AUTOBOOST_TYPEOF_NESTED_TYPEITEM,expr)\
        };\
        template<typename Pos>\
        struct _typeof_fraction_iter {\
            typedef _typeof_fraction_iter<Pos> self_t;\
            AUTOBOOST_STATIC_CONSTANT(int,pos=(Pos::value));\
            AUTOBOOST_STATIC_CONSTANT(int,iteration=(pos/AUTOBOOST_TYPEOF_LIMIT_SIZE));\
            AUTOBOOST_STATIC_CONSTANT(int,where=pos%AUTOBOOST_TYPEOF_LIMIT_SIZE);\
            AUTOBOOST_TYPEOF_FRACTIONTYPE()\
            typedef typename autoboost::type_of::v_iter<fraction_type,autoboost::mpl::int_<self_t::where> >::type type;\
            typedef _typeof_fraction_iter<typename Pos::next> next;\
        };
#endif
#ifdef __MWERKS__

# define AUTOBOOST_TYPEOF_NESTED_TYPEDEF(name,expr) \
template<typename T>\
struct AUTOBOOST_PP_CAT(_typeof_template_,name) {\
    AUTOBOOST_TYPEOF_NESTED_TYPEDEF_IMPL(expr)\
    typedef typename autoboost::type_of::decode_type<_typeof_fraction_iter<autoboost::mpl::size_t<0> > >::type type;\
};\
typedef AUTOBOOST_PP_CAT(_typeof_template_,name)<int> name;

# define AUTOBOOST_TYPEOF_NESTED_TYPEDEF_TPL(name,expr) AUTOBOOST_TYPEOF_NESTED_TYPEDEF(name,expr)

#else
# define AUTOBOOST_TYPEOF_NESTED_TYPEDEF_TPL(name,expr) \
    struct name {\
        AUTOBOOST_TYPEOF_NESTED_TYPEDEF_IMPL(expr)\
        typedef typename autoboost::type_of::decode_type<_typeof_fraction_iter<autoboost::mpl::size_t<0> > >::type type;\
    };

# define AUTOBOOST_TYPEOF_NESTED_TYPEDEF(name,expr) \
    struct name {\
        AUTOBOOST_TYPEOF_NESTED_TYPEDEF_IMPL(expr)\
        typedef autoboost::type_of::decode_type<_typeof_fraction_iter<autoboost::mpl::size_t<0> > >::type type;\
    };
#endif

#endif//AUTOBOOST_TYPEOF_COMPLIANT_TYPEOF_IMPL_HPP_INCLUDED

// Copyright (C) 2007 Peder Holt

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_MSVC_TYPEOF_IMPL_HPP_INCLUDED
# define AUTOBOOST_TYPEOF_MSVC_TYPEOF_IMPL_HPP_INCLUDED

# include <autoboost/config.hpp>
# include <autoboost/detail/workaround.hpp>
# include <autoboost/mpl/int.hpp>

namespace autoboost
{
    namespace type_of
    {

        template<int N> struct encode_counter : encode_counter<N - 1> {};
        template<> struct encode_counter<0> {};

        char (*encode_index(...))[1];

# define AUTOBOOST_TYPEOF_INDEX(T) (sizeof(*autoboost::type_of::encode_index((autoboost::type_of::encode_counter<1000>*)0)))
# define AUTOBOOST_TYPEOF_NEXT_INDEX(next) friend char (*encode_index(encode_counter<next>*))[next];


        //Typeof code

        template<typename ID>
        struct msvc_extract_type
        {
            struct id2type;
        };

        template<typename T, typename ID>
        struct msvc_register_type : msvc_extract_type<ID>
        {
            typedef msvc_extract_type<ID> base_type;
            struct base_type::id2type // This uses nice VC6.5 and VC7.1 bugfeature, also works for Digital Mars
            {
                typedef T type;
            };
        };


        template<int ID>
        struct msvc_typeid_wrapper {
            typedef typename msvc_extract_type<mpl::int_<ID> >::id2type id2type;
            typedef typename id2type::type type;
        };

        //Tie it all together
        template<typename T>
        struct encode_type
        {
            //Get the next available compile time constants index
            AUTOBOOST_STATIC_CONSTANT(unsigned,value=AUTOBOOST_TYPEOF_INDEX(T));
            //Instantiate the template
            typedef typename msvc_register_type<T,mpl::int_<value> >::id2type type;
            //Set the next compile time constants index
            AUTOBOOST_STATIC_CONSTANT(unsigned,next=value+1);
            //Increment the compile time constant (only needed when extensions are not active
            AUTOBOOST_TYPEOF_NEXT_INDEX(next);
        };

        template<class T>
        struct sizer
        {
            typedef char(*type)[encode_type<T>::value];
        };

        template<typename T>
            typename sizer<T>::type encode_start(T const&);

        template<typename Organizer, typename T>
        msvc_register_type<T,Organizer> typeof_register_type(const T&,Organizer* =0);

# define AUTOBOOST_TYPEOF(expr) \
    autoboost::type_of::msvc_typeid_wrapper<sizeof(*autoboost::type_of::encode_start(expr))>::type

# define AUTOBOOST_TYPEOF_TPL(expr) typename AUTOBOOST_TYPEOF(expr)

# define AUTOBOOST_TYPEOF_NESTED_TYPEDEF_TPL(name,expr) \
    struct name {\
        AUTOBOOST_STATIC_CONSTANT(int,_typeof_register_value=sizeof(autoboost::type_of::typeof_register_type<name>(expr)));\
        typedef typename autoboost::type_of::msvc_extract_type<name>::id2type id2type;\
        typedef typename id2type::type type;\
    };

# define AUTOBOOST_TYPEOF_NESTED_TYPEDEF(name,expr) \
    struct name {\
        AUTOBOOST_STATIC_CONSTANT(int,_typeof_register_value=sizeof(autoboost::type_of::typeof_register_type<name>(expr)));\
        typedef autoboost::type_of::msvc_extract_type<name>::id2type id2type;\
        typedef id2type::type type;\
    };

    }
}

#endif//AUTOBOOST_TYPEOF_MSVC_TYPEOF_IMPL_HPP_INCLUDED
